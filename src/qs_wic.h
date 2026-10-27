#pragma once
#include <windows.h>
#include <comdef.h>
#include <ocidl.h>

typedef enum WICBitmapPaletteType
{
    WICBitmapPaletteTypeCustom = 0,
    WICBitmapPaletteTypeMedianCut = 0x1,
    WICBitmapPaletteTypeFixedBW = 0x2,
    WICBitmapPaletteTypeFixedHalftone8 = 0x3,
    WICBitmapPaletteTypeFixedHalftone27 = 0x4,
    WICBitmapPaletteTypeFixedHalftone64 = 0x5,
    WICBitmapPaletteTypeFixedHalftone125 = 0x6,
    WICBitmapPaletteTypeFixedHalftone216 = 0x7,
    WICBitmapPaletteTypeFixedWebPalette = WICBitmapPaletteTypeFixedHalftone216,
    WICBitmapPaletteTypeFixedHalftone252 = 0x8,
    WICBitmapPaletteTypeFixedHalftone256 = 0x9,
    WICBitmapPaletteTypeFixedGray4 = 0xa,
    WICBitmapPaletteTypeFixedGray16 = 0xb,
    WICBitmapPaletteTypeFixedGray256 = 0xc,
    WICBITMAPPALETTETYPE_FORCE_DWORD = 0x7fffffff
} WICBitmapPaletteType;

typedef UINT32 WICColor;

DECLARE_INTERFACE_IID_(IWICPalette, IUnknown, "00000040-a8f2-4877-ba0a-fd2b6645fb94")
{
    STDMETHOD(InitializePredefined)(
        THIS_
        WICBitmapPaletteType ePaletteType,
        BOOL fAddTransparentColor) PURE;

    STDMETHOD(InitializeCustom)(
        THIS_
        WICColor * pColors,
        UINT cCount) PURE;

    STDMETHOD(InitializeFromBitmap)(
        THIS_
        struct IWICBitmapSource * pISurface,
        UINT cCount,
        BOOL fAddTransparentColor) PURE;

    STDMETHOD(InitializeFromPalette)(
        THIS_
        IWICPalette * pIPalette) PURE;

    STDMETHOD(GetType)(
        THIS_
        WICBitmapPaletteType * pePaletteType) PURE;

    STDMETHOD(GetColorCount)(
        THIS_
        UINT * pcCount) PURE;

    STDMETHOD(GetColors)(
        THIS_
        UINT cCount,
        WICColor * pColors,
        UINT * pcActualColors) PURE;

    STDMETHOD(IsBlackWhite)(
        THIS_
        BOOL * pfIsBlackWhite) PURE;

    STDMETHOD(IsGrayscale)(
        THIS_
        BOOL * pfIsGrayscale) PURE;

    STDMETHOD(HasAlpha)(
        THIS_
        BOOL * pfHasAlpha) PURE;
};

typedef struct WICRect
{
    INT X;
    INT Y;
    INT Width;
    INT Height;
} WICRect;

DECLARE_INTERFACE_IID_(IWICBitmapSource, IUnknown, "00000120-a8f2-4877-ba0a-fd2b6645fb94")
{
    STDMETHOD(GetSize)(
        THIS_
        UINT * puiWidth,
        UINT * puiHeight) PURE;

    STDMETHOD(GetPixelFormat)(
        THIS_
        GUID * pPixelFormat) PURE;

    STDMETHOD(GetResolution)(
        THIS_
        double *pDpiX,
        double *pDpiY) PURE;

    STDMETHOD(CopyPalette)(
        THIS_
        IWICPalette * pIPalette) PURE;

    STDMETHOD(CopyPixels)(
        THIS_
        const WICRect *prc,
        UINT cbStride,
        UINT cbBufferSize,
        BYTE *pbBuffer) PURE;
};

DECLARE_INTERFACE_IID_(IWICBitmapLock, IUnknown, "00000123-a8f2-4877-ba0a-fd2b6645fb94")
{
    STDMETHOD(GetSize)(
        THIS_
        UINT * puiWidth,
        UINT * puiHeight) PURE;

    STDMETHOD(GetStride)(
        THIS_
        UINT * pcbStride) PURE;

    STDMETHOD(GetDataPointer)(
        THIS_
        UINT * pcbBufferSize,
        BYTE * *ppbData) PURE;

    STDMETHOD(GetPixelFormat)(
        THIS_
        GUID * pPixelFormat) PURE;
};

