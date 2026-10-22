#pragma once
#include "pch.h"
#include "window.h"
#include "screenshot_manager.h"
#include "dynarray.h"

enum ScreenshotEditorTool
{
    SSET_SELECT,
    SSET_DRAG,
    SSET_SHOWCURSOR, // Not really a tool.
    SSET_ILLEGAL, // Used as a fallback if an extension tool fails.

    SSET_EXTENSIONFIRST, // Used for extensions.
};

/**
 * Modes for the drag tool.
 */
enum DragMode
{
    DRAGM_DRAG = 0,
    DRAGM_SIZEN = 1,
    DRAGM_SIZES = 2,
    DRAGM_SIZEW = 1 << 2,
    DRAGM_SIZEE = 2 << 2,
    DRAGM_SIZENW = DRAGM_SIZEN | DRAGM_SIZEW, // Top-left corner
    DRAGM_SIZENE = DRAGM_SIZEN | DRAGM_SIZEE, // Top-right corner
    DRAGM_SIZESW = DRAGM_SIZES | DRAGM_SIZEW, // Bottom-left corner
    DRAGM_SIZESE = DRAGM_SIZES | DRAGM_SIZEE, // Bottom-right corner
};

enum Side
{
    SIDE_NONE   = 0,
    SIDE_TOP    = 1,
    SIDE_BOTTOM = 2,
    SIDE_LEFT   = 1 << 2,
    SIDE_RIGHT  = 2 << 2,
};

struct ExtensionToolInfo
{
    IScreenshotEditorTool *pTool;
    UINT idTool;
    const TCHAR *pszToolName;
};

//
// The main toolbar, used by both the floating toolbar and in the windowed editor.
//
class CEditorToolbar : public CWindow<CEditorToolbar>
{
    DEFINE_WINDOW_CLASS("screenkirk_EditorToolbar");

private:
    class CScreenshotEditorWindow *_pEditor;
    HWND _hwndToolbar;
    CDynamicArray<HBITMAP> _vhbmIcons;
    UINT _uDpi;

protected:
    LRESULT v_WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
    LRESULT _OnCreate(CREATESTRUCT *pcs);
    LRESULT _OnDestroy();
    LRESULT _OnCommand(WPARAM wParam, LPARAM lParam);
    LRESULT _OnNotify(NMHDR *pnmh, WPARAM wParam, bool *pfHandled);

    HRESULT _GenerateToolIcon(const TCHAR *pszToolName, OUT HBITMAP *phbmOut);

    HRESULT _UnselectTool();

public:
    static HRESULT RegisterWindowClass();

    /**
     * Creates the editor toolbar.
     */
    static CEditorToolbar *Create(class CScreenshotEditorWindow *pEditor, DWORD dwExStyle, DWORD dwStyle, int x, int y, int cx, int cy, HWND hwndParent);

    CEditorToolbar()
        : _pEditor(nullptr)
        , _hwndToolbar(nullptr)
        , _uDpi(96)
    {
    }

    inline class CScreenshotEditorWindow *GetOwnerEditor() { return _pEditor; }

    HRESULT SelectOrdinalTool(int idx);
    HRESULT OnToolChanged(ScreenshotEditorTool toolNew);
};

//
// Floating toolbar action strip
//
class CEditorActionsStrip : public CWindow<CEditorActionsStrip>
{
    DEFINE_WINDOW_CLASS("screenkirk_EditorActionsStrip");

private:
    struct ExternalEditorDef
    {
        TCHAR szName[MAX_PATH];
        TCHAR szProgramPath[MAX_PATH];
        HICON hIcon;
        HBITMAP hbmIcon;
        HBITMAP hbmIconMenu; // The bitmap for the icon used in menus on old systems.
    };

    class CScreenshotEditorWindow *_pEditor;
    HWND _hwndActions;
    CDynamicArray<ExternalEditorDef> _vExternEditors;
    HBITMAP _hbmIconDiscard;
    HBITMAP _hbmIconSave;
    HBITMAP _hbmIconCopy;
    UINT _uDpi;

protected:
    LRESULT v_WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;

