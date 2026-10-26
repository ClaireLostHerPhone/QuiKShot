#include "pch.h"
#include "cursor.h"
#include "util.h"

//
// It is practically impossible to retrieve the current frame of the current animated cursor, since this
// is opaque data in Windows USER. Even at the extent to which it is possible, i.e. listening for cursor
// changes ourselves and calculating the time difference, I find it difficult to find worthwhile.
//

bool CCursorRenderer::_IsAnyPixelXor(HBITMAP hbmMask)
{
    // This function doesn't seem to work right now.
    return false;
    BITMAP bm = { 0 };
    if (!GetObject(hbmMask, sizeof(bm), &bm))
    {
        // If this function fails at any point, we'll treat it as though there is
        // an XOR pixel present. This stops the cursor shadow from being rendered.
        return true;
    }

    BITMAPINFO bmi = { 0 };
    bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
    bmi.bmiHeader.biWidth = bm.bmWidth;
    bmi.bmiHeader.biHeight = -bm.bmHeight;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    size_t cbBuffer = bm.bmWidth * bm.bmHeight * 4;
    BYTE *pbPixels = new (std::nothrow) BYTE[cbBuffer];
    if (!pbPixels)
        return true;

    bool fResult = true;

    HDC hdc = CreateCompatibleDC(nullptr);
    if (hdc)
    {
        fResult = false;

        GetDIBits(
            hdc,
            hbmMask,
            0,
            bm.bmHeight,
            pbPixels,
            &bmi,
            DIB_RGB_COLORS
        );

        pbPixels += (bm.bmHeight / 2) * 4;

        for (int i = 0, len = bm.bmHeight / 2; i < len; i++)
        {
            DWORD dw = *(((DWORD *)pbPixels) + i) & 0x00FFFFFF;
            if (dw != RGB(255, 255, 255))
            {
                fResult = true;
                break;
            }
        }

        DeleteDC(hdc);
    }

    delete[] pbPixels;
    return fResult;
}

bool CCursorRenderer::_IsCursorShadowEnabled()
{
    BOOL fEnabled = FALSE;
    SystemParametersInfo(SPI_GETCURSORSHADOW, 0, &fEnabled, FALSE);
    return fEnabled;
}

HRESULT CCursorRenderer::_DrawCursorShadow(HDC hdc, BITMAP *pbm)
{
    if (PortableIsAlphaBlendAvailable() && pbm->bmBitsPixel == 32 && !_IsAnyPixelXor(_hbmCursorMask))
    {
        // Convert the mask to 32bpp:
        HDC hdcDesktop = GetDC(HWND_DESKTOP);
        HDC hdcShadow = CreateCompatibleDC(hdcDesktop);
        HDC hdcMask = CreateCompatibleDC(hdcDesktop);

        BITMAPINFO bmi = { 0 };
        bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
        bmi.bmiHeader.biWidth = pbm->bmWidth;
        bmi.bmiHeader.biHeight = -pbm->bmHeight;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;

        void *pvPixels;
        HBITMAP hbmMask32bpp = CreateDIBSection(hdcShadow, &bmi, DIB_RGB_COLORS, &pvPixels, nullptr, 0);
        HGDIOBJ hbmOld = SelectObject(hdcShadow, hbmMask32bpp);
        HGDIOBJ hbmOldMask = SelectObject(hdcMask, _hbmCursorMask);

        BitBlt(hdcShadow, 0, 0, pbm->bmWidth, pbm->bmHeight, hdcMask, 0, 0, SRCCOPY);

        // Apply the constant alpha:
        ULONG *pulSrc = (ULONG *)pvPixels;
        for (int y = 0; y < pbm->bmHeight; y++)
        {
            for (int x = 0; x < pbm->bmWidth; x++)
            {
                static constexpr int c_iAlpha = 0x40;

                int r = GetRValue(*pulSrc);
                int g = GetGValue(*pulSrc);
                int b = GetBValue(*pulSrc);

                if (RGB(r, g, b) == RGB(255, 255, 255))
                {
                    // If the pixel is white, skip.
                    pulSrc += 1;
                    continue;
                }
                else
                {
                    *pulSrc = 0x40000000;
                }

                pulSrc += 1;
            }
        }

        // Apply the blur. Windows applies the box blur twice.
        _BlurCursorShadowBitmap(hbmMask32bpp);
        _BlurCursorShadowBitmap(hbmMask32bpp);

#if 0 // Not good yet.
        // Remove the cursor mask from the buffer:
        void *pvPixels2 = new (std::nothrow) BYTE[pbm->bmWidth * pbm->bmHeight * 4];
        GetDIBits(
            hdcMask,
            _hbmCursorMask,
            0,
            pbm->bmHeight,
            pvPixels2,
            &bmi,
            DIB_RGB_COLORS
        );

        pulSrc = (ULONG *)pvPixels;
        ULONG *pulSrc2 = (ULONG *)pvPixels2;
        for (int y = 0; y < pbm->bmHeight; y++)
        {
            for (int x = 0; x < pbm->bmWidth; x++)
            {
                int r = GetRValue(*pulSrc2);
                int g = GetGValue(*pulSrc2);
                int b = GetBValue(*pulSrc2);

                if (RGB(r, g, b) == RGB(0, 0, 0))
                {
                    *pulSrc = 0x00000000;
                }

                pulSrc2 += 1;
                pulSrc += 1;
            }
        }

        delete[] pvPixels2;
#endif

        // Draw the shadow into the framebuffer:
        BLENDFUNCTION bf;
        bf.AlphaFormat = AC_SRC_ALPHA;
        bf.SourceConstantAlpha = 255;
        bf.BlendFlags = 0;
        bf.BlendOp = AC_SRC_OVER;

        PortableAlphaBlend(
            hdc,
            3, 1,
            pbm->bmWidth, pbm->bmHeight,
            hdcShadow,
            0, 0,
            pbm->bmWidth, pbm->bmHeight,
            bf
        );

        SelectObject(hdcMask, hbmOldMask);
        DeleteDC(hdcMask);

        SelectObject(hdcShadow, hbmOld);
        DeleteDC(hdcShadow);
        ReleaseDC(HWND_DESKTOP, hdcDesktop);
    }

    return S_OK;
}