DECLARE_INTERFACE_IID_(IWICBitmap, IWICBitmapSource, "00000121-a8f2-4877-ba0a-fd2b6645fb94")
{
    STDMETHOD(Lock)(
        THIS_
        const WICRect *prcLock,
        DWORD flags,
        IWICBitmapLock **ppILock) PURE;

    STDMETHOD(SetPalette)(
        THIS_
        IWICPalette * pIPalette) PURE;

    STDMETHOD(SetResolution)(
        THIS_
        double dpiX,
        double dpiY) PURE;
};

DECLARE_INTERFACE_IID_(IWICStream, IStream, "135FF860-22B7-4ddf-B0F6-218F4F299A43")
{
    STDMETHOD(InitializeFromIStream)(
        THIS_
        IStream * pIStream) PURE;

    STDMETHOD(InitializeFromFilename)(
        THIS_
        LPCWSTR wzFileName,
        DWORD dwDesiredAccess) PURE;

    STDMETHOD(InitializeFromMemory)(
        THIS_
        BYTE * pbBuffer,
        DWORD cbBufferSize) PURE;

    STDMETHOD(InitializeFromIStreamRegion)(
        THIS_
        IStream * pIStream,
        ULARGE_INTEGER ulOffset,
        ULARGE_INTEGER ulMaxSize) PURE;
};

typedef enum WICComponentType
{
    WICDecoder = 0x1,
    WICEncoder = 0x2,
    WICPixelFormatConverter = 0x4,
    WICMetadataReader = 0x8,
    WICMetadataWriter = 0x10,
    WICPixelFormat = 0x20,
    WICAllComponents = 0x3f,
    WICCOMPONENTTYPE_FORCE_DWORD = 0x7fffffff
} WICComponentType;

DECLARE_INTERFACE_IID_(IWICComponentInfo, IUnknown, "23BC3F0A-698B-4357-886B-F24D50671334")
{
    STDMETHOD(GetComponentType)(
        THIS_
        WICComponentType * pType) PURE;

    STDMETHOD(GetCLSID)(
        THIS_
        CLSID * pclsid) PURE;

    STDMETHOD(GetSigningStatus)(
        THIS_
        DWORD * pStatus) PURE;

    STDMETHOD(GetAuthor)(
        THIS_
        UINT cchAuthor,
        WCHAR * wzAuthor,
        UINT * pcchActual) PURE;

    STDMETHOD(GetVendorGUID)(
        THIS_
        GUID * pguidVendor) PURE;

    STDMETHOD(GetVersion)(
        THIS_
        UINT cchVersion,
        WCHAR * wzVersion,
        UINT * pcchActual) PURE;

    STDMETHOD(GetSpecVersion)(
        THIS_
        UINT cchSpecVersion,
        WCHAR * wzSpecVersion,
        UINT * pcchActual) PURE;

    STDMETHOD(GetFriendlyName)(
        THIS_
        UINT cchFriendlyName,
        WCHAR * wzFriendlyName,
        UINT * pcchActual) PURE;
};

DECLARE_INTERFACE_IID_(IWICBitmapCodecInfo, IWICComponentInfo, "E87A44C4-B76E-4c47-8B09-298EB12A2714")
{
    STDMETHOD(GetContainerFormat)(
        THIS_
        GUID * pguidContainerFormat) PURE;

    STDMETHOD(GetPixelFormats)(
        THIS_
        UINT cFormats,
        GUID * pguidPixelFormats,
        UINT * pcActual) PURE;

    STDMETHOD(GetColorManagementVersion)(
        THIS_
        UINT cchColorManagementVersion,
        WCHAR * wzColorManagementVersion,
        UINT * pcchActual) PURE;

    STDMETHOD(GetDeviceManufacturer)(
        THIS_
        UINT cchDeviceManufacturer,
        WCHAR * wzDeviceManufacturer,
        UINT * pcchActual) PURE;

    STDMETHOD(GetDeviceModels)(
        THIS_
        UINT cchDeviceModels,
        WCHAR * wzDeviceModels,
        UINT * pcchActual) PURE;

    STDMETHOD(GetMimeTypes)(
        THIS_
        UINT cchMimeTypes,
        WCHAR * wzMimeTypes,
        UINT * pcchActual) PURE;

    STDMETHOD(GetFileExtensions)(
        THIS_
        UINT cchFileExtensions,
        WCHAR * wzFileExtensions,
        UINT * pcchActual) PURE;

    STDMETHOD(DoesSupportAnimation)(
        THIS_
        BOOL * pfSupportAnimation) PURE;

    STDMETHOD(DoesSupportChromakey)(
        THIS_
        BOOL * pfSupportChromakey) PURE;

    STDMETHOD(DoesSupportLossless)(
        THIS_
        BOOL * pfSupportLossless) PURE;

    STDMETHOD(DoesSupportMultiframe)(
        THIS_
        BOOL * pfSupportMultiframe) PURE;

    STDMETHOD(MatchesMimeType)(
        THIS_
        LPCWSTR wzMimeType,
        BOOL * pfMatches) PURE;
};