    LRESULT _OnCreate(CREATESTRUCT *pcs);
    LRESULT _OnDestroy();
    LRESULT _OnCommand(WPARAM wParam, LPARAM lParam);
    LRESULT _OnNotify(NMHDR *pnmh, WPARAM wParam, LPARAM lParam, bool *pfHandled);

    HRESULT _LoadIcons();
    HRESULT _EnumOpenWithHandlers();

public:
    enum Command
    {
        IDM_DISCARD = 100,
        IDM_COPY,
        IDM_SAVE,
        IDM_OPENINEXTERNALEDITOR,

        IDM_EXTERNEDITMANAGE = 200,
        IDM_EXTERNEDITFIRST = 201,
        IDM_EXTERNEDITLAST = 299,

        IDM_TOOLFIRST = 300,
    };

    static HRESULT RegisterWindowClass();

    /**
     * Creates the floating toolbar.
     */
    static CEditorActionsStrip *Create(class CScreenshotEditorWindow *pEditor, DWORD dwExStyle, DWORD dwStyle, int x, int y, int cx, int cy, HWND hwndParent);

    CEditorActionsStrip()
        : _pEditor(nullptr)
        , _hwndActions(nullptr)
        , _hbmIconDiscard(nullptr)
        , _hbmIconCopy(nullptr)
        , _hbmIconSave(nullptr)
        , _uDpi(96)
    {
    }
};

//
// Toolbar for properties for the current tool.
//
class CEditorToolProperties : public CWindow<CEditorToolProperties>
{
    DEFINE_WINDOW_CLASS("screenkirk_EditorToolProperties");

private:
    enum PropertyOwnerType
    {
        PROPOWNERT_TOOL,
        PROPOWNERT_OBJECT,
    };

    struct PropertyInfo
    {
        IScreenshotEditorToolProperty *pProp;
        PropertyOwnerType ownerType;
        union
        {
            IScreenshotEditorTool *pTool;
            IScreenshotEditorObject *pObj;
        } owner;
    };

    class CScreenshotEditorWindow *_pEditor;
    HWND _hwndToolbar;
    CDynamicArray<HBITMAP> _vhbmIcons;
    CDynamicArray<PropertyInfo> _vPropInfo;
    UINT _uDpi;

protected:
    LRESULT v_WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
    LRESULT _OnCreate(CREATESTRUCT *pcs);
    LRESULT _OnDestroy();
    LRESULT _OnCommand(WPARAM wParam, LPARAM lParam);
    LRESULT _OnNotify(NMHDR *pnmh, WPARAM wParam, bool *pfHandled);

    HRESULT _CreateToolbar();
    HRESULT _EnumerateProperties(
        IScreenshotEditorPropertyProvider *pProvider, CDynamicArray<IScreenshotEditorToolProperty *> *pvpProps);
    HRESULT _ClearPropInfo();

public:
    static HRESULT RegisterWindowClass();

    /**
     * Creates the editor toolbar.
     */
    static CEditorToolProperties *Create(class CScreenshotEditorWindow *pEditor, DWORD dwExStyle, DWORD dwStyle, int x, int y, int cx, int cy, HWND hwndParent);

    CEditorToolProperties()
        : _pEditor(nullptr)
        , _hwndToolbar(nullptr)
        , _uDpi(96)
    {
    }

    inline class CScreenshotEditorWindow *GetOwnerEditor() { return _pEditor; }

    HRESULT Refresh(IScreenshotEditorTool *pToolSel, IScreenshotEditorObject *pObjSel);
};

//
// Editor floating toolbar (used in fullscreen mode)
//
class CEditorFloatingToolbar : public CWindow<CEditorFloatingToolbar>
{
    DEFINE_WINDOW_CLASS("screenkirk_EditorFloatingToolbar");

private:
    struct CreateData
    {
        CEditorToolbar *pToolbar;
        CEditorToolProperties *pToolProps;
    };

    class CScreenshotEditorWindow *_pEditor;
    CEditorToolbar *_pToolbarTools;
    CEditorActionsStrip *_pActionStrip;
    CEditorToolProperties *_pToolProps;

protected:
    LRESULT v_WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;

    HRESULT _OnCreate();
    HRESULT _DoLayout();

public:
    static HRESULT RegisterWindowClass();

