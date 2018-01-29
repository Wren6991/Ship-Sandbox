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
 * on the element(s) to be removed and then invoking the container's ShrinkToFit() method. Removal of
 * elements is not optimized.
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
	{
		// Allocate array of pointers and copy all pointers
		mPointers = new TElement*[pointers.size()];
		memcpy(mPointers, pointers.data(), pointers.size() * sizeof(TElement *));
		mSize = pointers.size();

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

	TElement * operator[](size_t index) noexcept
	{
		assert(index < mSize);
		return mPointers[index];
	}

	TElement const * operator[](size_t index) const noexcept
	{
		assert(index < mSize);
		return mPointers[index];
	}

	size_t size() const noexcept
	{
		return mSize;
	}

	//
	// Modifiers
	//

	/*
	 * Scavenges deleted elements (those flagged with IsDeleted=true),
	 * frees them, and removes them from the container.
	 *
	 * After this call, the new size of the container will be equal to the
	 * old size minus the number of elements removed.
	 */
	void shrink_to_fit()
	{
		//
		// The idea is to minimize the number of memmove's by identifying strides
		// of deleted elements and then memmove'ing towards the left everything 
		// after each deleted stride.
		//
		// As a further optimization, we could also identify strides
		// of non-deleted elements afterwards and memmove just these; this would
		// guarantee that each element is only memmove'd once. For later.
		//

		//
		// 1. Compact array
		//

		size_t const mAllocatedSize = mSize;

		TElement ** pEnd = mPointers + mSize;

		for (TElement ** p1 = mPointers; p1 < pEnd; /*incremented in loop*/)
		{
			if ((*p1)->IsDeleted)
			{
				// Free this pointer
				delete *p1;

				// Find first pointer of non-deleted element, deleting elements
				// in the meantime
				TElement ** p2;
				for (
					p2 = p1 + 1; 
					p2 < pEnd && (*p2)->IsDeleted; 
					delete *p2, ++p2);

				// Move back everything from p2 onwards
				std::memmove(p1, p2, (pEnd - p2) * sizeof(TElement *));

				// Adjust size 
				mSize -= (p2 - p1);
				pEnd -= (p2 - p1);

				// Keep going from here
			}
			else
			{
				// Skip
				++p1;
			}
		}
		


		//
		// 2. Shrink array (if needed)
		//

		if (mAllocatedSize != mSize)
		{
			TElement **pNewPointers = new TElement*[mSize];
			memcpy(pNewPointers, mPointers, mSize * sizeof(TElement *));
			delete[] mPointers;
			mPointers = pNewPointers;
		}
	}

private:

	// The actual array of pointers
	TElement * * mPointers;

	// The size of the array
	size_t mSize;
};
