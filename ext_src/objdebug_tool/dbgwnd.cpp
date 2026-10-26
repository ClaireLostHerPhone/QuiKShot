#include "pch.h"
#include "dbgwnd.h"
#include <commctrl.h>

LRESULT CObjectDebugWindow::v_WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CREATE:
        {
            return _OnCreate((CREATESTRUCT *)lParam);
        }

        case WM_DESTROY:
        {
            return _OnDestroy();
        }

        case WM_CLOSE:
        {
            ShowWindow(_hwnd, SW_HIDE);
            return 0;
        }

        case WM_SIZE:
        {
            _UpdateLayout();
            break;
        }

        case WM_SETCURSOR:
        {
            SetCursor(LoadCursor(nullptr, IDC_ARROW));
            return 0;
        }

        case WM_COMMAND:
        {
            return _OnCommand(wParam, lParam);
        }

        case WM_NOTIFY:
        {
            NMHDR *pnmh = (NMHDR *)lParam;

            if (pnmh->idFrom == IDC_LISTVIEW && pnmh->code == NM_CUSTOMDRAW)
            {
                NMLVCUSTOMDRAW *plvcd = (NMLVCUSTOMDRAW *)lParam;

                switch (plvcd->nmcd.dwDrawStage)
                {
                    case CDDS_PREPAINT:
                    {
                        return CDRF_NOTIFYITEMDRAW;
                    }
                }
            }

            break;
        }

        case WM_OBJDBGWND_TOOLCHANGED:
        {
            if (!_fIsSticky && wParam == 0)
            {
                ShowWindow(_hwnd, SW_HIDE);
            }
            else if (wParam == 1)
            {
                ShowWindow(_hwnd, SW_SHOW);
            }
            return 0;
        }
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CObjectDebugWindow::_OnCreate(CREATESTRUCT *pcs)
{
    _pEditor = (IScreenshotEditor *)pcs->lpCreateParams;

    _hwndToolbar = CreateWindowEx(
        0,
        TOOLBARCLASSNAME,
        nullptr,
        WS_CHILD | TBSTYLE_LIST | TBSTYLE_FLAT | CCS_TOP,
        0, 0,
        0, 0,
        _hwnd,
        (HMENU)IDC_LISTVIEW,
        g_hinst,
        nullptr
    );

    SendMessage(_hwndToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);

    TBADDBITMAP ab;
    ab.hInst = HINST_COMMCTRL;
    ab.nID = IDB_STD_SMALL_COLOR;
    SendMessage(_hwndToolbar, TB_ADDBITMAP, 15, (LPARAM)&ab);

    TBBUTTON rgtbButton[4] = { 0 };

    int iButton = 0;
    rgtbButton[iButton].idCommand = IDM_REFRESH;
    rgtbButton[iButton].iString = (INT_PTR)TEXT("&Refresh");
    rgtbButton[iButton].fsState = TBSTATE_ENABLED;
    rgtbButton[iButton].fsStyle = BTNS_BUTTON | BTNS_AUTOSIZE;
    rgtbButton[iButton].iBitmap = STD_REPLACE; // Closest icon to what I want.

    iButton++;
    rgtbButton[iButton].idCommand = IDM_AUTOREFRESH;
    rgtbButton[iButton].iString = (INT_PTR)TEXT("&Auto-refresh");
    rgtbButton[iButton].fsState = TBSTATE_ENABLED;
    rgtbButton[iButton].fsStyle = BTNS_BUTTON | BTNS_AUTOSIZE | BTNS_CHECK;
    rgtbButton[iButton].iBitmap = STD_REPLACE; // Closest icon to what I want.

    iButton++;
    rgtbButton[iButton].idCommand = IDM_STICKY;
    rgtbButton[iButton].iString = (INT_PTR)TEXT("Keep window &visible");
    rgtbButton[iButton].fsState = TBSTATE_ENABLED;
    rgtbButton[iButton].fsStyle = BTNS_BUTTON | BTNS_AUTOSIZE | BTNS_CHECK;
    rgtbButton[iButton].iBitmap = STD_PROPERTIES; // Closest icon to what I want.

    iButton++;
    rgtbButton[iButton].idCommand = IDM_DELETE;
    rgtbButton[iButton].iString = (INT_PTR)TEXT("&Delete");
    rgtbButton[iButton].fsState = TBSTATE_ENABLED;
    rgtbButton[iButton].fsStyle = BTNS_BUTTON | BTNS_AUTOSIZE;
    rgtbButton[iButton].iBitmap = STD_DELETE;

    SendMessage(_hwndToolbar, TB_ADDBUTTONS, ARRAYSIZE(rgtbButton), (LPARAM)&rgtbButton);
    SendMessage(_hwndToolbar, TB_AUTOSIZE, 0, 0);

    RECT rcToolbar;
    GetClientRect(_hwndToolbar, &rcToolbar);

    _hwndListView = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        WC_LISTVIEW,
        nullptr,
        WS_CHILD | WS_BORDER | LVS_LIST,
        0, 0,
        0, 0,
        _hwnd,
        nullptr,
        g_hinst,
        nullptr
    );

    RECT rcAdjusted;
    GetWindowRect(_hwnd, &rcAdjusted);
    rcAdjusted.right = rcAdjusted.left + RECTWIDTH(rcToolbar);
    DWORD dwStyle = GetWindowLong(_hwnd, GWL_STYLE);
    DWORD dwExStyle = GetWindowLong(_hwnd, GWL_EXSTYLE);
    AdjustWindowRectEx(&rcAdjusted, dwStyle, FALSE, dwExStyle);

    SetWindowPos(_hwnd, nullptr, 0, 0, RECTWIDTH(rcAdjusted), RECTHEIGHT(rcAdjusted), SWP_NOMOVE);

    ShowWindow(_hwndToolbar, SW_SHOW);
    ShowWindow(_hwndListView, SW_SHOW);
    return DefWindowProc(_hwnd, WM_CREATE, 0, (LPARAM)pcs);
}

