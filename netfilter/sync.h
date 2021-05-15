//
// 	NetFilterSDK
// 	Copyright (C) Vitaly Sidorov
//	All rights reserved.
//
//	This file is a part of the NetFilter SDK.
//	The code and information is provided "as-is" without
//	warranty of any kind, either expressed or implied.
//

#pragma once

#include "base.h"

class CriticalSection
{
  public:
    CriticalSection() throw()
    {
        memset(&m_sec, 0, sizeof(CRITICAL_SECTION));
        InitializeCriticalSection(&m_sec);
    }
    ~CriticalSection()
    {
        DeleteCriticalSection(&m_sec);
    }
    HRESULT Lock() throw()
    {
        EnterCriticalSection(&m_sec);
        return S_OK;
    }
    HRESULT Unlock() throw()
    {
        LeaveCriticalSection(&m_sec);
        return S_OK;
    }

  private:
    CRITICAL_SECTION m_sec;
};

class AutoLock
{
  public:
    AutoLock(CriticalSection &cs) : m_cs(cs)
    {
        m_cs.Lock();
    }

    virtual ~AutoLock()
    {
        m_cs.Unlock();
    }

  private:
    CriticalSection &m_cs;
};

class AutoHandle
{
  public:
    AutoHandle() throw() : m_h(INVALID_HANDLE_VALUE)
    {
    }
    AutoHandle(AutoHandle &h) throw() : m_h(INVALID_HANDLE_VALUE)
    {
        Attach(h.Detach());
    }
    explicit AutoHandle(HANDLE h) throw() : m_h(h)
    {
    }
    ~AutoHandle() throw()
    {
        if (m_h != INVALID_HANDLE_VALUE)
        {
            Close();
        }
    }

    AutoHandle &operator=(AutoHandle &h) throw()
    {
        if (this != &h)
        {
            if (m_h != INVALID_HANDLE_VALUE)
            {
                Close();
            }
            Attach(h.Detach());
        }

        return (*this);
    }

    operator HANDLE() const throw()
    {
        return (m_h);
    }

    // Attach to an existing handle (takes ownership).
    void Attach(HANDLE h) throw()
    {
        m_h = h; // Take ownership
    }

    // Detach the handle from the object (releases ownership).
    HANDLE Detach() throw()
    {
        HANDLE h;

        h = m_h; // Release ownership
        m_h = INVALID_HANDLE_VALUE;

        return (h);
    }

    // Close the handle.
    void Close() throw()
    {
        if (m_h != INVALID_HANDLE_VALUE)
        {
            ::CloseHandle(m_h);
            m_h = INVALID_HANDLE_VALUE;
        }
    }

  protected:
    HANDLE m_h;
};

class AutoEventHandle : public AutoHandle
{
  public:
    AutoEventHandle()
    {
        m_h = CreateEvent(NULL, FALSE, FALSE, NULL);
    }
    ~AutoEventHandle()
    {
        Close();
    }
};
