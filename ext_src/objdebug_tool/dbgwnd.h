#pragma once
#include "pch.h"
#include "window.h"

#define DEFINE_WINDOW_CLASS(cls) public: static const TCHAR *GetWindowClass() { return TEXT(cls); }

class CObjectDebugWindow : public CWindow<CObjectDebugWindow>
{
    DEFINE_WINDOW_CLASS("quikshot.objdebug.ObjectDebugWindow");

private:
    IScreenshotEditor *_pEditor;
    HWND _hwndToolbar;
    HWND _hwndListView;
    bool _fIsSticky;

protected:
    LRESULT v_WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
    LRESULT _OnCreate(CREATESTRUCT *pcs);
    LRESULT _OnDestroy();
    LRESULT _OnCommand(WPARAM wParam, LPARAM lParam);
    HRESULT _UpdateLayout();
    HRESULT _EnumItems();
    HRESULT _SetAutoRefreshSubclass();
    HRESULT _RemoveAutoRefreshSubclass();
    static LRESULT CALLBACK _AutoRefreshSubclassProc(
        HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

public:
    enum WM
    {
        WM_OBJDBGWND_TOOLCHANGED = WM_APP + 1,
    };

    enum Command
    {
        IDC_LISTVIEW = 100,

        IDM_REFRESH = 200,
        IDM_STICKY,
        IDM_AUTOREFRESH,
        IDM_DELETE,
    };

    static HRESULT RegisterWindowClass();
    static CObjectDebugWindow *Create(IScreenshotEditor *pse, DWORD dwExStyle, DWORD dwStyle, int x, int y, int cx, int cy, HWND hwndParent);

    CObjectDebugWindow()
        : _pEditor(nullptr)
        , _hwndToolbar(nullptr)
        , _hwndListView(nullptr)
        , _fIsSticky(false)
    {
    }
};