    /**
     * Creates the floating toolbar.
     */
    static CEditorFloatingToolbar *Create(CEditorToolbar *pToolbar, CEditorToolProperties *pToolProps);

	CEditorFloatingToolbar()
		: _pEditor(nullptr)
		, _pToolbarTools(nullptr)
		, _pActionStrip(nullptr)
        , _pToolProps(nullptr)
	{
	}
};

class CRenderObject
{
public:
    IScreenshotEditorObject *_pObj;
    IScreenshotEditorObjectRenderer *_pRenderer;
    IScreenshotEditorObjectRendererGDI *_pRendererGdi;
    HBITMAP _hbmLayer;
    RECT _rcLatestLog;
    RECT _rcLatestVisual;

	CRenderObject()
		: _pObj(nullptr)
		, _pRenderer(nullptr)
		, _pRendererGdi(nullptr)
		, _hbmLayer(nullptr)
	{
        ZeroMemory(&_rcLatestLog, sizeof(_rcLatestLog));
        ZeroMemory(&_rcLatestVisual, sizeof(_rcLatestVisual));
	}

    inline bool HasGdiRenderer()
    {
        return _pRendererGdi != nullptr;
    }
};

class CScreenshotEditorRendererGDI
{
    CScreenshotContext *_pScreenshotCtx;
    HWND _hwndRenderTarget;
    HPEN _hpenSelect;
    HPEN _hpenSizingHelpers;
    HBITMAP _hbmScreenshotLight;
    HBITMAP _hbmScreenshotDimmed;
    HBITMAP _hbmCursor;
    HBITMAP _hbmCursorNone;
    HBITMAP _hbmCursorDimmed;
    HBITMAP _hbmCursorDimmedNone;
    HBITMAP *_hbmMipmaps;
    CRenderObject *_pRenderObjSel;
    RECT _rcSelection;
    RECT _rcSelectionVisual;
    RECT _rcCursor;
    CDynamicArray<CRenderObject> _vRenderObjs;
    int _cMipmaps;
    int _iSelMarqueeFrame;
    float _iZoom;

    struct Bitmap
    {
        bool fHasAnySelectionMade : 1;
        bool fPreviewSelection : 1;
        bool fAnyObjectDirty : 1;
        bool fDrawSelectionSizeHelpers : 1;
        bool fDrawMarqueeSelection : 1;
        bool fCopiedScreenshot : 1;
        bool fSelectionBorderAnimDirty : 1;
        bool fSelectionDirty : 1;
        bool fSelectionDirtyNorth : 1;
        bool fSelectionDirtyEast : 1;
        bool fSelectionDirtySouth : 1;
        bool fSelectionDirtyWest : 1;
        bool fEntireFrameDirty : 1;
        bool fCursorVisible : 1;
        bool fRender : 1;
    } _bmp;

    HRESULT _PaintSelectionRectangle(HDC hdc, RECT *prc, bool fUseMarquee);
    HRESULT _PaintSizingHelpers(HDC hdc, RECT *prc);
    HRESULT _PaintRenderObjectVisualBuffer(
        HDC hdcRenderTarget, RECT *prcPaint, CRenderObject *pRenderObject);
    void _UpdateMarquee();
    HRESULT _StartSelectionMarqueeTimer();
    HRESULT _EndSelectionMarqueeTimer();
    HRESULT _DrawMarqueeDottedRectangle(HDC hdc, RECT *prc);
    HRESULT _MakeDimmedScreenshot(HBITMAP hbmSrc, SIZE bmpDimensions, OUT HBITMAP *phbmOut);
    HRESULT _DimScreenshot32BPPAlpha(HBITMAP hbmSrc, SIZE bmpDimensions, OUT HBITMAP *phbmOut);
    HRESULT _DimScreenshot32BPP(HBITMAP hbmSrc, SIZE bmpDimensions, OUT HBITMAP *phbmOut);
    HRESULT _DimScreenshot24BPP(HBITMAP hbmSrc, SIZE bmpDimensions, OUT HBITMAP *phbmOut);
    HRESULT _DitherScreenshot(BITMAP *pbm, HBITMAP hbmSrc, SIZE bmpDimensions, OUT HBITMAP *phbmOut);
    HRESULT _FindRenderObjectFromInterfaceObject(
        IScreenshotEditorObject *pIfaceObj, OUT CRenderObject **ppRenderObjOut, OUT int *pIdxOut = nullptr);
    HBITMAP _MakeHideCursorBuffer(HBITMAP hbm);
    HRESULT _MakeCursorBuffers();
    HRESULT _ShowHideCursor(bool fVisible);

public:
    static constexpr int c_iDimAmount = 0xFF * 0.75;
    static constexpr int c_iRadiusSelHelper = 9;
    static constexpr int c_idTimerMarquee = 101;

