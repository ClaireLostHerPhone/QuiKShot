#pragma once
#include "pch.h"

template <typename T>
class CUniqueGdiObject
{
protected:
    T _h;

public:
    inline CUniqueGdiObject()
        : _h(nullptr)
    {
    }

    inline CUniqueGdiObject(CUniqueGdiObject &rOther)
        : _h(rOther._h)
    {
    }

    inline CUniqueGdiObject(T h)
        : _h(h)
    {
    }

    inline ~CUniqueGdiObject()
    {
        if (_h)
        {
            DeleteObject(_h);
        }
    }

    inline T Get() { return _h; }

    inline void Release()
    {
        _h = nullptr;
    }

    inline T ReleaseAndGet()
    {
        T h = _h;
        Release();
        return h;
    }

    inline T *operator &()
    {
        return &_h;
    }
};

class CUniqueHdc : public CUniqueGdiObject<HDC>
{
public:
    inline CUniqueHdc()
        : CUniqueGdiObject(nullptr)
    {
    }

    inline CUniqueHdc(CUniqueHdc &rOther)
        : CUniqueGdiObject(rOther._h)
    {
    }

    inline CUniqueHdc(HDC h)
        : CUniqueGdiObject(h)
    {
    }

    inline ~CUniqueHdc()
    {
        if (_h)
        {
            DeleteDC(_h);
        }
    }
};

class CUniqueHdcWindow : public CUniqueHdc
{
    HWND _hwnd;

public:
    inline CUniqueHdcWindow(HDC hdc)
        : CUniqueHdc(hdc)
    {
    }

    inline CUniqueHdcWindow(HWND hwnd)
        : _hwnd(hwnd)
    {
        _h = GetDC(hwnd);
    }

    inline ~CUniqueHdcWindow()
    {
        if (_h && _hwnd)
        {
            ReleaseDC(_hwnd, _h);
        }
        else if (_h)
        {
            DeleteDC(_h);
        }
    }
};

typedef CUniqueGdiObject<HBITMAP> CUniqueHBitmap;
typedef CUniqueGdiObject<HBRUSH> CUniqueHBrush;
typedef CUniqueGdiObject<HPEN> CUniqueHPen;
typedef CUniqueGdiObject<HFONT> CUniqueHFont;

class CUniqueHIcon : public CUniqueGdiObject<HICON>
{
public:
    inline CUniqueHIcon()
        : CUniqueGdiObject(nullptr)
    {
    }

    inline CUniqueHIcon(CUniqueHIcon &rOther)
        : CUniqueGdiObject(rOther._h)
    {
    }

    inline CUniqueHIcon(HICON h)
        : CUniqueGdiObject(h)
    {
    }

    inline ~CUniqueHIcon()
    {
        if (_h)
        {
            DestroyIcon(_h);
        }
    }
};

class CSelectObject : public CUniqueGdiObject<HGDIOBJ>
{
    HDC _hdc;

public:
    inline CSelectObject(HDC hdc, HGDIOBJ h)
        : _hdc(hdc)
    {
        _h = SelectObject(hdc, h);
    }

    inline ~CSelectObject()
    {
        if (_h)
        {
            SelectObject(_hdc, _h);
        }
    }
};