LRESULT CObjectDebugWindow::_OnDestroy()
{
    _RemoveAutoRefreshSubclass();
    return DefWindowProc(_hwnd, WM_DESTROY, 0, 0);
}

LRESULT CObjectDebugWindow::_OnCommand(WPARAM wParam, LPARAM lParam)
{
    switch (LOWORD(wParam))
    {
        case IDM_STICKY:
        {
            _fIsSticky = SendMessage(_hwndToolbar, TB_ISBUTTONCHECKED, IDM_STICKY, 0);
            return 0;
        }

        case IDM_REFRESH:
        {
            _EnumItems();
            return 0;
        }

        case IDM_AUTOREFRESH:
        {
            bool fChecked = SendMessage(_hwndToolbar, TB_ISBUTTONCHECKED, LOWORD(wParam), 0);

            if (fChecked)
            {
                _SetAutoRefreshSubclass();
            }
            else
            {
                _RemoveAutoRefreshSubclass();
            }

            return 0;
        }

        case IDM_DELETE:
        {
            MessageBox(_hwnd, TEXT("This feature has yet to be implemented."), TEXT("Not implemented!"), MB_OK | MB_ICONERROR);
            return 0;
        }
    }

    return DefWindowProc(_hwnd, WM_COMMAND, wParam, lParam);
}