    CScreenshotEditorRendererGDI(CScreenshotContext *pCtx, HWND hwndRenderTarget)
        : _pScreenshotCtx(pCtx)
        , _hwndRenderTarget(hwndRenderTarget)
		, _hpenSelect(nullptr)
		, _hpenSizingHelpers(nullptr)
        , _hbmScreenshotLight(pCtx->_hbmScreenshot)
		, _hbmScreenshotDimmed(nullptr)
        , _hbmCursor(nullptr)
        , _hbmCursorNone(nullptr)
        , _hbmCursorDimmed(nullptr)
        , _hbmCursorDimmedNone(nullptr)
		, _hbmMipmaps(nullptr)
        , _pRenderObjSel(nullptr)
        , _cMipmaps(0)
        , _iSelMarqueeFrame(0)
        , _iZoom(1.0)
    {
        ZeroMemory(&_rcSelection, sizeof(_rcSelection));
        ZeroMemory(&_rcSelectionVisual, sizeof(_rcSelectionVisual));
        ZeroMemory(&_bmp, sizeof(_bmp));
    }

    ~CScreenshotEditorRendererGDI();

    HRESULT Initialize();
    HRESULT Paint(HDC hdc, RECT *prcPaint = nullptr);
    HRESULT RenderToBitmap(OUT HBITMAP *phbmOut);
    HRESULT UpdateSelection(RECT *prcNew);
    HRESULT PreviewSelection(RECT *prcNew);
    HRESULT UpdateSizingHelpersVisibility(bool fVisible);
    HRESULT SetMarqueeSelection(bool fMarquee);
    HRESULT HandleWindowMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    HRESULT ShowCursor();
    HRESULT HideCursor();
    bool IsCursorShown();
    HRESULT CreateRenderObject(IScreenshotEditorObject *pObj);
    HRESULT RemoveRenderObject(IScreenshotEditorObject *pObj);
    HRESULT InvalidateRenderObject(IScreenshotEditorObject *pObj);
    HRESULT ChangeSelectedObject(IScreenshotEditorObject *pObj);
};

class CEditHistoryManager
{
public:
    enum EditHistoryActionId
    {
        EHID_SELECTREGION,
        EHID_OBJECTINSERT,
        EHID_OBJECTREMOVE,
        EHID_OBJECTMOVE,
        EHID_OBJECT, // Unique handler for an object.
    };

    struct EditHistoryData
    {
        EditHistoryActionId idAction;

        union
        {
            struct SelectionHistoryEntry
            {
                RECT rcRegionNew;
            } selection;

            IScreenshotEditorObject *pObject;

            struct ObjectMoveEditHistoryEntry
            {
                IScreenshotEditorObject *pObject;
                RECT rcNew;
                RECT rcOld;
            } objectMove;

            struct ObjectEditHistoryEntry
            {
                IScreenshotEditorObject *pObject;
                ULONG ulEventId;
            } objectUnique;
        } data;
    };

private:
    CDynamicArray<EditHistoryData> _vData;
    int _iPos;
    bool _fIsLocked;

    HRESULT _Push(EditHistoryData *pData);
    HRESULT _DestroyEntry(int idxEntry);

public:
    CEditHistoryManager()
        : _iPos(0)
        , _fIsLocked(false)
    {
    }

    ~CEditHistoryManager();

    inline int GetPosition() { return _iPos; }
    inline int SetPosition(int iPos) { _iPos = iPos; }
    inline void Rewind() { _iPos--; if (_iPos < 0) _iPos = 0; }
    inline void Progress() { _iPos++; if (_iPos > _vData.GetSize() - 1) _iPos = _vData.GetSize() - 1;  }