DECLARE_INTERFACE_IID_(IWICBitmapEncoderInfo, IWICBitmapCodecInfo, "94C9B4EE-A09F-4f92-8A1E-4A9BCE7E76FB")
{
    STDMETHOD(CreateInstance)(
        THIS_
        struct IWICBitmapEncoder * *ppIBitmapEncoder) PURE;
};

typedef enum WICColorContextType
{
    WICColorContextUninitialized = 0,
    WICColorContextProfile = 0x1,
    WICColorContextExifColorSpace = 0x2
} WICColorContextType;

DECLARE_INTERFACE_IID_(IWICColorContext, IUnknown, "3C613A02-34B2-44ea-9A7C-45AEA9C6FD6D")
{
    STDMETHOD(InitializeFromFilename)(
        THIS_
        LPCWSTR wzFilename) PURE;

    STDMETHOD(InitializeFromMemory)(
        THIS_
        const BYTE *pbBuffer,
        UINT cbBufferSize) PURE;

    STDMETHOD(InitializeFromExifColorSpace)(
        THIS_
        UINT value) PURE;

    STDMETHOD(GetType)(
        THIS_
        WICColorContextType * pType) PURE;

    STDMETHOD(GetProfileBytes)(
        THIS_
        UINT cbBuffer,
        BYTE * pbBuffer,
        UINT * pcbActual) PURE;

    STDMETHOD(GetExifColorSpace)(
        THIS_
        UINT * pValue) PURE;
};

DECLARE_INTERFACE_IID_(IWICMetadataQueryReader, IUnknown, "30989668-E1C9-4597-B395-458EEDB808DF")
{
    STDMETHOD(GetContainerFormat)(
        THIS_
        GUID * pguidContainerFormat) PURE;

    STDMETHOD(GetLocation)(
        THIS_
        UINT cchMaxLength,
        WCHAR * wzNamespace,
        UINT * pcchActualLength) PURE;

    STDMETHOD(GetMetadataByName)(
        THIS_
        LPCWSTR wzName,
        PROPVARIANT * pvarValue) PURE;

    STDMETHOD(GetEnumerator)(
        THIS_
        IEnumString * *ppIEnumString) PURE;
};

DECLARE_INTERFACE_IID_(IWICMetadataQueryWriter, IWICMetadataQueryReader, "A721791A-0DEF-4d06-BD91-2118BF1DB10B")
{
    STDMETHOD(SetMetadataByName)(
        THIS_
        LPCWSTR wzName,
        const PROPVARIANT *pvarValue) PURE;

    STDMETHOD(RemoveMetadataByName)(
        THIS_
        LPCWSTR wzName) PURE;
};

