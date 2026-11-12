#pragma once
#include "pch.h"

class CArena
{
    size_t _size;
    size_t _pos;
    BYTE *_pbData;
    CArena *_pNext;
    bool _fIsHeap;

    HRESULT _EnsureNext(size_t nextSize)
    {
        if (!_pNext)
        {
            _pNext = new CArena(nextSize + _size);
            _pNext->_fIsHeap = true;
        }
        return _pNext ? S_OK : E_OUTOFMEMORY;
    }

public:
    CArena(size_t size)
        : _size(size)
        , _pos(0)
        , _pNext(nullptr)
        , _fIsHeap(false)
    {
        _pbData = new BYTE[size];
    }

    ~CArena()
    {
        if (_fIsHeap)
            return;

        CArena *pPrev = nullptr;
        CArena *pCur = this;
        CArena *pNext = nullptr;

        // Reverse the linked list:
        while (pCur)
        {
            pNext = pCur->_pNext;
            pCur->_pNext = pPrev;
            pPrev = pCur;
            pCur = pNext;
        }

        pCur = pPrev;

        // Delete all links in the chain from the end to the beginning:
        while (pCur)
        {
            if (pCur->_pbData)
            {
                delete[] pCur->_pbData;
                pCur->_pbData = nullptr;
            }

            CArena *pNext = pCur->_pNext;

            if (pCur->_fIsHeap)
            {
                delete pCur;
            }

            pCur = pNext;
        }
    }

    void *GetData()
    {
        return (void *)_pbData;
    }

    void *GetBack()
    {
        return (void *)(_pbData + _pos);
    }

    void *Push(size_t nBytes)
    {
        if (_pos + nBytes >= _size)
        {
            if (FAILED(_EnsureNext(nBytes * 2)))
            {
                return nullptr;
            }
            return _pNext->Push(nBytes);
        }
        else
        {
            BYTE *pRet = _pbData + _pos;
            _pos += nBytes;
            return (void *)pRet;
        }
    }
};