    // I don't think this architecture with actually popping the entry will work. The position
    // should be pushed back, and future entries are destroyed if there is anything beyond the
    // cursor when pushing.

    HRESULT PushSelectRegion(RECT *prcSelectionNew);
    HRESULT PushObjectCreate(IScreenshotEditorObject *pObj);
    HRESULT PushObjectRemove(IScreenshotEditorObject *pObj);
    HRESULT PushObjectMove(IScreenshotEditorObject *pObj, RECT *prcNew, RECT *prcOld);
    HRESULT PushObjectUnique(IScreenshotEditorObject *pObj, ULONG ulEventId);
    HRESULT GetPrevious(EditHistoryData *pData);
    HRESULT GetNext(EditHistoryData *pData);
    HRESULT GetCurrent(EditHistoryData *pData);
    HRESULT Lock();
    HRESULT Unlock();
};

class CScreenshotEditorToolPropertyBase
    : public IScreenshotEditorToolProperty
{
    TCHAR _szLabel[MAX_PATH];
    IScreenshotEditorTool *_pTool;

public:
    IMPLEMENT_IUNKNOWN;

    //@Begin IObjectWithSite
    STDMETHODIMP GetSite(REFIID riid, void **ppvSite) override;
    STDMETHODIMP SetSite(IUnknown *pUnkSite) override;
    //@End IObjectWithSite

    //@Begin IScreenshotEditorToolProperty
    STDMETHODIMP GetLabel(OUT TCHAR **ppszOut) override;
    STDMETHOD(GetIcon)(OUT HICON *phIcon) PURE;
    STDMETHODIMP GetHWND(HWND *phwnd) override { return E_NOTIMPL; }
    STDMETHODIMP CreateToolbarItem(HWND hwndToolbar) override { return S_FALSE; }
    STDMETHOD(OnClick)() PURE;
    STDMETHOD(CreateMenuItem)(IN HMENU hMenu) PURE;
    STDMETHOD(OnClickMenuItem)() PURE;
    STDMETHOD(MeasureToolbar)(IN RECT *prcToolbar, IN OUT RECT *prcProperty) PURE;
    STDMETHOD(GetValue)(VARIANT *pvar) PURE;
    STDMETHOD(GetDefaultValue)(VARIANT *pvar) PURE;
    //@End IScreenshotEditorToolProperty

    CScreenshotEditorToolPropertyBase(TCHAR *pszLabel)
        : _pTool(nullptr)
    {
        _tcscpy_s(_szLabel, pszLabel);
    }
};

class CScreenshotEditorToolPropertyLabel
    : public CScreenshotEditorToolPropertyBase
{
public:
    //@Begin IScreenshotEditorToolProperty
    STDMETHODIMP GetIcon(OUT HICON *phIcon) override { return E_NOTIMPL; }
    STDMETHODIMP OnClick() override;
    STDMETHODIMP CreateMenuItem(IN HMENU hMenu) override;
    STDMETHODIMP OnClickMenuItem() override;
    STDMETHODIMP MeasureToolbar(IN RECT *prcToolbar, IN OUT RECT *prcProperty) override;
    STDMETHODIMP GetValue(VARIANT *pvar) override;
    STDMETHODIMP GetDefaultValue(VARIANT *pvar) override;
    //@End IScreenshotEditorToolProperty

    CScreenshotEditorToolPropertyLabel(TCHAR *pszLabel)
        : CScreenshotEditorToolPropertyBase(pszLabel)
    {
    }
};

class CScreenshotEditorToolPropertyColor
    : public CScreenshotEditorToolPropertyBase
{
public:
    //@Begin IScreenshotEditorToolProperty
    STDMETHODIMP GetIcon(OUT HICON *phIcon) override { return E_NOTIMPL; }
    STDMETHODIMP OnClick() override;
    STDMETHODIMP CreateMenuItem(IN HMENU hMenu) override;
    STDMETHODIMP OnClickMenuItem() override;
    STDMETHODIMP MeasureToolbar(IN RECT *prcToolbar, IN OUT RECT *prcProperty) override;
    STDMETHODIMP GetValue(VARIANT *pvar) override;
    STDMETHODIMP GetDefaultValue(VARIANT *pvar) override;
    //@End IScreenshotEditorToolProperty

    CScreenshotEditorToolPropertyColor(TCHAR *pszLabel)
        : CScreenshotEditorToolPropertyBase(pszLabel)
    {
    }
};