HRESULT CCursorRenderer::_BlurCursorShadowBitmap(HBITMAP hbm)
{
    USHORT us0[64];
    USHORT us1[64];
    USHORT us2[64];

    USHORT *pus[3] = { 0 };

    BITMAP bm = { 0 };
    if (!GetObject(hbm, sizeof(bm), &bm))
    {
        return E_FAIL;
    }

    if (bm.bmWidth < 3 || bm.bmHeight < 3)
    {
        return E_FAIL;
    }

    if (bm.bmWidth > 64)
    {
        pus[0] = new USHORT[bm.bmWidth * 3];
        pus[1] = pus[0] + bm.bmWidth;
        pus[2] = pus[1] + bm.bmWidth;
    }
    else
    {
        pus[0] = us0;
        pus[1] = us1;
        pus[2] = us2;
    }

    BITMAPINFO bmi = { 0 };
    bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
    bmi.bmiHeader.biWidth = bm.bmWidth;
    bmi.bmiHeader.biHeight = -bm.bmHeight;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    HRESULT hr = E_FAIL;

    size_t cbBuffer = bm.bmWidth * bm.bmHeight * 4;
    BYTE *pbPixels = new (std::nothrow) BYTE[cbBuffer];
    if (pbPixels)
    {
        HDC hdc = CreateCompatibleDC(nullptr);
        if (hdc)
        {
            GetDIBits(
                hdc,
                hbm,
                0,
                bm.bmHeight,
                pbPixels,
                &bmi,
                DIB_RGB_COLORS
            );

            ULONG *pulIn = (ULONG *)pbPixels;

            // The first 3 scanlines have a 3x1 box blur calculated for them as
            // input for the following blur.

            USHORT *pus2 = nullptr;
            USHORT *pusEnd = nullptr;
            ULONG *pulTmp = nullptr;
            for (int i = 0; i < 3; i++)
            {
                pus2 = pus[i];
                pusEnd = pus2 + (bm.bmWidth - 2);
                pulTmp = pulIn;

                while (pus2 < pusEnd)
                {
                    *pus2 = (USHORT)((pulTmp[0] >> 24) + (pulTmp[1] >> 24) + (pulTmp[2] >> 24));
                    pus2++;
                    pulTmp++;
                }

                // Increment the input scanline:
                pulIn += bm.bmWidth;
            }

            // Next, a 3x3 box blur is applied to each output scanline.

            ULONG *pulOut = (ULONG *)pbPixels + bm.bmWidth + 1;
            ULONG ulNumScans = bm.bmHeight - 2;
            ULONG ulNext = 0;

            while (ulNumScans--)
            {
                ULONG *pulAvg = pulOut;
                ULONG *pulAvgEnd = pulAvg + (bm.bmWidth - 2);

                USHORT *pusTmp[3];
                pusTmp[0] = pus[0];
                pusTmp[1] = pus[1];
                pusTmp[2] = pus[2];

                while (pulAvg < pulAvgEnd)
                {
                    USHORT usSum = *pusTmp[0] + *pusTmp[1] + *pusTmp[2];
                    *pulAvg = (usSum >> 3) << 24;

                    pulAvg++;
                    pusTmp[0]++;
                    pusTmp[1]++;
                    pusTmp[2]++;
                }

                // Increment the output scanline:
                pulOut += bm.bmWidth;

                if (ulNumScans)
                {
                    pus2 = pus[ulNext];
                    pusEnd = pus2 + (bm.bmWidth - 2);
                    pulTmp = pulIn;

                    while (pus2 < pusEnd)
                    {
                        *pus2 = (USHORT)((pulTmp[0] >> 24) + (pulTmp[1] >> 24) + (pulTmp[2] >> 24));
                        pus2++;
                        pulTmp++;
                    }

                    // Increment the input scanline:
                    pulIn += bm.bmWidth;

                    ulNext++;
                    if (ulNext >= 3)
                        ulNext = 0;
                }
            }

            SetDIBits(hdc, hbm, 0, bm.bmHeight, pbPixels, &bmi, DIB_RGB_COLORS);

            DeleteDC(hdc);
        }

        delete[] pbPixels;
    }

    if (pus[0] != us0)
    {
        delete[] pus[0];
    }

    return hr;
}

