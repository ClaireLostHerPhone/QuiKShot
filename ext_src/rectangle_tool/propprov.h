#pragma once
#include "pch.h"

class CPropertyProvider
    : public IScreenshotEditorPropertyProvider
{
    static constexpr int c_iNumProperties = 2;

    IScreenshotEditorToolProperty *_pPropColorFill;
    IScreenshotEditorToolProperty *_pPropColorBorder;
    COLORREF _crFill;
    COLORREF _crBorder;
    bool _fPropsInitialized = false;

    HRESULT _InitializeProperties();

public:

    //@Begin IScreenshotEditorPropertyProvider
    STDMETHODIMP GetProperties(SAFEARRAY **ppsaProperties) override;
    STDMETHODIMP OnPropertyChanged(IScreenshotEditorToolProperty *pProperty) override;
    //@End IScreenshotEditorPropertyProvider

    CPropertyProvider()
        : _pPropColorFill(nullptr)
        , _pPropColorBorder(nullptr)
        , _crFill(RGB(255, 0, 0))
        , _crBorder(RGB(0, 0, 0))
    {
    }
};
