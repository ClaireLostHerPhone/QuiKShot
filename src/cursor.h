#pragma once
#include "pch.h"

class CCursorRenderer
{
    HCURSOR _hcursor;
    HBITMAP _hbmScreenshot;
    HBITMAP _hbmCursorColor;
    HBITMAP _hbmCursorMask;
    POINT _ptCursor;

    bool _IsAnyPixelXor(HBITMAP hbmMask);
    bool _IsCursorShadowEnabled();
    HRESULT _DrawCursorShadow(HDC hdc, BITMAP *pbm);
    HRESULT _BlurCursorShadowBitmap(HBITMAP hbm);

public:
    CCursorRenderer(HBITMAP hbmScreenshot, HCURSOR hcursor, HBITMAP hbmCursorColor, HBITMAP hbmCursorMask, POINT ptCursor)
        : _hbmScreenshot(hbmScreenshot)
        , _hcursor(hcursor)
        , _hbmCursorColor(hbmCursorColor)
        , _hbmCursorMask(hbmCursorMask)
    {
        _ptCursor.x = ptCursor.x;
        _ptCursor.y = ptCursor.y;
    }

    HRESULT RenderCursor(HBITMAP *phbmCursor, RECT *prcCursor);
};