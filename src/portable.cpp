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

HRESULT PortableInitialize()
{
    HMODULE hmUser32 = LoadLibrary(TEXT("user32.dll"));

    HMODULE hmMsimg32 = LoadLibrary(TEXT("msimg32.dll"));
    if (hmMsimg32)
    {
        g_pfnAlphaBlend = (AlphaBlend_t)GetProcAddress(hmMsimg32, "AlphaBlend");
    }

    return S_OK;
}