#include "pch.h"
#include "propprov.h"

HRESULT CPropertyProvider::_InitializeProperties()
{
    if (_fPropsInitialized)
        return S_OK;

    IObjectWithSite *pows = nullptr;
    if (SUCCEEDED(QueryInterface(IID_PPV_ARGS(&pows))))
    {
        IScreenshotEditor *pEditor = nullptr;
        if (SUCCEEDED(pows->GetSite(IID_PPV_ARGS(&pEditor))))
        {
            pEditor->CreateStandardToolProperty(TOOLPROPID_COLOR, (WCHAR *)TEXT("Fill color"), &_pPropColorFill);
            pEditor->CreateStandardToolProperty(TOOLPROPID_COLOR, (WCHAR *)TEXT("Border color"), &_pPropColorBorder);

            _fPropsInitialized = true;
            return S_OK;
        }
    }

    return E_PENDING;
}

STDMETHODIMP CPropertyProvider::GetProperties(SAFEARRAY **ppsaProperties)
{
    if (!ppsaProperties)
        return E_POINTER;

    _InitializeProperties();

    *ppsaProperties = SafeArrayCreateVector(VT_VOID, 0, c_iNumProperties);
    if (!*ppsaProperties)
        return E_OUTOFMEMORY;

    LONG i = 0;
    if (FAILED(SafeArrayPutElement(*ppsaProperties, &i, _pPropColorFill)))
    {
        SafeArrayDestroy(*ppsaProperties);
        return E_FAIL;
    }

    i++;
    if (FAILED(SafeArrayPutElement(*ppsaProperties, &i, _pPropColorBorder)))
    {
        SafeArrayDestroy(*ppsaProperties);
        return E_FAIL;
    }

    return S_OK;
}

STDMETHODIMP CPropertyProvider::OnPropertyChanged(IScreenshotEditorToolProperty *pProperty)
{
    if (pProperty == _pPropColorFill)
    {
        VARIANT var = { 0 };
        if (SUCCEEDED(pProperty->GetValue(&var)) && var.vt == VT_COLOR)
        {
            _crFill = var.lVal;
        }
    }
    else if (pProperty == _pPropColorBorder)
    {
        VARIANT var = { 0 };
        if (SUCCEEDED(pProperty->GetValue(&var)) && var.vt == VT_COLOR)
        {
            _crBorder = var.lVal;
        }
    }

    // TODO: Consider how to get this reported over to the owner object.

    return S_OK;
}