class CScreenshotEditorToolPropertyButton
    : public CScreenshotEditorToolPropertyBase
{
public:
    //@Begin IScreenshotEditorToolProperty
    STDMETHODIMP GetIcon(OUT HICON *phIcon) override { return E_NOTIMPL; }
    STDMETHODIMP CreateToolbarItem(HWND hwndToolbar) override;
    STDMETHODIMP OnClick() override;
    STDMETHODIMP CreateMenuItem(IN HMENU hMenu) override;
    STDMETHODIMP OnClickMenuItem() override;
    STDMETHODIMP MeasureToolbar(IN RECT *prcToolbar, IN OUT RECT *prcProperty) override;
    STDMETHODIMP GetValue(VARIANT *pvar) override;
    STDMETHODIMP GetDefaultValue(VARIANT *pvar) override;
    //@End IScreenshotEditorToolProperty

    CScreenshotEditorToolPropertyButton(TCHAR *pszLabel)
        : CScreenshotEditorToolPropertyBase(pszLabel)
    {
    }
};

class CScreenshotEditorSelectionTemplateObject
    : public IScreenshotEditorObject
{
    IScreenshotEditor *_pEditor;
    RECT _rcPosLogical;

public:
    IMPLEMENT_IUNKNOWN;

    //@Begin IObjectWithSite
    STDMETHODIMP GetSite(REFIID riid, void **ppvSite) override;
    STDMETHODIMP SetSite(IUnknown *pUnkSite) override;
    //@End IObjectWithSite

    //@Begin IScreenshotRendererObject
    STDMETHODIMP_(const TCHAR *) GetClassName() override
    {
        return TEXT("quikshot.CScreenshotEditorSelectionObject");
    }
    STDMETHODIMP_(ULONG) GetFlags() override
    {
        return SSEOF_SELECTIONTEMPLATE | SSEOF_NODRAG | SSEOF_NORENDERER;
    }
    STDMETHODIMP InsertedIntoDocument() override { return S_OK; }
    STDMETHODIMP GetLogicalRect(IN RECT *prc) override;
    STDMETHODIMP GetVisualRect(IN RECT *prc) override { return GetLogicalRect(prc); }
    STDMETHODIMP_(BOOL) IsVisualDirty() override { return false; }
    STDMETHODIMP Move(RECT *prcNew) override { _rcPosLogical = *prcNew; return S_OK; }
    STDMETHODIMP CreateRenderer(const REFIID riid, OUT IScreenshotEditorObjectRenderer **ppRendererOut) override { return E_NOINTERFACE; }
    STDMETHODIMP CreateHistoryManager(const REFIID riid, OUT IScreenshotEditorObjectHistoryManager **ppHistoryManagerOut) override { return E_NOTIMPL; }
    //@End IScreenshotRendererObject

    CScreenshotEditorSelectionTemplateObject()
        : _uRefCount(0)
        , _pEditor(nullptr)
    {
        ZeroMemory(&_rcPosLogical, sizeof(_rcPosLogical));
    }
};

