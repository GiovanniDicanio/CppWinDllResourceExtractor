////////////////////////////////////////////////////////////////////////////////
// FILE: ExtractorExe.cpp
// DESC: Demo: extracts the embedded DLL, and invokes a function from it.
//
// By Giovanni Dicanio
//
////////////////////////////////////////////////////////////////////////////////


#include <Windows.h>            // Windows Platform SDK

#include <exception>            // std::exception
#include <fstream>              // std::ofstream
#include <iostream>             // std::cout, std::cerr
#include <stdexcept>            // std::runtime_error

#include "ResourceBinaryView.h" // Access the binary resource

#include "resource.h"       // The resource header that defines the resource ID for the DLL resource


//==============================================================================
// Simple scoped RAII wrapper around LoadLibrary/FreeLibrary
//==============================================================================
class ScopedDllLoader
{
public:
    explicit ScopedDllLoader(const TCHAR* dllFilename)
    {
        m_hDll = ::LoadLibrary(dllFilename);
    }

    ~ScopedDllLoader()
    {
        if (m_hDll)
        {
            ::FreeLibrary(m_hDll);
        }
    }

    HMODULE GetHModule() const
    {
        return m_hDll;
    }

    // Ban copy
    ScopedDllLoader(ScopedDllLoader const&) = delete;
    ScopedDllLoader& operator=(ScopedDllLoader const&) = delete;

private:
    HMODULE m_hDll;
};


//==============================================================================
// Application Entry-Point
//==============================================================================
int main()
{
    constexpr int kExitOk = 0;
    constexpr int kExitError = 1;

    try
    {
        std::cout << "\n *** Extracting a DLL embedded as a binary resource\n";
        std::cout << " *** by Giovanni Dicanio\n\n";

        // Access the binary resource embedded in the executable file
        ResourceBinaryView dllResource(MAKEINTRESOURCE(IDR_TEST_DLL));

        // Extract the DLL from the binary resource:
        // basically, create a binary file named TestMyDll.dll,
        // and copy the bytes from the embedded DLL resource into that output file.
        std::ofstream outDllFile("TestMyDll.dll", std::ios::out | std::ios::binary | std::ios::app);
        outDllFile.write(reinterpret_cast<const char*>(dllResource.Data()), dllResource.Size());
        outDllFile.close();

        // Load the extracted DLL
        ScopedDllLoader dll(TEXT("TestMyDll.dll"));
        if (dll.GetHModule() == nullptr)
        {
            throw std::runtime_error("Cannot load test DLL.");
        }

        // Access the test function from the DLL
        typedef void (APIENTRY* PFN_DLL_TEST_HELLO)(void);
        PFN_DLL_TEST_HELLO pfnDllHello = reinterpret_cast<PFN_DLL_TEST_HELLO>(
            ::GetProcAddress(dll.GetHModule(), "DllHello") );
        if (pfnDllHello == nullptr)
        {
            throw std::runtime_error("Cannot load test function from DLL.");
        }

        // Call the DLL test function
        pfnDllHello();

        std::cout << " All right!\n";
        return kExitOk;
    }
    catch (const std::exception& e)
    {
        std::cerr << "\n !!! ERROR: " << e.what() << '\n';
        return kExitError;
    }
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started:
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
