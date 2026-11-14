#include "pch.h"
#include "portablep.h"

typedef int (WINAPI *GetSystemMetricsForDpi_t)(int nIndex, UINT dpi);
GetSystemMetricsForDpi_t g_pfnGetSystemMetricsForDpi = nullptr;

int PortableGetSystemMetricsForDpi(int nIndex, UINT dpi)
{
    if (g_pfnGetSystemMetricsForDpi)
    {
        return g_pfnGetSystemMetricsForDpi(nIndex, dpi);
    }
    else
    {
        return GetSystemMetrics(nIndex);
    }
}

typedef BOOL (WINAPI *AlphaBlend_t)(
    HDC hdcDest, int xoriginDest, int yoriginDest, int wDest, int hDest,
    HDC hdcSrc, int xoriginSrc, int yoriginSrc, int wSrc, int hSrc, BLENDFUNCTION ftn
);
AlphaBlend_t g_pfnAlphaBlend = nullptr;

HRESULT PortableAlphaBlend(
    HDC hdcDest, int xoriginDest, int yoriginDest, int wDest, int hDest,
    HDC hdcSrc, int xoriginSrc, int yoriginSrc, int wSrc, int hSrc, BLENDFUNCTION ftn
)
{
    if (!g_pfnAlphaBlend)
    {
        return E_NOTIMPL;
    }

    return g_pfnAlphaBlend(hdcDest, xoriginDest, yoriginDest, wDest, hDest, hdcSrc, xoriginSrc, yoriginSrc, wSrc, hSrc, ftn)
        ? S_OK
        : S_FALSE;
}

bool PortableIsAlphaBlendAvailable()
{
    return g_pfnAlphaBlend ? true : false;
}

typedef HRESULT (WINAPI *AssocCreate_t)(CLSID clsid, REFIID riid, void **ppv);
AssocCreate_t g_pfnAssocCreate = nullptr;

HRESULT PortableAssocCreate(CLSID clsid, REFIID riid, void **ppv)
{
    if (!g_pfnAssocCreate)
        return E_NOTIMPL;

    return g_pfnAssocCreate(clsid, riid, ppv);
}

bool PortableIsAssocCreateAvailable()
{
    return g_pfnAssocCreate ? true : false;
}

typedef BOOL (WINAPI *EnumDisplayMonitors_t)(HDC hdc, LPCRECT lprcClip, MONITORENUMPROC lpfnEnum, LPARAM dwData);
EnumDisplayMonitors_t g_pfnEnumDisplayMonitors;

typedef HMONITOR (WINAPI *MonitorFromPoint_t)(POINT pt, DWORD dwFlags);
MonitorFromPoint_t g_pfnMonitorFromPoint;

typedef BOOL (WINAPI *GetMonitorInfo_t)(HMONITOR hMonitor, LPMONITORINFO lpmi);
GetMonitorInfo_t g_pfnGetMonitorInfo;

bool PortableIsMultiMonitorSupportAvailable()
{
    return g_pfnEnumDisplayMonitors ? true : false;
}

HRESULT PortableEnumDisplayMonitors(HDC hdc, const RECT *prcClip, MONITORENUMPROC pfnEnum, LPARAM dwData)
{
    if (!g_pfnEnumDisplayMonitors)
        return E_NOTIMPL;
    BOOL fRes = g_pfnEnumDisplayMonitors(hdc, prcClip, pfnEnum, dwData);
    return fRes ? S_OK : S_FALSE;
}

HMONITOR PortableMonitorFromPoint(POINT pt, DWORD dwFlags)
{
    if (!g_pfnMonitorFromPoint)
        return nullptr;
    return g_pfnMonitorFromPoint(pt, dwFlags);
}

HRESULT PortableGetMonitorInfo(HMONITOR hMonitor, MONITORINFO *pmi)
{
    if (!g_pfnGetMonitorInfo)
        return E_NOTIMPL;
    BOOL fResult = g_pfnGetMonitorInfo(hMonitor, pmi);
    return fResult ? S_OK : S_FALSE;
}

typedef BOOL (WINAPI *GetCursorInfo_t)(CURSORINFO *pci);
GetCursorInfo_t g_pfnGetCursorInfo = nullptr;
HRESULT PortableGetCursorInfo(CURSORINFO *pci)
{
    // Polyfill for Windows 95:
    if (!g_pfnGetCursorInfo)
    {
        if (!pci->cbSize != sizeof(CURSORINFO))
            return E_INVALIDARG;

        GetCursorPos(&pci->ptScreenPos);
        
        HCURSOR hCursor = nullptr;
        HWND hwndTarget = WindowFromPoint(pci->ptScreenPos);
        if (!hwndTarget)
        {
            hCursor = GetCursor();
        }

        DWORD dwCurrentThread = GetCurrentThreadId();
        DWORD dwTargetThread = GetWindowThreadProcessId(hwndTarget, nullptr);

        if (dwTargetThread == 0 || dwTargetThread == dwCurrentThread)
        {
            hCursor = GetCursor();
        }

        if (AttachThreadInput(dwCurrentThread, dwTargetThread, TRUE))
        {
            hCursor = GetCursor();
            AttachThreadInput(dwCurrentThread, dwTargetThread, FALSE);
        }
        else
        {
            hCursor = GetCursor();
        }

        if (hCursor)
        {
            pci->hCursor = hCursor;
            pci->flags = CURSOR_SHOWING;
        }
        else
        {
            pci->hCursor = nullptr;
            pci->flags = 0;
        }
    }

    return g_pfnGetCursorInfo(pci);
}

HRESULT PortableInitialize()
{
    HMODULE hmUser32 = LoadLibrary(TEXT("user32.dll"));
    if (hmUser32)
    {
        g_pfnEnumDisplayMonitors = (EnumDisplayMonitors_t)GetProcAddress(hmUser32, "EnumDisplayMonitors");
        g_pfnMonitorFromPoint = (MonitorFromPoint_t)GetProcAddress(hmUser32, "MonitorFromPoint");
        g_pfnGetMonitorInfo = (GetMonitorInfo_t)GetProcAddress(hmUser32, "GetMonitorInfo" _AWSTR);
        g_pfnGetSystemMetricsForDpi = (GetSystemMetricsForDpi_t)GetProcAddress(hmUser32, "GetSystemMetricsForDpi");
        g_pfnGetCursorInfo = (GetCursorInfo_t)GetProcAddress(hmUser32, "GetCursorInfo");
    }

    HMODULE hmMsimg32 = LoadLibrary(TEXT("msimg32.dll"));
    if (hmMsimg32)
    {
        g_pfnAlphaBlend = (AlphaBlend_t)GetProcAddress(hmMsimg32, "AlphaBlend");
    }

    HMODULE hmShlwapi = LoadLibrary(TEXT("shlwapi.dll"));
    if (hmShlwapi)
    {
        g_pfnAssocCreate = (AssocCreate_t)GetProcAddress(hmShlwapi, "AssocCreate");
    }

    return S_OK;
}