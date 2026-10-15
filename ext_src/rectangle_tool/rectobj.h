#pragma once
#include "pch.h"
#include "propprov.h"

class CRectangleObject
    : public IScreenshotEditorObject
    , public IScreenshotEditorObjectRendererGDI
{
public:
    enum RectangleType
    {
        TYPE_SOLID,
        TYPE_HIGHLIGHTER,
    };

private:
    CPropertyProvider _propProvider;
    IScreenshotEditor *_pEditor;
    HDC _hdc;
    RECT _rcPosLogical;
    RECT _rcPosVisual;
    RectangleType _type;
    bool _fDirty;

public:
    IMPLEMENT_IUNKNOWN;

    //@Begin IObjectWithSite
    STDMETHODIMP GetSite(REFIID riid, void **ppvSite) override;
    STDMETHODIMP SetSite(IUnknown *pUnkSite) override;
    //@End IObjectWithSite

    //@Begin IScreenshotRendererObject
    STDMETHODIMP_(const TCHAR *) GetClassName() override;
    STDMETHODIMP_(ULONG) GetFlags() override;
    STDMETHODIMP InsertedIntoDocument() override;
    STDMETHODIMP GetLogicalRect(IN RECT *prc) override;
    STDMETHODIMP GetVisualRect(IN RECT *prc) override;
    STDMETHODIMP_(BOOL) IsVisualDirty() override;
    STDMETHODIMP Move(RECT *prcNew) override;
    STDMETHODIMP CreateRenderer(const REFIID riid, OUT IScreenshotEditorObjectRenderer **ppRendererOut) override;
    STDMETHODIMP CreateHistoryManager(const REFIID riid, OUT IScreenshotEditorObjectHistoryManager **ppHistoryManagerOut) override;
    //@End IScreenshotRendererObject

    //@Begin IScreenshotRendererObjectRendererGDI
    STDMETHODIMP Paint() override;
    STDMETHODIMP SetGdiParameters(HDC hdc, RECT *prcPaint) override;
    //@End IScreenshotRendererObjectRendererGDI

    CRectangleObject(RectangleType type)
        : _uRefCount(0)
        , _pEditor(nullptr)
        , _hdc(nullptr)
        , _type(type)
        , _fDirty(false)
    {
        ZeroMemory(&_rcPosLogical, sizeof(_rcPosLogical));
        ZeroMemory(&_rcPosVisual, sizeof(_rcPosVisual));

        // We set the site of the property provider, but instantly release that reference
        // because it is a circular reference.
        if (SUCCEEDED(_propProvider.SetSite(static_cast<IScreenshotEditorObject *>(this))))
        {
            Release();
        }
    }

    HRESULT UpdatePosition(RECT *prc);
};
