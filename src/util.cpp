#include "pch.h"
#include "util.h"

//
// Operating system detection
//

static bool IsWine()
{
    HMODULE hmNtdll = GetModuleHandle(TEXT("ntdll.dll"));
    if (hmNtdll && GetProcAddress(hmNtdll, "wine_get_version"))
    {
        return true;
    }
    return false;
}

OSVersion g_osVersion = { 0 };
OSVersion *GetOSVersion()
{
    if (g_osVersion.dwMajorVersion != 0)
        return &g_osVersion;

    g_osVersion.flags |= IsWine()
        ? OSVF_WINE
        : 0;

    typedef struct _RTL_OSVERSIONINFOW {
        ULONG dwOSVersionInfoSize;
        ULONG dwMajorVersion;
        ULONG dwMinorVersion;
        ULONG dwBuildNumber;
        ULONG dwPlatformId;
        WCHAR szCSDVersion[128];
    } RTL_OSVERSIONINFOW;

    // RtlGetVersion is available since 5.0. If it is available, then the OS is guaranteed to be
    // NT.
    typedef LONG (WINAPI *RtlGetVersion_t)(RTL_OSVERSIONINFOW *lpVersionInformation);
    RtlGetVersion_t pfnRtlGetVersion = nullptr;
    HMODULE hmNtdll = GetModuleHandle(TEXT("ntdll.dll"));
    if (hmNtdll && (pfnRtlGetVersion = (RtlGetVersion_t)GetProcAddress(hmNtdll, "RtlGetVersion")))
    {
        RTL_OSVERSIONINFOW ovi = { sizeof(ovi) };
        if (pfnRtlGetVersion(&ovi) == 0)
        {
            g_osVersion.dwMajorVersion = ovi.dwMajorVersion;
            g_osVersion.dwMinorVersion = ovi.dwMinorVersion;
            g_osVersion.dwBuildNumber = ovi.dwBuildNumber;
            g_osVersion.flags |= OSVF_WINNT;
            return &g_osVersion;
        }
    }

#pragma warning(push)
#pragma warning(disable : 4996) // Disable deprecation warning when using Windows 8 or higher SDKs.
    OSVERSIONINFO osvi = { sizeof(osvi) };
    if (GetVersionEx(&osvi))
    {
        g_osVersion.dwMajorVersion = osvi.dwMajorVersion;
        g_osVersion.dwMinorVersion = osvi.dwMinorVersion;
        g_osVersion.flags |= (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)
            ? OSVF_WINNT
            : 0;

        // On NT, the build number is the whole dwBuildNumber property. On DOS-based Windows, it
        // is just the lower word of the build number.
        g_osVersion.dwBuildNumber = (g_osVersion.flags & OSVF_WINNT)
            ? osvi.dwBuildNumber
            : LOWORD(osvi.dwBuildNumber);

        return &g_osVersion;
    }
#pragma warning(pop)

    return nullptr;
}

typedef struct _DLLVERSIONINFO {
    DWORD cbSize;
    DWORD dwMajorVersion;
    DWORD dwMinorVersion;
    DWORD dwBuildNumber;
    DWORD dwPlatformID;
} DLLVERSIONINFO;

typedef HRESULT (CALLBACK *DllGetVersion_t)(DLLVERSIONINFO *pVerInfo);

CommonControlsVersion GetCommonControlsVersion()
{
    static CommonControlsVersion result = { 0 };

    if (result.dwMajorVersion != 0)
        return result;

    HMODULE hComctl32 = GetModuleHandle(TEXT("comctl32.dll"));
    DllGetVersion_t pfnDllGetVersion = nullptr;

    if (hComctl32 && (pfnDllGetVersion = (DllGetVersion_t)GetProcAddress(hComctl32, "DllGetVersion")))
    {
        DLLVERSIONINFO dvi = { sizeof(dvi) };
        HRESULT hr = pfnDllGetVersion(&dvi);
        if (SUCCEEDED(hr))
        {
            result.dwMajorVersion = dvi.dwMajorVersion;
            result.dwMinorVersion = dvi.dwMinorVersion;
            result.dwBuildNumber = dvi.dwBuildNumber;
        }
    }

    return result;
}

//
// String manipulation
//

void _tcstrim(TCHAR *psz)
{
    if (!psz || !*psz)
        return;

    TCHAR *pszStart = psz;
    TCHAR *pszEnd = nullptr;

    while (*pszStart && *pszStart == TEXT(' '))
    {
        pszStart++;
    }

    if (!*pszStart)
    {
        *psz = '\0';
        return;
    }

    pszEnd = pszStart + _tcslen(pszStart) - 1;
    while (pszEnd > pszStart && *pszEnd == TEXT(' '))
    {
        pszEnd--;
    }

    *(pszEnd + 1) = '\0';

    if (pszStart != psz)
    {
        size_t size = _tcslen(pszStart) * sizeof(TCHAR);
        memmove_s(psz, size, pszStart, _tcslen(pszStart) + 1);
    }
}

//
// Path handling
//

const TCHAR *PathFindFileName(const TCHAR *pszPath)
{
    const TCHAR *pc = pszPath;

    for (const TCHAR *c = pszPath; *c; c++)
    {
        if (*c == TEXT('\\'))
        {
            pc = c + 1;
        }
    }

    return pc;
}

const TCHAR *PathFindFileExtension(const TCHAR *pszPath)
{
    const TCHAR *pc = pszPath;

    for (const TCHAR *c = pszPath; *c; c++)
    {
        if (*c == TEXT('.'))
        {
            pc = c;
        }
    }

    return pc;
}

HRESULT PathPopFileName(TCHAR *pszPath)
{
    TCHAR *pszFileName = (TCHAR *)PathFindFileName(pszPath);
    _tcscpy_s(pszFileName, MAX_PATH - (pszFileName - pszPath), TEXT("\0"));
    return S_OK;
}