DECLARE_INTERFACE_IID_(IWICBitmapFrameEncode, IUnknown, "00000105-a8f2-4877-ba0a-fd2b6645fb94")
{
    STDMETHOD(Initialize)(
        THIS_
        IPropertyBag2 * pIEncoderOptions) PURE;

    STDMETHOD(SetSize)(
        THIS_
        UINT uiWidth,
        UINT uiHeight) PURE;

    STDMETHOD(SetResolution)(
        THIS_
        double dpiX,
        double dpiY) PURE;

    STDMETHOD(SetPixelFormat)(
        THIS_
        GUID * pPixelFormat) PURE;

    STDMETHOD(SetColorContexts)(
        THIS_
        UINT cCount,
        IWICColorContext * *ppIColorContext) PURE;

    STDMETHOD(SetPalette)(
        THIS_
        IWICPalette * pIPalette) PURE;

    STDMETHOD(SetThumbnail)(
        THIS_
        IWICBitmapSource * pIThumbnail) PURE;

    STDMETHOD(WritePixels)(
        THIS_
        UINT lineCount,
        UINT cbStride,
        UINT cbBufferSize,
        BYTE * pbPixels) PURE;

    STDMETHOD(WriteSource)(
        THIS_
        IWICBitmapSource * pIBitmapSource,
        WICRect * prc) PURE;

    STDMETHOD(Commit)(THIS) PURE;

    STDMETHOD(GetMetadataQueryWriter)(
        THIS_
        IWICMetadataQueryWriter * *ppIMetadataQueryWriter) PURE;
};

typedef enum WICBitmapEncoderCacheOption
{
    WICBitmapEncoderCacheInMemory = 0,
    WICBitmapEncoderCacheTempFile = 0x1,
    WICBitmapEncoderNoCache = 0x2,
    WICBITMAPENCODERCACHEOPTION_FORCE_DWORD = 0x7fffffff
} WICBitmapEncoderCacheOption;

DECLARE_INTERFACE_IID_(IWICBitmapEncoder, IUnknown, "00000103-a8f2-4877-ba0a-fd2b6645fb94")
{
    STDMETHOD(Initialize)(
        THIS_
        IStream * pIStream,
        WICBitmapEncoderCacheOption cacheOption) PURE;

    STDMETHOD(GetContainerFormat)(
        THIS_
        GUID * pguidContainerFormat) PURE;

    STDMETHOD(GetEncoderInfo)(
        THIS_
        IWICBitmapEncoderInfo * *ppIEncoderInfo) PURE;

    STDMETHOD(SetColorContexts)(
        THIS_
        UINT cCount,
        IWICColorContext * *ppIColorContext) PURE;

    STDMETHOD(SetPalette)(
        THIS_
        IWICPalette * pIPalette) PURE;

    STDMETHOD(SetThumbnail)(
        THIS_
        IWICBitmapSource * pIThumbnail) PURE;

    STDMETHOD(SetPreview)(
        THIS_
        IWICBitmapSource * pIPreview) PURE;

    STDMETHOD(CreateNewFrame)(
        THIS_
        IWICBitmapFrameEncode * *ppIFrameEncode,
        IPropertyBag2 * *ppIEncoderOptions) PURE;

    STDMETHOD(Commit)(THIS) PURE;

    STDMETHOD(GetMetadataQueryWriter)(
        THIS_
        IWICMetadataQueryWriter * *ppIMetadataQueryWriter) PURE;
};

typedef enum WICDecodeOptions
{
    WICDecodeMetadataCacheOnDemand = 0,
    WICDecodeMetadataCacheOnLoad = 0x1,
    WICMETADATACACHEOPTION_FORCE_DWORD = 0x7fffffff
} WICDecodeOptions;

typedef struct WICBitmapPattern
{
    ULARGE_INTEGER Position;
    ULONG Length;
    BYTE *Pattern;
    BYTE *Mask;
    BOOL EndOfStream;
} WICBitmapPattern;

DECLARE_INTERFACE_IID_(IWICBitmapDecoderInfo, IWICBitmapCodecInfo, "D8CD007F-D08F-4191-9BFC-236EA7F0E4B5")
{
    STDMETHOD(GetPatterns)(
        THIS_
        UINT cbSizePatterns,
        WICBitmapPattern * pPatterns,
        UINT * pcPatterns,
        UINT * pcbPatternsActual) PURE;

    STDMETHOD(MatchesPattern)(
        THIS_
        IStream * pIStream,
        BOOL * pfMatches) PURE;

    STDMETHOD(CreateInstance)(
        THIS_
        struct IWICBitmapDecoder * *ppIBitmapDecoder) PURE;
};

DECLARE_INTERFACE_IID_(IWICBitmapFrameDecode, IWICBitmapSource, "3B16811B-6A43-4ec9-A813-3D930C13B940")
{
    STDMETHOD(GetMetadataQueryReader)(
        THIS_
        IWICMetadataQueryReader * *ppIMetadataQueryReader) PURE;

    STDMETHOD(GetColorContexts)(
        THIS_
        UINT cCount,
        IWICColorContext * *ppIColorContexts,
        UINT * pcActualCount) PURE;

    STDMETHOD(GetThumbnail)(
        THIS_
        IWICBitmapSource * *ppIThumbnail) PURE;
};

