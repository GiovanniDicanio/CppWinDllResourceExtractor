#ifndef GIOVANNI_DICANIO_RESOURCEBINARYVIEW_H
#define GIOVANNI_DICANIO_RESOURCEBINARYVIEW_H

////////////////////////////////////////////////////////////////////////////////
//
// FILE: ResourceBinaryView.h
// DESC: C++ class to access a Windows PE resource as a read-only binary view
//
// Written by Giovanni Dicanio
//
// E-mail: <first name>.<last name> AT REMOVE_THIS gmail.com
//
// First version: 2024, March 25th
// Last update:   2024, March 25th
//
// IDE: Visual Studio 2019
// (Code compiles cleanly at at /W4 in C++14 mode)
//
// ===========================================================================
//
// The MIT License(MIT)
//
// Copyright(c) 2024 by Giovanni Dicanio
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
////////////////////////////////////////////////////////////////////////////////


#include <Windows.h>        // Windows Platform SDK

#include <string>           // std::string
#include <system_error>     // std::system_error


//==============================================================================
// An exception object of this class is thrown in case of errors occurred
// when trying to access a resource with the ResourceBinaryView class
//==============================================================================
class ResourceAccessException : public std::system_error
{
public:

    ResourceAccessException(DWORD errorCode, const char* message)
        : std::system_error(errorCode, std::system_category(), message)
    {}

    ResourceAccessException(DWORD errorCode, const std::string& message)
        : std::system_error(errorCode, std::system_category(), message)
    {}
};


//==============================================================================
// Access a binary resource as a (read-only) sequence of bytes
//==============================================================================
class ResourceBinaryView
{
public:

    // Default-initialize to a view to nothing
    ResourceBinaryView() = default;


    // Create a read-only view to a resource identified by 'resourceId'
    // and contained in the PE file associated to the HMODULE handle.
    //
    // Use MAKEINTRESOURCE(resourceId) where you have an *integer ID*
    // identifying the resource.
    explicit ResourceBinaryView(const TCHAR* resourceId, HMODULE hModule = nullptr)
    {
        Init(resourceId, hModule);
    }


    // Get read-only access the address of the first byte of the resource
    const BYTE* Data() const noexcept
    {
        return static_cast<const BYTE*>(m_data);
    }


    // Get the size of the resource, in bytes
    DWORD Size() const noexcept
    {
        return m_size;
    }


    // Same as Data, but following the STL coding convention
    // (TODO: May use C++17 std::byte here...)
    const BYTE* data() const noexcept
    {
        return Data();
    }


    // Same as Size, but following the STL coding convention
    size_t size() const noexcept
    {
        return Size();
    }


    //
    // begin, end, cbegin, cend, can be used to iterate the binary resource view
    // using range-for and standard algorithms.
    //

    const BYTE* cbegin() const noexcept
    {
        return Data();
    }

    const BYTE* cend() const noexcept
    {
        return Data() + Size();
    }

    const BYTE* begin() const noexcept
    {
        return cbegin();
    }

    const BYTE* end() const noexcept
    {
        return cend();
    }


    // Create a read-only view to a resource identified by 'resourceId'
    // and contained in the PE file associated to the HMODULE handle.
    //
    // Use MAKEINTRESOURCE(resourceId) where you have an *integer ID*
    // identifying the resource.
    void Reset(const TCHAR* resourceId, HMODULE hModule = nullptr)
    {
        Init(resourceId, hModule);
    }


    // Clear the view
    void Clear() noexcept
    {
        m_data = nullptr;
        m_size = 0;
    }


    //
    // IMPLEMENTATION
    //
private:
    const void* m_data{ nullptr };       // Address of the first byte of the resource
    DWORD m_size{ 0 };                   // Size of the resource, in bytes


    void Init(const TCHAR* resourceId, HMODULE hModule)
    {
        HRSRC hResourceInfo = ::FindResource(hModule, resourceId, RT_RCDATA);
        if (hResourceInfo == nullptr)
        {
            DWORD errorCode = ::GetLastError();
            throw ResourceAccessException(errorCode, "Cannot find the resource - FindResource failed.");
        }

        HGLOBAL hResourceData = ::LoadResource(hModule, hResourceInfo);
        if (hResourceData == nullptr)
        {
            DWORD errorCode = ::GetLastError();
            throw ResourceAccessException(errorCode, "Cannot load the resource - LoadResource failed.");
        }

        const void* pvResourceData = ::LockResource(hResourceData);
        if (pvResourceData == nullptr)
        {
            DWORD errorCode = ::GetLastError();
            throw ResourceAccessException(errorCode, "Cannot access the resource's bytes - LockResource failed.");
        }

        DWORD dwResourceSize = ::SizeofResource(hModule, hResourceInfo);
        if (dwResourceSize == 0)
        {
            DWORD errorCode = ::GetLastError();
            throw ResourceAccessException(errorCode, "Cannot get the resource size - SizeofResource failed.");
        }

        m_data = pvResourceData;
        m_size = dwResourceSize;
    }
};


#endif // GIOVANNI_DICANIO_RESOURCEBINARYVIEW_H
