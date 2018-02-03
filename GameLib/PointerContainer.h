/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-01-19
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include <cassert>
#include <cstring>
#include <vector>

/*
 * This class is a container of pointers to arbitrary types. The container *owns* the pointers
 * and thus frees them each time an element is removed from the container, or when the
 * container itself is deleted.
 *
 * The container is optimized for fast *visit*, so that it can be used to iterate through
 * all elements. 
 *
 * Removing individual elements from the container is achieved by means of setting an "IsDeleted" flag
 * on the element(s) to be removed, by calling the container's register_deletion() method, and then 
 * by invoking the container's shrink_to_fit() method, which will scour elements flagged as deleted, 
 * delete them, and shrink the container.
 */
template<typename TElement>
class PointerContainer
{
private:

	/*
	 * Our iterator.
	 */
	template <
		typename TIteratedPointer, 
		typename TIteratedReference,
		typename TIteratedPointerPointer>
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

		inline TIteratedReference operator*() noexcept
		{
			return *(*mCurrent);
		}

		inline TIteratedPointer operator->() noexcept
		{
			return *mCurrent;
		}

	private:

		friend class PointerContainer<TElement>;

		explicit _iterator(TIteratedPointerPointer current) noexcept
			: mCurrent(current)
		{}

		TIteratedPointerPointer mCurrent;
	};

public:

	typedef _iterator<TElement *, TElement &, TElement **> iterator;
	
	typedef _iterator<TElement const *, TElement const &, TElement const * const *> const_iterator;
	
public:

	explicit PointerContainer(std::vector<TElement *> && pointers)
		: mDeletedElementsCount(0u)
	{
		// Allocate array of pointers and copy all pointers
		mPointers = new TElement*[pointers.size()];
		memcpy(mPointers, pointers.data(), pointers.size() * sizeof(TElement *));
		mSize = pointers.size();
		
		// Empty the moved container
		pointers.clear();
	}

	// No copy!
	PointerContainer(PointerContainer const & other) = delete;
	PointerContainer & operator=(PointerContainer const & other) = delete;

	~PointerContainer()
	{
		// Delete all pointers we own
		for (size_t i = 0; i < mSize; ++i)
		{
			delete mPointers[i];
		}

		// ...and now delete the array of pointers
		delete[] mPointers;
	}

	//
	// Visitors
	//

	inline iterator begin() noexcept
	{
		return iterator(mPointers);
	}

	inline iterator end() noexcept
	{
		return iterator(mPointers + mSize);
	}

	inline const_iterator begin() const noexcept
	{
		return const_iterator(mPointers);
	}

	inline const_iterator end() const noexcept
	{
		return const_iterator(mPointers + mSize);
	}

	inline TElement * operator[](size_t index) noexcept
	{
		assert(index < mSize);
		return mPointers[index];
	}

	inline TElement const * operator[](size_t index) const noexcept
	{
		assert(index < mSize);
		return mPointers[index];
	}

	inline size_t size() const noexcept
	{
		return mSize;
	}

	//
	// Modifiers
	//

	inline void register_deletion() noexcept
	{
		++mDeletedElementsCount;
	}

	/*
	 * Scavenges deleted elements (those flagged with IsDeleted=true),
	 * frees them, and removes them from the container.
	 *
	 * After this call, the new size of the container will be equal to the
	 * old size minus the number of elements removed.
	 */
	void shrink_to_fit()
	{
		if (0u == mDeletedElementsCount)
		{
			// Nothing to do!
			return;
		}

		// Allocate new array
		size_t newSize = mSize - mDeletedElementsCount;		
		assert(newSize < mSize);
		TElement **pNewPointers = new TElement*[newSize];

		// Copy non-deleted items
		TElement ** pEnd = mPointers + mSize;
		TElement ** pNewPosition = pNewPointers;
		for (TElement ** p1 = mPointers; p1 < pEnd; ++p1)
		{
			if ((*p1)->IsDeleted())
			{
				// Free it
				delete *p1;
			}
			else
			{
				// Copy it
				*(pNewPosition++) = *p1;
			}
		}

		assert(pNewPointers + newSize == pNewPosition);

		// Swap pointers array
		delete[] mPointers;
		mPointers = pNewPointers;
		mSize = newSize;

		// Reset deletion count
		mDeletedElementsCount = 0u;
	}

private:

	// The actual array of pointers
	TElement * * mPointers;

	// The size of the array
	size_t mSize;

	// The current number of deleted elements
	size_t mDeletedElementsCount;
};