HRESULT CCursorRenderer::RenderCursor(HBITMAP *phbmCursor, RECT *prcCursor)
{
    BITMAP bm;
    if (!GetObject(_hbmCursorColor, sizeof(bm), &bm) &&
        !GetObject(_hbmCursorMask, sizeof(bm), &bm))
    {
        return E_FAIL;
    }

    HDC hdcDesktop = GetDC(HWND_DESKTOP);
    HDC hdc = CreateCompatibleDC(hdcDesktop);
    HBITMAP hbm = CreateCompatibleBitmap(hdcDesktop, bm.bmWidth, bm.bmHeight);
    HGDIOBJ hBmpOld = SelectObject(hdc, hbm);

    // Paint the background of the cursor:
    HDC hdcScreenshot = CreateCompatibleDC(hdcDesktop);
    HGDIOBJ hScreenshotOld = SelectObject(hdcScreenshot, _hbmScreenshot);
    BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcScreenshot, _ptCursor.x, _ptCursor.y, SRCCOPY);
    SelectObject(hdcScreenshot, hScreenshotOld);
    DeleteDC(hdcScreenshot);

    // We just use this procedure to check if the cursor has premultiplied alpha. We don't actually
    // use the result.
    HBITMAP hbmCursorPremult = nullptr;
    PremultiplyAlpha(_hbmCursorColor, &hbmCursorPremult);
    
    if (PortableIsAlphaBlendAvailable() && hbmCursorPremult)
    {
        HDC hdcCursor = CreateCompatibleDC(hdcDesktop);
        HGDIOBJ hBmpOld = SelectObject(hdcCursor, hbmCursorPremult);

        if (_IsCursorShadowEnabled())
        {
            _DrawCursorShadow(hdc, &bm);
        }

        // If we have an alpha cursor, then we'll just draw it with DrawIcon. This avoids alpha issues
        // with animated cursors that I couldn't figure out how to avoid with AlphaBlending the bitmap
        // from GetIconInfo.
        DrawIcon(hdc, 0, 0, _hcursor);

        DeleteDC(hdcCursor);
    }
    else if (_hbmCursorColor) // Premultiplied cursor is not available.
    {
        HDC hdcCursor = CreateCompatibleDC(hdcDesktop);
        HGDIOBJ hBmpOld = SelectObject(hdcCursor, _hbmCursorMask);

        BitBlt(
            hdc,
            0, 0,
            bm.bmWidth, bm.bmHeight,
            hdcCursor,
            0, 0,
            SRCAND
        );

        SelectObject(hdcCursor, hBmpOld);
        hBmpOld = SelectObject(hdcCursor, _hbmCursorColor);

        BitBlt(
            hdc,
            0, 0,
            bm.bmWidth, bm.bmHeight,
            hdcCursor,
            0, 0,
            SRCINVERT
        );
        SelectObject(hdcCursor, hBmpOld);
        DeleteDC(hdcCursor);
    }
    else // No color cursor is available, so the mask cursor will be drawn instead.
    {
        HDC hdcCursor = CreateCompatibleDC(hdcDesktop);
        HGDIOBJ hBmpOld = SelectObject(hdcCursor, _hbmCursorMask);

        // The upper half of the mask is the AND channel, and the lower half is the XOR channel.
        BitBlt(
            hdc,
            0, 0,
            bm.bmWidth, bm.bmHeight / 2,
            hdcCursor,
            0, 0,
            SRCAND
        );
        BitBlt(
            hdc,
            0, 0,
            bm.bmWidth, bm.bmHeight / 2,
            hdcCursor,
            0, bm.bmHeight / 2,
            SRCINVERT
        );
        SelectObject(hdcCursor, hBmpOld);
        DeleteDC(hdcCursor);

        bm.bmHeight /= 2;
    }

    if (hbmCursorPremult)
        DeleteObject(hbmCursorPremult);

    SelectObject(hdc, hBmpOld);
    DeleteDC(hdc);
    ReleaseDC(HWND_DESKTOP, hdcDesktop);

    RECT rcCursor;
    rcCursor.left = _ptCursor.x;
    rcCursor.top = _ptCursor.y;
    rcCursor.right = rcCursor.left + bm.bmWidth;
    rcCursor.bottom = rcCursor.top + bm.bmHeight;
    *phbmCursor = hbm;
    *prcCursor = rcCursor;
    return S_OK;
}