HRESULT CObjectDebugWindow::_UpdateLayout()
{
    RECT rcClient;
    GetClientRect(_hwnd, &rcClient);

    HDWP hdwp = BeginDeferWindowPos(2);

    RECT rcToolbarCur;
    GetClientRect(_hwndToolbar, &rcToolbarCur);
    hdwp = DeferWindowPos(hdwp, _hwndToolbar, nullptr, 0, 0, RECTWIDTH(rcClient), RECTHEIGHT(rcToolbarCur), SWP_NOMOVE);

    RECT rcListView = rcClient;
    rcListView.top += RECTHEIGHT(rcToolbarCur);
    InflateRect(&rcListView, -6, -6);

    hdwp = DeferWindowPos(hdwp, _hwndListView, nullptr, rcListView.left, rcListView.top, RECTWIDTH(rcListView), RECTHEIGHT(rcListView), 0);

    EndDeferWindowPos(hdwp);

    return S_OK;
}

HRESULT CObjectDebugWindow::_EnumItems()
{
    IEnumUnknown *pEnum = nullptr;
    HRESULT hr = _pEditor->EnumObjects(&pEnum);

    SendMessage(_hwndListView, WM_SETREDRAW, 0, 0);

    if (SUCCEEDED(hr))
    {
        IUnknown *pUnk = nullptr;
        IScreenshotEditorObject *pObj = nullptr;

        SendMessage(_hwndListView, LVM_DELETEALLITEMS, 0, 0);

        int i = 0;
        while (S_OK == pEnum->Next(1, &pUnk, nullptr))
        {
            hr = pUnk->QueryInterface(IID_PPV_ARGS(&pObj));
            if (SUCCEEDED(hr))
            {
                LVITEM lvi = { 0 };
                lvi.mask = LVIF_TEXT;
                lvi.iItem = i++;
                lvi.pszText = (LPTSTR)pObj->GetClassName();
                lvi.cchTextMax = MAX_PATH;

                SendMessage(_hwndListView, LVM_INSERTITEM, 0, (LPARAM)&lvi);

                pObj->Release();
            }

            pUnk->Release();
        }
    }

    SendMessage(_hwndListView, WM_SETREDRAW, 1, 0);
    RedrawWindow(_hwndListView, nullptr, nullptr, RDW_ERASE | RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN);

    return hr;
}

HRESULT CObjectDebugWindow::_SetAutoRefreshSubclass()
{
    return SetWindowSubclass(_pEditor->GetEditorHWND(), _AutoRefreshSubclassProc, 0, (DWORD_PTR)this)
        ? S_OK
        : E_FAIL;
}

HRESULT CObjectDebugWindow::_RemoveAutoRefreshSubclass()
{
    return RemoveWindowSubclass(_pEditor->GetEditorHWND(), _AutoRefreshSubclassProc, 0)
        ? S_OK
        : E_FAIL;
}

// static
LRESULT CObjectDebugWindow::_AutoRefreshSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    CObjectDebugWindow *pThis = (CObjectDebugWindow *)dwRefData;

    switch (uMsg)
    {
        case WM_PAINT:
        {
            // We refresh all items on paint. The editor doesn't expose an event that we can listen
            // for when items change, so this is the best way to do it. Basically any object change
            // results in a repaint of the screenshot editor.
            pThis->_EnumItems();

            // [[fallthrough]]
        }
    }

    return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}

// static
HRESULT CObjectDebugWindow::RegisterWindowClass()
{
    WNDCLASS cls = { 0 };
    cls.hInstance = g_hinst;
    cls.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    cls.hCursor = nullptr;

    // Load IDI_APP from the main module.
    cls.hIcon = LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(100));

    return CWindow::RegisterWindowClass(&cls);
}

// static
CObjectDebugWindow *CObjectDebugWindow::Create(IScreenshotEditor *pse, DWORD dwExStyle, DWORD dwStyle, int x, int y, int cx, int cy, HWND hwndParent)
{
    if (FAILED(RegisterWindowClass()))
    {
        return nullptr;
    }

    CObjectDebugWindow *pWnd = CWindow::Create(
        dwExStyle,
        TEXT("Object Debug"),
        dwStyle,
        x, y,
        cx, cy,
        hwndParent,
        nullptr,
        g_hinst,
        pse
    );

    return pWnd;
}