//
// Editor main window. 
//
class CScreenshotEditorWindow 
    : public CWindow<CScreenshotEditorWindow>
    , public IScreenshotEditor
{
    DEFINE_WINDOW_CLASS("screenkirk_ScreenshotEditorWindow");

private:
    class CEnumObjects : public IEnumUnknown
    {
        IMPLEMENT_IUNKNOWN;

    private:
        CDynamicArray<IScreenshotEditorObject *> *_pvObjs;
        UINT _idx;

    public:
        CEnumObjects(CDynamicArray<IScreenshotEditorObject *> *pvObjs)
            : _pvObjs(pvObjs)
            , _idx(0)
        {
        }

        STDMETHODIMP Next(ULONG celt, IUnknown **rgelt, ULONG *pceltFetched) override;
        STDMETHODIMP Skip(ULONG celt) override;
        STDMETHODIMP Reset() override;
        STDMETHODIMP Clone(IEnumUnknown **ppenum) override;
    };

    CScreenshotContext *_pScreenshotCtx;
    CScreenshotEditorRendererGDI *_pRenderer;
    CEditHistoryManager *_pHistoryMgr;
    CEditorToolbar *_pToolbar;
    CEditorToolProperties *_pToolProps;
    CEditorFloatingToolbar *_pFloatingToolbar;
    IScreenshotEditorObject *_pSelectedObject;
    IScreenshotEditorObject *_pHoveredObj;
    CDynamicArray<IScreenshotEditorObject *> _vObjs;
    CDynamicArray<ExtensionToolInfo> _vExtToolInfo;
    POINT _ptSelectionOrigin;
    RECT _rcSelection;
    RECT _rcDragSelectCur;
    RECT _rcDragBegin;
    ScreenshotEditorTool _tool;
    IScreenshotEditorTool *_pExtTool; // The current extension tool, if any.
    int _iToolMode;
    bool _fEnumeratedWindows;
    bool _fIsSelectingRegion;
    bool _fHasAnySelectionMade;

protected:
    inline bool _IsExtensionTool()
    {
        return _tool >= SSET_EXTENSIONFIRST && _pExtTool;
    }

    LRESULT v_WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
    LRESULT _OnCreate(CREATESTRUCT *pCs);
    LRESULT _OnDestroy();
    LRESULT _OnKeyDown(WPARAM virtualKey, LPARAM lParam);
    LRESULT _OnMouseMove(int x, int y, WPARAM flags);
    LRESULT _OnMouseLButtonDown(int x, int y, WPARAM flags);
    LRESULT _OnMouseLButtonUp(int x, int y, WPARAM flags);
    LRESULT _OnMouseRButtonDown(int x, int y, WPARAM flags);
    LRESULT _OnMouseRButtonUp(int x, int y, WPARAM flags);

    HRESULT _BeginGetWindowPositions();
    HRESULT _HandleGetWindowPositions();
    static DWORD WINAPI GetWindowPositionsThreadProc(void *lpParameter);
    static HRESULT _OnGetWindowPositions(void *pvData)
    {
        PostMessage((HWND)pvData, WM_SSE_GETWINDOWPOSITIONS, 0, 0);
        return S_OK;
    }

    HRESULT _ApplyChangesToImage();

    HRESULT _LoadExtensionTools();

    HRESULT _ApplyHistoryState(CEditHistoryManager::EditHistoryData *pehd, bool fUndo);
    HRESULT _Undo();
    HRESULT _Redo();

    HRESULT _ChangeTool(ScreenshotEditorTool newTool);
    HRESULT _EnsureToolbar();
    void _ShowFloatingToolbar();
    HRESULT _DetermineFloatingToolbarPosition(RECT *prcBox, OUT POINT *ptOut);
    void _HideFloatingToolbar();
    void _UpdateCursor();
    void _CancelSelection(bool fPushHistory);
    int _ComputeDragMode(POINT ptCursor, RECT *prcDraggedObj);
    HRESULT _RemoveObject(IScreenshotEditorObject *pObj);
    IScreenshotEditorObject *_GetMousedOverObject();
    HRESULT _SelectObject(IScreenshotEditorObject *pObj);
    HRESULT _BuildObjectMenu(HMENU hmenu, IScreenshotEditorObject *pObj);
    HRESULT _DoPreviewSelection(IScreenshotEditorObject *pObj);
    HRESULT _BuildSelectionTemplatesForMonitors();

public:
    static constexpr int c_iPaddingFloatingTB = 10;

    enum WM
    {
        WM_SSE_GETWINDOWPOSITIONS = WM_APP + 1,
        WM_SSE_CHANGETOOL,
        WM_SSE_COPYTOCLIPBOARD,
        WM_SSE_SAVEIMAGE,
        WM_SSE_SHOWCURSOR,
        WM_SSE_HIDECURSOR,
    };

    enum Command
    {
        IDM_REMOVEOBJECT = 200,
    };

    //@Begin IUnknown
    STDMETHODIMP QueryInterface(const REFIID riid, void **ppvOut) override;
    STDMETHODIMP_(ULONG) AddRef() override
    {
        // We don't use COM reference counting since the lifetime of this object
        // is bound to the window. Unfortunately, we just must risk crashing if
        // a bad user doesn't respond to our tear-down requests.
        return 1;
    }
    STDMETHODIMP_(ULONG) Release() override
    {
        return 1;
    }
    //@End IUnknown

    //@Begin IScreenshotEditor
    STDMETHODIMP InsertObject(IScreenshotEditorObject *pObj) override;
    STDMETHODIMP RemoveObject(IScreenshotEditorObject *pObj) override;
    STDMETHODIMP InvalidateObject(IScreenshotEditorObject *pObj) override;
    STDMETHODIMP GetScreenshotContext(OUT IScreenshotContext **ppContext) override;
    STDMETHODIMP GetCursorPosition(OUT POINT *pptCursor) override;
    STDMETHODIMP_(HWND) GetEditorHWND() override;
    STDMETHODIMP EnumObjects(OUT IEnumUnknown **ppEnumUnknown) override;
    STDMETHODIMP GetSelectedRegion(RECT *prc) override;
    STDMETHODIMP SetSelectedRegion(RECT *prc) override;
    STDMETHODIMP SetSelectedObject(IScreenshotEditorObject *pObj) override;
    STDMETHODIMP CreateStandardToolProperty(
        const CLSID &rclsid,
        IN TCHAR *pszLabel,
        OUT interface IScreenshotEditorToolProperty **ppObjOut
    ) override;
    //@End IScreenshotEditor

	CScreenshotEditorWindow()
		: _pScreenshotCtx(nullptr)
		, _pRenderer(nullptr)
        , _pToolbar(nullptr)
        , _pToolProps(nullptr)
		, _pFloatingToolbar(nullptr)
        , _pSelectedObject(nullptr)
        , _pHoveredObj(nullptr)
        , _tool(SSET_SELECT)
		, _pExtTool(nullptr)
        , _iToolMode(0)
        , _fEnumeratedWindows(false)
        , _fIsSelectingRegion(false)
        , _fHasAnySelectionMade(false)
	{
        _pHistoryMgr = new CEditHistoryManager();

        ZeroMemory(&_ptSelectionOrigin, sizeof(_ptSelectionOrigin));
        ZeroMemory(&_rcSelection, sizeof(_rcSelection));
        ZeroMemory(&_rcDragBegin, sizeof(_rcDragBegin));
	}

    bool IsCursorShown();
    HRESULT RefreshPropertiesUI();
    HRESULT CopyToClipboardAndAccept();
    HRESULT SaveImageToFileAndAccept();
    int GetExtensionToolCount();
    HRESULT GetExtensionToolInfo(int idx, OUT ExtensionToolInfo *pExtToolInfo);

    static HRESULT RegisterWindowClass();

    /**
     * Creates the main screenshot editor viewport window.
     * 
     * @param _pScreenshotCtx
     *      Taken ownership of.
     */
    static CScreenshotEditorWindow *CreateAndShow(CScreenshotContext *pScreenshotCtx);

#ifdef _DEBUG
    friend class CEditHistoryManager;
#endif
};

//
// Host window for floating (non-fullscreen) screenshot editors.
//
const TCHAR c_szFloatingScreenshotEditorWindowClassName[] = TEXT("screenkirk_FloatingScreenshotEditorWindow");
class CFloatingScreenshotEditorWindow : public CWindow<CFloatingScreenshotEditorWindow>
{
    DEFINE_WINDOW_CLASS("screenkirk_FloatingScreenshotEditorWindow");

private:
    CScreenshotEditorWindow *_pEditor;
    HWND _hwndToolbar;

protected:
    LRESULT v_WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;

public:
    static HRESULT RegisterWindowClass();

    /**
     *
     */
    static CFloatingScreenshotEditorWindow *CreateAndShow(CScreenshotEditorWindow *pEditor);

	CFloatingScreenshotEditorWindow()
		: _pEditor(nullptr)
		, _hwndToolbar(nullptr)
	{
	}
};