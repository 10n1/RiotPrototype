/*********************************************************\
File:       vector.h
Purpose:    STL vector replacement
\*********************************************************/
#ifndef _VECTOR_H_
#define _VECTOR_H_
#include "RiotTypes.h"
#include <stdlib.h>

#ifdef _DEBUG
#include <assert.h>
// Using _ASSERT because its defined to nothing in release
#endif

#define fMultiplier 1.5f // Size to increase array by when resizing

template<class T>
class vector
{
private:
    T*      m_pData;
    uint    m_nSize;
    uint    m_nCapacity;

    const static uint nInitialSize = 16;
public:
    vector() 
        : m_pData(NULL)
        , m_nSize(0)
        , m_nCapacity(0)
    {
        reserve(nInitialSize);
    }
    vector(vector& ref)
    {
        m_nSize = ref.m_nSize;
        m_nCapacity = ref.m_nCapacity;
        m_pData = (T*)malloc(m_nCapacity * sizeof(T));
        memcpy(m_pData, ref.m_pData, sizeof(T) * m_nSize);
    }

    vector& operator=(vector& ref)
    {
        m_nSize = ref.m_nSize;
        m_nCapacity = ref.m_nCapacity;
        m_pData = (T*)malloc(m_nCapacity * sizeof(T));
        memcpy(m_pData, ref.m_pData, sizeof(T) * m_nSize);

        return *this;
    }

    ~vector()
    {
        if(m_pData)
        {
            free(m_pData);
        }
        m_pData = NULL;
    }

    // return reference to back element of vector
    _inline T& back()
    {
        return m_pData[m_nSize - 1];
    }

    // return the capacity of the vector
    _inline uint capacity()
    {
        return m_nCapacity;
    }

    // clear out the vector
    // bReset = false to keep memory allocated
    _inline void clear(const bool bReset = true)
    {
        m_nSize = 0;
        if(bReset)
        {
            m_pData = (T*)realloc(m_pData, sizeof(T)*nInitialSize);
            // TODO: Add error handling for m_pData == NULL
            m_nCapacity = nInitialSize;
        }
    }

    // tests if the vector is empty
    _inline bool empty()
    {
        return (m_nSize == 0);
    }

    // erases an element at the specified index
    _inline void erase(const uint nIndex)
    {
        _ASSERT(nIndex < m_nSize);
        for(uint i = nIndex; i < m_nSize-1; ++i)
        {
            m_pData[i] = m_pData[i+1];
        }
        --m_nSize;
    }

    // return reference to back element of vector
    _inline T& front()
    {
        if(m_nSize > 0)
            return m_pData[0];

        return *(T*)(void*);
    }

    // inserts an element into the array
    _inline void insert(const uint nIndex, T& data)
    {
        _ASSERT(nIndex < m_nSize);

        if(m_nSize >= m_nCapacity)
        {
            resize( (uint)((float32)m_nCapacity * fMultiplier) );
        }

        for(uint i = m_nSize++; i > nIndex; --i)
        {
            m_pData[i] = m_pData[i-1];
        }

        m_pData[nIndex] = data;
    }

    // DANGEROUS
    // Will quick erase by swaping the nIndex with the end,
    // then just decreasing the size of the array
    // Will NOT maintain the order, don't use when order matters
    _inline void quick_erase(const uint nIndex)
    {
        _ASSERT(nIndex < m_nSize);
        m_pData[nIndex] = m_pData[m_nSize-1];
        m_nSize--;
    }

    // pop the back off and return it
    // does not free the memory
    _inline T pop_back(void)
    {
        return m_pData[--m_nSize];
    }

    // add a new item to the vector
    _inline void push_back(const T& data)
    {
        if(m_nSize >= m_nCapacity)
        {
            resize( (uint)((float32)m_nCapacity * fMultiplier) );
        }

        m_pData[m_nSize++] = data;
    }

    // returns a pointer to the array
    _inline void* ptr(void)
    {
        return m_pData;
    }

    // reserves the memory in advance
    _inline void reserve(const uint nElements)
    {
        if(nElements < m_nCapacity)
            return;

        m_nCapacity = nElements;
        m_pData = (T*)realloc(m_pData, sizeof(T)*m_nCapacity);
        // TODO: Add error handling for m_pData == NULL
    }

    // resizes the array, allocating memory as necessary
    _inline void resize(const uint nSize)
    {
        // TODO: Should there be this many safety checks?
        if(nSize < m_nSize)
        {
            m_nSize = nSize;
            return;
        }

        if(nSize == m_nSize)
            return;

        if(nSize <= m_nCapacity)
        {
            m_nSize = nSize;
            return;
        }

        m_nCapacity = nSize;
        m_pData = (T*)realloc(m_pData, sizeof(T)*m_nSize);
        // TODO: Add error handling for m_pData == NULL
        m_nSize = nSize;
    }

    // returns the size of the array
    _inline uint size()
    {
        return m_nSize;
    }

    // swaps two elements in the array
    // not safe
    _inline void swap(const uint nLeft, const uint nRight)
    {
        _ASSERT(nIndex < m_nSize);
        T temp;
        temp = m_pData[nLeft];
        m_pData[nLeft] = m_pData[nRight];
        m_pData[nRight] = temp;        
    }

    _inline T& operator[](const uint nIndex)
    {
        _ASSERT(nIndex < m_nSize);

        return m_pData[nIndex];
    }

};

#undef fMultiplier

#endif // #ifndef _VECTOR_H_
