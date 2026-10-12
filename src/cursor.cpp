#include "pch.h"
#include "cursor.h"
#include "util.h"

//
// It is practically impossible to retrieve the current frame of the current animated cursor, since this
// is opaque data in Windows USER. Even at the extent to which it is possible, i.e. listening for cursor
// changes ourselves and calculating the time difference, I find it difficult to find worthwhile.
//

HRESULT CCursorRenderer::_BlurCursorBitmap(HBITMAP hbm)
{
	// Not yet implemented.
	return S_OK;
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
	
	if (hbmCursorPremult)
	{
		HDC hdcCursor = CreateCompatibleDC(hdcDesktop);
		HGDIOBJ hBmpOld = SelectObject(hdcCursor, hbmCursorPremult);

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