HRESULT PathAppend(TCHAR *pszPath, const TCHAR *pszPath2)
{
    if (pszPath[_tcslen(pszPath) - 1] != TEXT('\\'))
    {
        _tcscat_s(pszPath, MAX_PATH, TEXT("\\"));
    }

    _tcscat_s(pszPath, MAX_PATH, pszPath2);
    return S_OK;
}

//
// Image manipulation
//

HRESULT CopyBitmap(HBITMAP *phbmDest, HBITMAP hbmSrc, RECT *prcCrop)
{
    if (!phbmDest)
        return E_POINTER;

    HRESULT hr = E_FAIL;
    RECT rcCrop = { 0 };

    if (prcCrop)
    {
        rcCrop = *prcCrop;
    }
    else
    {
        BITMAP bm;
        if (!GetObject(hbmSrc, sizeof(BITMAP), &bm))
        {
            return E_FAIL;
        }
        rcCrop.left = rcCrop.top = 0;
        rcCrop.right = bm.bmWidth;
        rcCrop.bottom = bm.bmHeight;
    }

    HDC hdcDesktop = GetDC(HWND_DESKTOP);
    HBITMAP hbmNew = nullptr;
    if (hdcDesktop)
    {
        HDC hdcSrc = CreateCompatibleDC(hdcDesktop);
        HDC hdcDst = CreateCompatibleDC(hdcDesktop);
        if (hdcSrc && hdcDst)
        {
            hbmNew = CreateCompatibleBitmap(hdcDesktop, RECTWIDTH(rcCrop), RECTHEIGHT(rcCrop));
            if (hbmNew)
            {
                HGDIOBJ hObjOldSrc = SelectObject(hdcSrc, hbmSrc);
                HGDIOBJ hObjOldDst = SelectObject(hdcDst, hbmNew);

                BitBlt(hdcDst, 0, 0, RECTWIDTH(rcCrop), RECTHEIGHT(rcCrop), hdcSrc, rcCrop.left, rcCrop.top, SRCCOPY);

                SelectObject(hdcDst, hObjOldDst);
                SelectObject(hdcSrc, hObjOldSrc);

                hr = S_OK;
            }
        }
        if (hdcSrc)
            DeleteDC(hdcSrc);
        if (hdcDst)
            DeleteDC(hdcDst);
        ReleaseDC(HWND_DESKTOP, hdcDesktop);
    }

    if (hbmNew)
    {
        if (SUCCEEDED(hr))
        {
            *phbmDest = hbmNew;
        }
        else
        {
            DeleteObject(hbmNew);
            hr = E_FAIL;
        }
    }
    else
    {
        hr = E_FAIL;
    }

    return hr;
}

HBITMAP HICONToHBITMAP(HICON hicon)
{
    ICONINFO ii;
    if (GetIconInfo(hicon, &ii))
    {
        DeleteObject(ii.hbmMask);
        return ii.hbmColor;
    }

    return nullptr;
}

HRESULT PremultiplyAlpha(HBITMAP hbmIn, OUT HBITMAP *phbmOut)
{
    BITMAP bm;
    if (!GetObject(hbmIn, sizeof(bm), &bm))
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    bool fAnyAlphaPixel = false;

    HDC hdcDesktop = GetDC(HWND_DESKTOP);
    if (hdcDesktop)
    {
        HDC hdc = CreateCompatibleDC(hdcDesktop);
        if (hdc)
        {
            BITMAPINFO bmi = { 0 };
            bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
            bmi.bmiHeader.biWidth = bm.bmWidth;
            bmi.bmiHeader.biHeight = bm.bmHeight;
            bmi.bmiHeader.biPlanes = 1;
            bmi.bmiHeader.biBitCount = 32;
            bmi.bmiHeader.biCompression = BI_RGB;
            bmi.bmiHeader.biSizeImage = 0;

            void *pvPixels = nullptr;
            *phbmOut = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &pvPixels, nullptr, 0);
            if (*phbmOut)
            {
                HGDIOBJ hObjOld = SelectObject(hdc, *phbmOut);

                HDC hdcOrig = CreateCompatibleDC(hdcDesktop);
                HGDIOBJ hObjOld2 = SelectObject(hdcOrig, hbmIn);

                BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcOrig, 0, 0, SRCCOPY);

                SelectObject(hdcOrig, hObjOld2);
                DeleteDC(hdcOrig);

                int cLength = bm.bmWidth * bm.bmHeight;
                ULONG *pulSrc = (ULONG *)pvPixels;
                BYTE bFirstAlpha = (pulSrc[0] & 0xFF000000) >> 24;
                for (int y = 0; y < bm.bmHeight; y++)
                {
                    for (int x = 0; x < bm.bmWidth; x++)
                    {
                        BYTE b = GetBValue(*pulSrc);
                        BYTE g = GetGValue(*pulSrc);
                        BYTE r = GetRValue(*pulSrc);
                        BYTE a = (*pulSrc & 0xFF000000) >> 24;

                        if (a != bFirstAlpha)
                        {
                            b = (b * a) / 255;
                            g = (g * a) / 255;
                            r = (r * a) / 255;

                            *pulSrc = r | (g << 8) | (b << 16) | (a << 24);

                            fAnyAlphaPixel = true;
                        }

                        pulSrc += 1;
                    }
                }

                SelectObject(hdc, hObjOld);
            }

            DeleteDC(hdc);
        }

        ReleaseDC(HWND_DESKTOP, hdcDesktop);
    }

    if (!fAnyAlphaPixel)
    {
        DeleteObject(*phbmOut);
        *phbmOut = nullptr;
        return S_FALSE;
    }

    return S_OK;
}
