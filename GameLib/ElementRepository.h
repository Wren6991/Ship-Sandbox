/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-03-14
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include <cassert>

/*
 * This class is a fixed-size vector holding elements in contiguous memory.
 *
 * Elements can only be emplaced back, up to the specified maximum size; after the maximum size, an exception occurs.
 */
template<typename TElement>
class ElementRepository
{
private:

    /*
     * Our iterator.
     */
    template <typename TIteratedElement>
    struct _iterator
    {
    public:

        inline bool operator==(_iterator const & other) const noexcept
        {
            return mCurrent == other.mCurrent;
        }

        inline bool operator!=(_iterator const & other) const noexcept
        {
            return !(mCurrent == other.mCurrent);
        }

        inline void operator++() noexcept
        {
            ++mCurrent;
        }

        inline TIteratedElement & operator*() noexcept
        {
            return *mCurrent;
        }

        inline TIteratedElement * operator->() noexcept
        {
            return mCurrent;
        }

    private:

        friend class ElementRepository<TElement>;

        explicit _iterator(TIteratedElement * current) noexcept
            : mCurrent(current)
        {}

        TIteratedElement * mCurrent;
    };

public:

    typedef _iterator<TElement> iterator;
    
    typedef _iterator<TElement const> const_iterator;
    
public:

    ElementRepository(size_t maxSize)
        : mCurrentSize(0u)
        , mMaxSize(maxSize)
        , mBuffer(reinterpret_cast<TElement *>(malloc(sizeof(TElement) * maxSize)))
    {
    }

    ElementRepository(ElementRepository && other)
    {
        mCurrentSize = other.mCurrentSize;
        mMaxSize = other.mMaxSize;
        mBuffer = other.mBuffer;
        other.mBuffer = nullptr;
    }

    ~ElementRepository()
    {
        if (nullptr != mBuffer)
        {
            for (size_t i = 0; i < mCurrentSize; ++i)
            {
                mBuffer[i].~TElement();
            }

            free(mBuffer);
        }
    }

    ElementRepository & operator=(ElementRepository && other)
    {
        mCurrentSize = other.mCurrentSize;
        mMaxSize = other.mMaxSize;
        mBuffer = other.mBuffer;
        other.mBuffer = nullptr;

        return *this;
    }

    // Make sure we don't introduce unnecessary copies inadvertently
    ElementRepository(ElementRepository const & other) = delete;
    ElementRepository & operator=(ElementRepository const & other) = delete;



    //
    // Visitors
    //

    inline iterator begin() noexcept
    {
        return iterator(mBuffer);
    }

    inline iterator end() noexcept
    {
        return iterator(mBuffer + mCurrentSize);
    }

    inline const_iterator begin() const noexcept
    {
        return const_iterator(mBuffer);
    }

    inline const_iterator end() const noexcept
    {
        return const_iterator(mBuffer + mCurrentSize);
    }

    inline TElement & operator[](size_t index) noexcept
    {
        assert(index < mCurrentSize);
        return mBuffer[index];
    }

    inline TElement const & operator[](size_t index) const noexcept
    {
        assert(index < mCurrentSize);
        return mBuffer[index];
    }

    inline size_t size() const noexcept
    {
        return mCurrentSize;
    }

    //
    // Modifiers
    //

    template <typename... Args>
    TElement & emplace_back(Args&&... args)
    {
        if (mCurrentSize < mMaxSize)
        {
            return *new(&(mBuffer[mCurrentSize++])) TElement(std::forward<Args>(args)...);
        }
        else
        {
            throw std::runtime_error("The repository is already full");
        }
    }


private:
    
    size_t mCurrentSize;
    size_t mMaxSize;
    TElement * mBuffer;
};
