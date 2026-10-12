#pragma once
#include <windows.h>
#include <ShlObj.h>

HRESULT PortableInitialize();

int PortableGetSystemMetricsForDpi(int nIndex, UINT dpi);

HRESULT PortableAlphaBlend(
	HDC hdcDest, int xoriginDest, int yoriginDest, int wDest, int hDest,
	HDC hdcSrc, int xoriginSrc, int yoriginSrc, int wSrc, int hSrc, BLENDFUNCTION ftn
);

bool PortableIsAlphaBlendAvailable();

#if NTDDI_VERSION < 0x0600
enum _ASSOC_FILTER
{
    ASSOC_FILTER_NONE	        = 0,
    ASSOC_FILTER_RECOMMENDED	= 0x1
};
typedef int ASSOC_FILTER;

MIDL_INTERFACE("F04061AC-1659-4a3f-A954-775AA57FC083")
IAssocHandler : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE GetName( 
        /* [annotation][out][string] */ 
        LPWSTR *ppsz) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetUIName( 
        /* [annotation][out][string] */ 
        LPWSTR *ppsz) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetIconLocation( 
        /* [annotation][out][string] */ 
        LPWSTR *ppszPath,
        /* [annotation][out] */ 
        int *pIndex) = 0;

    virtual HRESULT STDMETHODCALLTYPE IsRecommended( void) = 0;

    virtual HRESULT STDMETHODCALLTYPE MakeDefault( 
        /* [annotation][string][in] */ 
        LPCWSTR pszDescription) = 0;

    virtual HRESULT STDMETHODCALLTYPE Invoke( 
        /* [annotation][in] */ 
        IDataObject *pdo) = 0;

    virtual HRESULT STDMETHODCALLTYPE CreateInvoker( 
        /* [annotation][in] */ 
        IDataObject *pdo,
        /* [annotation][out] */ 
        void **ppInvoker) = 0;

};

MIDL_INTERFACE("973810ae-9599-4b88-9e4d-6ee98c9552da")
IEnumAssocHandlers : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE Next( 
        /* [annotation][in] */ 
        ULONG celt,
        /* [annotation][length_is][size_is][out] */ 
        IAssocHandler **rgelt,
        /* [annotation][out] */ 
        ULONG *pceltFetched) = 0;

};

#endif // NTDDI_VERSION

typedef HRESULT (WINAPI *SHAssocEnumHandlers_t)(
    PCWSTR pszExtra,
    ASSOC_FILTER afFilter,
    IEnumAssocHandlers **ppEnumHandler
);

typedef int (WINAPI *Shell_GetCachedImageIndex_t)(
    PCWSTR pwszIconPath,
    int iIconIndex,
    UINT uIconFlags
);