DECLARE_INTERFACE_IID_(IWICBitmapDecoder, IUnknown, "9EDDE9E7-8DEE-47ea-99DF-E6FAF2ED44BF")
{
    STDMETHOD(QueryCapability)(
        THIS_
        IStream * pIStream,
        DWORD * pdwCapability) PURE;

    STDMETHOD(Initialize)(
        THIS_
        IStream * pIStream,
        WICDecodeOptions cacheOptions) PURE;

    STDMETHOD(GetContainerFormat)(
        THIS_
        GUID * pguidContainerFormat) PURE;

    STDMETHOD(GetDecoderInfo)(
        THIS_
        IWICBitmapDecoderInfo * *ppIDecoderInfo) PURE;

    STDMETHOD(CopyPalette)(
        THIS_
        IWICPalette * pIPalette) PURE;

    STDMETHOD(GetMetadataQueryReader)(
        THIS_
        IWICMetadataQueryReader * *ppIMetadataQueryReader) PURE;

    STDMETHOD(GetPreview)(
        THIS_
        IWICBitmapSource * *ppIBitmapSource) PURE;

    STDMETHOD(GetColorContexts)(
        THIS_
        UINT cCount,
        IWICColorContext * *ppIColorContexts,
        UINT * pcActualCount) PURE;

    STDMETHOD(GetThumbnail)(
        THIS_
        IWICBitmapSource * *ppIThumbnail) PURE;

    STDMETHOD(GetFrameCount)(
        THIS_
        UINT * pCount) PURE;

    STDMETHOD(GetFrame)(
        THIS_
        UINT index,
        IWICBitmapFrameDecode * *ppIBitmapFrame) PURE;
};

typedef enum WICBitmapDitherType
{
    WICBitmapDitherTypeNone = 0,
    WICBitmapDitherTypeSolid = 0,
    WICBitmapDitherTypeOrdered4x4 = 0x1,
    WICBitmapDitherTypeOrdered8x8 = 0x2,
    WICBitmapDitherTypeOrdered16x16 = 0x3,
    WICBitmapDitherTypeSpiral4x4 = 0x4,
    WICBitmapDitherTypeSpiral8x8 = 0x5,
    WICBitmapDitherTypeDualSpiral4x4 = 0x6,
    WICBitmapDitherTypeDualSpiral8x8 = 0x7,
    WICBitmapDitherTypeErrorDiffusion = 0x8,
    WICBITMAPDITHERTYPE_FORCE_DWORD = 0x7fffffff
} WICBitmapDitherType;

DECLARE_INTERFACE_IID_(IWICFormatConverter, IWICBitmapSource, "00000301-a8f2-4877-ba0a-fd2b6645fb94")
{
    STDMETHOD(Initialize)(
        THIS_
        IWICBitmapSource * pISource,
        REFIID dstFormat,
        WICBitmapDitherType dither,
        IWICPalette * pIPalette,
        double alphaThresholdPercent,
        WICBitmapPaletteType paletteTranslate) PURE;

    STDMETHOD(CanConvert)(
        THIS_
        REFIID srcPixelFormat,
        REFIID dstPixelFormat,
        BOOL * pfCanConvert) PURE;
};

typedef enum WICBitmapInterpolationMode
{
    WICBitmapInterpolationModeNearestNeighbor = 0,
    WICBitmapInterpolationModeLinear = 0x1,
    WICBitmapInterpolationModeCubic = 0x2,
    WICBitmapInterpolationModeFant = 0x3,
    WICBITMAPINTERPOLATIONMODE_FORCE_DWORD = 0x7fffffff
} WICBitmapInterpolationMode;

DECLARE_INTERFACE_IID_(IWICBitmapScaler, IWICBitmapSource, "00000302-a8f2-4877-ba0a-fd2b6645fb94")
{
    STDMETHOD(Initialize)(
        THIS_
        IWICBitmapSource * pISource,
        UINT uiWidth,
        UINT uiHeight,
        WICBitmapInterpolationMode mode) PURE;
};

