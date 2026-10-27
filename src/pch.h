#pragma once

#define NOMINMAX
#include <windows.h>
#include <unknwn.h>
#include <objbase.h>
#include <tchar.h>
#include "QuiKShot.h"
#include "portable.h"
#include "portablep.h"
#include "ccomptr.h"

#define QS_SHOW_CONSOLE

#define QS_APP_NAME         TEXT("QuiKShot")
#define QS_VER_STRING       TEXT("ver. alpha 1.0")
#define QS_APP_FULL_BRAND   QS_APP_NAME TEXT(" ") QS_VER_STRING

#define TOK_CAT_INNER(a, b) a##b
#define TOK_CAT(a, b) TOK_CAT_INNER(a, b)

#if defined(_DEBUG) || defined(QS_SHOW_CONSOLE)
#define DBGPRINT(...)                                                                    \
    (                                                                                    \
        (_tprintf(TEXT("[") TEXT(__FUNCTION__) TEXT("] "))),                                   \
        (_tprintf(__VA_ARGS__)),                                                         \
        (_tprintf(TEXT("\n")))                                                           \
    )
#else
#define DBGPRINT(...)
#endif

#define PRINT_GUID_PATTERN TEXT("{%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}")
#define PRINT_GUID_PARAMS(guid)                                                          \
    (guid).Data1, (guid).Data2, (guid).Data3,                                            \
    (guid).Data4[0], (guid).Data4[1], (guid).Data4[2], (guid).Data4[3],                  \
    (guid).Data4[4], (guid).Data4[5], (guid).Data4[6], (guid).Data4[7]                   \

#ifdef _DEBUG
#define ASSERT_KEEP(expr) assert(expr)
#else
#define ASSERT_KEEP(expr) (expr)
#endif

// util.h
extern void LogIfFailedHelper(const TCHAR *pszExpr, HRESULT hr, const TCHAR *pszFile, const int iLine);

#define LOG_IF_FAILED(hr_expr)                                                           \
    do                                                                                   \
    {                                                                                    \
        HRESULT __hr = hr_expr;                                                          \
        if (FAILED(__hr))                                                                \
        {                                                                                \
            LogIfFailedHelper(                                                           \
                TEXT(#hr_expr),                                                          \
                __hr,                                                                    \
                TEXT(__FILE__),                                                          \
                __LINE__                                                                 \
            );                                                                           \
        }                                                                                \
    }                                                                                    \
    while (0);

#define RECTWIDTH(rc) ((rc).right - (rc).left)
#define RECTHEIGHT(rc) ((rc).bottom - (rc).top)

#define DEFINE_WINDOW_CLASS(cls) public: static const TCHAR *GetWindowClass() { return TEXT(cls); }

#define ID_HOTKEY_SCREENSHOT (20)

#define IMPLEMENT_IUNKNOWN                                                               \
protected:                                                                               \
    ULONG _uRefCount;                                                                    \
public:                                                                                  \
    STDMETHODIMP QueryInterface(const IID &riid, void **ppvOut) override;                \
    STDMETHODIMP_(ULONG) AddRef() override                                               \
    {                                                                                    \
        _uRefCount++;                                                                    \
        return _uRefCount;                                                               \
    }                                                                                    \
    STDMETHODIMP_(ULONG) Release() override                                              \
    {                                                                                    \
        _uRefCount--;                                                                    \
        if (_uRefCount == 0)                                                             \
        {                                                                                \
            delete this;                                                                 \
            return 0;                                                                    \
        }                                                                                \
        return _uRefCount;                                                               \
    }

extern HINSTANCE g_hinst;
