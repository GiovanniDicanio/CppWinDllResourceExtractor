//////////////////////////////////////////////////////////////////////////
// dllmain.cpp : Defines the entry point for the DLL application.
//////////////////////////////////////////////////////////////////////////

#include "pch.h"


//========================================================================
// Test function exported by this DLL
//========================================================================
extern "C" void __stdcall DllHello()
{
    // Simply invoke MessageBox to show a message to the user
    ::MessageBox(nullptr,
                 TEXT("Hello from the TestDll!"),
                 TEXT("TestDll"),
                 MB_OK);
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    UNREFERENCED_PARAMETER(hModule);
    UNREFERENCED_PARAMETER(lpReserved);

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