DECLARE_INTERFACE_IID_(IWICBitmapClipper, IWICBitmapSource, "E4FBCF03-223D-4e81-9333-D635556DD1B5")
{
    STDMETHOD(Initialize)(
        THIS_
        IWICBitmapSource * pISource,
        const WICRect *prc) PURE;
};

typedef enum WICBitmapTransformOptions
{
    WICBitmapTransformRotate0 = 0,
    WICBitmapTransformRotate90 = 0x1,
    WICBitmapTransformRotate180 = 0x2,
    WICBitmapTransformRotate270 = 0x3,
    WICBitmapTransformFlipHorizontal = 0x8,
    WICBitmapTransformFlipVertical = 0x10,
    WICBITMAPTRANSFORMOPTIONS_FORCE_DWORD = 0x7fffffff
} WICBitmapTransformOptions;

DECLARE_INTERFACE_IID_(IWICBitmapFlipRotator, IWICBitmapSource, "5009834F-2D6A-41ce-9E1B-17C5AFF7A782")
{
    STDMETHOD(Initialize)(
        THIS_
        IWICBitmapSource * pISource,
        WICBitmapTransformOptions options) PURE;
};

DECLARE_INTERFACE_IID_(IWICColorTransform, IWICBitmapSource, "B66F034F-D0E2-40ab-B436-6DE39E321A94")
{
    STDMETHOD(Initialize)(
        THIS_
        IWICBitmapSource * pIBitmapSource,
        IWICColorContext * pIContextSource,
        IWICColorContext * pIContextDest,
        REFIID pixelFmtDest) PURE;
};

typedef enum WICBitmapCreateCacheOption
{
    WICBitmapNoCache = 0,
    WICBitmapCacheOnDemand = 0x1,
    WICBitmapCacheOnLoad = 0x2,
    WICBITMAPCREATECACHEOPTION_FORCE_DWORD = 0x7fffffff
} WICBitmapCreateCacheOption;

typedef enum WICBitmapAlphaChannelOption
{
    WICBitmapUseAlpha = 0,
    WICBitmapUsePremultipliedAlpha = 0x1,
    WICBitmapIgnoreAlpha = 0x2,
    WICBITMAPALPHACHANNELOPTIONS_FORCE_DWORD = 0x7fffffff
} WICBitmapAlphaChannelOption;

DECLARE_INTERFACE_IID_(IWICFastMetadataEncoder, IUnknown, "B84E2C09-78C9-4AC4-8BD3-524AE1663A2F")
{
    STDMETHOD(Commit)(THIS) PURE;

    STDMETHOD(GetMetadataQueryWriter)(
        THIS_
        IWICMetadataQueryWriter * *ppIMetadataQueryWriter) PURE;
};

