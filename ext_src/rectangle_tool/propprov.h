#pragma once
#include "pch.h"

class CEnumProperties : public IEnumUnknown
{
    class CPropertyProvider *_pPropProvider;
    UINT _uiIdx = 0;

public:
    IMPLEMENT_IUNKNOWN;

    CEnumProperties(class CPropertyProvider *pPropProvider)
        : _pPropProvider(pPropProvider)
    {
    }

    STDMETHODIMP Next(ULONG celt, IUnknown **rgelt, ULONG *pceltFetched) override;
    STDMETHODIMP Skip(ULONG celt) override;
    STDMETHODIMP Reset() override;
    STDMETHODIMP Clone(IEnumUnknown **ppenum) override;
};

class CPropertyProvider
    : public IScreenshotEditorPropertyProvider
{
    static constexpr int c_iNumProperties = 2;

    IUnknown *_pUnkSite;
    IScreenshotEditorToolProperty *_pPropColorFill;
    IScreenshotEditorToolProperty *_pPropColorBorder;
    COLORREF _crFill;
    COLORREF _crBorder;
    bool _fPropsInitialized = false;

    HRESULT _InitializeProperties();

public:
    IMPLEMENT_IUNKNOWN_NODELETE; // This is not heap-allocated.

    //@Begin IObjectWithSite
    STDMETHODIMP SetSite(IUnknown *pUnkSite) override;
    STDMETHODIMP GetSite(REFIID riid, void **ppvSite) override;
    //@End IObjectWithSite

    //@Begin IScreenshotEditorPropertyProvider
    STDMETHODIMP GetEnumProperties(OUT IEnumUnknown **pEnumOut) override;
    STDMETHODIMP OnPropertyChanged(IScreenshotEditorToolProperty *pProperty) override;
    //@End IScreenshotEditorPropertyProvider

    CPropertyProvider()
        : _uRefCount(0)
        , _pUnkSite(nullptr)
        , _pPropColorFill(nullptr)
        , _pPropColorBorder(nullptr)
        , _crFill(RGB(255, 0, 0))
        , _crBorder(RGB(0, 0, 0))
    {
    }

    friend class CEnumProperties;
};
