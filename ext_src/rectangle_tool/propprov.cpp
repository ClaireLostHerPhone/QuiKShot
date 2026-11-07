#include "pch.h"
#include "propprov.h"

HRESULT CPropertyProvider::_InitializeProperties()
{
    if (_fPropsInitialized)
        return S_OK;

    HRESULT hr = E_PENDING;

    IObjectWithSite *pows = nullptr;
    if (SUCCEEDED(QueryInterface(IID_PPV_ARGS(&pows))))
    {
        IScreenshotEditor *pEditor = nullptr;
        if (SUCCEEDED(pows->GetSite(IID_PPV_ARGS(&pEditor))))
        {
            pEditor->CreateStandardToolProperty(TOOLPROPID_COLOR, (WCHAR *)TEXT("Fill color"), &_pPropColorFill);
            _pPropColorFill->SetSite(this);
            pEditor->CreateStandardToolProperty(TOOLPROPID_COLOR, (WCHAR *)TEXT("Border color"), &_pPropColorBorder);
            _pPropColorBorder->SetSite(this);

            _fPropsInitialized = true;
            hr = S_OK;
        }
    }

    return hr;
}

STDMETHODIMP CPropertyProvider::QueryInterface(const IID &riid, void **ppvOut)
{
    if (!ppvOut)
        return E_POINTER;
    return _pUnkSite->QueryInterface(riid, ppvOut);
}

STDMETHODIMP CPropertyProvider::GetSite(REFIID riid, void **ppvSite)
{
    if (!ppvSite)
        return E_POINTER;

    return _pUnkSite->QueryInterface(riid, ppvSite);
}

STDMETHODIMP CPropertyProvider::SetSite(IUnknown *pUnkSite)
{
    _pUnkSite = pUnkSite;
    HRESULT hr = E_FAIL;

    if (_pUnkSite)
    {
        _pUnkSite->Release();
        _pUnkSite = nullptr;
    }

    hr = S_OK;
    return hr;
}

STDMETHODIMP CPropertyProvider::GetEnumProperties(OUT IEnumUnknown **pEnumOut)
{
    if (!pEnumOut)
        return E_POINTER;

    _InitializeProperties();

    *pEnumOut = new CEnumProperties(this);
    if (!*pEnumOut)
        return E_OUTOFMEMORY;
    (*pEnumOut)->AddRef();
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

STDMETHODIMP CEnumProperties::QueryInterface(const IID &riid, void **ppvOut)
{
    if (IsEqualGUID(riid, IID_IUnknown)
        || IsEqualGUID(riid, IID_IEnumUnknown))
    {
        *ppvOut = this;
        AddRef();
        return S_OK;
    }

    return E_NOINTERFACE;
}

STDMETHODIMP CEnumProperties::Next(ULONG celt, IUnknown **rgelt, ULONG *pceltFetched)
{
    if (celt == 0)
        return S_OK;
    if (!rgelt)
        return E_POINTER;
    if (celt > 1 && !pceltFetched)
        return E_POINTER;

    int i = 0;

    if (_uiIdx == 0)
    {
        rgelt[i] = _pPropProvider->_pPropColorFill;
        _uiIdx++;
        i++;
    }

    if (_uiIdx == 1)
    {
        rgelt[i] = _pPropProvider->_pPropColorBorder;
        _uiIdx++;
        i++;
    }

    *pceltFetched = i;
    return S_OK;
}

STDMETHODIMP CEnumProperties::Skip(ULONG celt)
{
    _uiIdx += celt;
    return S_OK;
}

STDMETHODIMP CEnumProperties::Reset()
{
    _uiIdx = 0;
    return S_OK;
}

STDMETHODIMP CEnumProperties::Clone(IEnumUnknown **ppenum)
{
    if (!ppenum)
        return E_POINTER;

    *ppenum = new CEnumProperties(_pPropProvider);
    return S_OK;
}