DECLARE_INTERFACE_IID_(IWICImagingFactory, IUnknown, "ec5ec8a9-c395-4314-9c77-54d7a935ff70")
{
    STDMETHOD(CreateDecoderFromFilename)(
        THIS_
        LPCWSTR wzFilename,
        const GUID *pguidVendor,
        DWORD dwDesiredAccess,
        WICDecodeOptions metadataOptions,
        IWICBitmapDecoder **ppIDecoder) PURE;

    STDMETHOD(CreateDecoderFromStream)(
        THIS_
        IStream * pIStream,
        const GUID *pguidVendor,
        WICDecodeOptions metadataOptions,
        IWICBitmapDecoder **ppIDecoder) PURE;

    STDMETHOD(CreateDecoderFromFileHandle)(
        THIS_
        ULONG_PTR hFile,
        const GUID *pguidVendor,
        WICDecodeOptions metadataOptions,
        IWICBitmapDecoder **ppIDecoder) PURE;

    STDMETHOD(CreateComponentInfo)(
        THIS_
        REFCLSID clsidComponent,
        IWICComponentInfo * *ppIInfo) PURE;

    STDMETHOD(CreateDecoder)(
        THIS_
        REFGUID guidContainerFormat,
        const GUID *pguidVendor,
        IWICBitmapDecoder **ppIDecoder) PURE;

    STDMETHOD(CreateEncoder)(
        THIS_
        REFGUID guidContainerFormat,
        const GUID *pguidVendor,
        IWICBitmapEncoder **ppIEncoder) PURE;

    STDMETHOD(CreatePalette)(
        THIS_
        IWICPalette * *ppIPalette) PURE;

    STDMETHOD(CreateFormatConverter)(
        THIS_
        IWICFormatConverter * *ppIFormatConverter) PURE;

    STDMETHOD(CreateBitmapScaler)(
        THIS_
        IWICBitmapScaler * *ppIBitmapScaler) PURE;

    STDMETHOD(CreateBitmapClipper)(
        THIS_
        IWICBitmapClipper * *ppIBitmapClipper) PURE;

    STDMETHOD(CreateBitmapFlipRotator)(
        THIS_
        IWICBitmapFlipRotator * *ppIBitmapFlipRotator) PURE;

    STDMETHOD(CreateStream)(
        THIS_
        IWICStream * *ppIWICStream) PURE;

    STDMETHOD(CreateColorContext)(
        THIS_
        IWICColorContext * *ppIWICColorContext) PURE;

    STDMETHOD(CreateColorTransformer)(
        THIS_
        IWICColorTransform * *ppIWICColorTransform) PURE;

    STDMETHOD(CreateBitmap)(
        THIS_
        UINT uiWidth,
        UINT uiHeight,
        REFIID pixelFormat,
        WICBitmapCreateCacheOption option,
        IWICBitmap * *ppIBitmap) PURE;

    STDMETHOD(CreateBitmapFromSource)(
        THIS_
        IWICBitmapSource * pIBitmapSource,
        WICBitmapCreateCacheOption option,
        IWICBitmap * *ppIBitmap) PURE;

    STDMETHOD(CreateBitmapFromSourceRect)(
        THIS_
        IWICBitmapSource * pIBitmapSource,
        UINT x,
        UINT y,
        UINT width,
        UINT height,
        IWICBitmap * *ppIBitmap) PURE;

    STDMETHOD(CreateBitmapFromMemory)(
        THIS_
        UINT uiWidth,
        UINT uiHeight,
        REFIID pixelFormat,
        UINT cbStride,
        UINT cbBufferSize,
        BYTE * pbBuffer,
        IWICBitmap * *ppIBitmap) PURE;

    STDMETHOD(CreateBitmapFromHBITMAP)(
        THIS_
        HBITMAP hBitmap,
        HPALETTE hPalette,
        WICBitmapAlphaChannelOption options,
        IWICBitmap * *ppIBitmap) PURE;

    STDMETHOD(CreateBitmapFromHICON)(
        THIS_
        HICON hIcon,
        IWICBitmap * *ppIBitmap) PURE;

    STDMETHOD(CreateComponentEnumerator)(
        THIS_
        DWORD componentTypes,
        DWORD options,
        IEnumUnknown * *ppIEnumUnknown) PURE;

    STDMETHOD(CreateFastMetadataEncoderFromDecoder)(
        THIS_
        IWICBitmapDecoder * pIDecoder,
        IWICFastMetadataEncoder * *ppIFastEncoder) PURE;

    STDMETHOD(CreateFastMetadataEncoderFromFrameDecode)(
        THIS_
        IWICBitmapFrameDecode * pIFrameDecoder,
        IWICFastMetadataEncoder * *ppIFastEncoder) PURE;

    STDMETHOD(CreateQueryWriter)(
        THIS_
        REFGUID guidMetadataFormat,
        const GUID *pguidVendor,
        IWICMetadataQueryWriter **ppIQueryWriter) PURE;

    STDMETHOD(CreateQueryWriterFromReader)(
        THIS_
        IWICMetadataQueryReader * pIQueryReader,
        const GUID *pguidVendor,
        IWICMetadataQueryWriter **ppIQueryWriter) PURE;
};

typedef enum WICComponentEnumerateOptions
{
    WICComponentEnumerateDefault = 0,
    WICComponentEnumerateRefresh = 0x1,
    WICComponentEnumerateDisabled = 0x80000000,
    WICComponentEnumerateUnsigned = 0x40000000,
    WICComponentEnumerateBuiltInOnly = 0x20000000,
    WICCOMPONENTENUMERATEOPTIONS_FORCE_DWORD = 0x7fffffff
} WICComponentEnumerateOptions;

DEFINE_GUID(CLSID_WICImagingFactory, 0xcacaf262, 0x9370, 0x4615, 0xa1, 0x3b, 0x9f, 0x55, 0x39, 0xda, 0x4c, 0xa);