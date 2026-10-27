#include "pch.h"
#include "cfgmgr.h"

#define REGSTR_QUIKSHOT TEXT("SOFTWARE\\ClaireLostHerPhone\\QuiKShot")

CConfigManager *g_pCfgMgrInst = nullptr;

HRESULT CConfigManager::Initialize()
{
    HKEY hkey = nullptr;
    LONG ls = RegOpenKeyEx(HKEY_CURRENT_USER, REGSTR_QUIKSHOT, 0, KEY_READ, &hkey);
    if (ls != ERROR_SUCCESS)
    {
        ls = RegCreateKey(HKEY_CURRENT_USER, REGSTR_QUIKSHOT, &hkey);

        if (ls != ERROR_SUCCESS)
        {
            return HRESULT_FROM_WIN32(GetLastError());
        }
    }

    _hkey = hkey;
    return S_OK;
}

HRESULT CConfigManager::GetBool(const TCHAR *pszName, OUT bool *pfOut)
{
    if (!pszName)
        return E_POINTER;

    DWORD dwValue = 0;
    DWORD dwcch = sizeof(DWORD);
    DWORD dwType = 0;
    LONG ls = RegQueryValueEx(_hkey, pszName, 0, &dwType, (BYTE *)&dwValue, &dwcch);

    if (ls == ERROR_SUCCESS && dwType == REG_DWORD)
    {
        *pfOut = dwValue != 0;
        return S_OK;
    }

    return E_NOT_SET;
}

HRESULT CConfigManager::GetString(const TCHAR *pszName, OUT TCHAR **ppszOut)
{
    if (!pszName || !ppszOut)
        return E_POINTER;

    DWORD dwcch;
    DWORD dwType = 0;
    LONG ls = RegQueryValueEx(_hkey, pszName, 0, &dwType, nullptr, &dwcch);

    if (ls == ERROR_SUCCESS && dwType == REG_SZ)
    {
        size_t cb = (dwcch + 2) * sizeof(TCHAR);
        *ppszOut = (TCHAR *)CoTaskMemAlloc(cb);
        ZeroMemory(*ppszOut, cb);

        ls = RegQueryValueEx(_hkey, pszName, 0, nullptr, (BYTE *)*ppszOut, &dwcch);
        if (ls == ERROR_SUCCESS)
        {
            return S_OK;
        }

        CoTaskMemFree(ppszOut);
        return E_FAIL;
    }

    return E_NOT_SET;
}

// static
HRESULT CConfigManager::CreateInstance()
{
    g_pCfgMgrInst = new (std::nothrow) CConfigManager();
    if (g_pCfgMgrInst)
    {
        HRESULT hr = g_pCfgMgrInst->Initialize();
        if (SUCCEEDED(hr))
        {
            DBGPRINT(TEXT("Initialized config manager."));
            return S_OK;
        }

        return hr;
    }
    else
    {
        return E_OUTOFMEMORY;
    }
}

// static
CConfigManager *CConfigManager::GetInstance()
{
    return g_pCfgMgrInst;
}
