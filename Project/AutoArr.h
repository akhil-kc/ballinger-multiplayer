/*
(C) 2004 - 2008 Thomas Bleeker
MadWizard.org

Released under the BSD license with the exception of parts of the code
not written by Thomas Bleeker (www.madwizard.org), which are marked in
the source code. See license.txt for details.
*/

#ifndef _CG_AUTOARR_H_
#define _CG_AUTOARR_H_

#include "DisallowCopy.h"
#include "DataFile.h"

// Maximum array element count (safety value to prevent
// large array allocation).
const int AUTOARR_MAX_ELEMENT_COUNT = 1000 * 1000 * 5;

/* Template for an array of a given type, which will
automatically deallocate the memory for the array
when it is destroyed. */
template<typename T>
class AutoArr : private DisallowCopy
{
public:

	// Creates a new AutoArr, with no data attached
	AutoArr()
	{
		_ptr = NULL;
		_count = 0;
	}

	// Creates a new AutoArr with memory for 'size' elements
	AutoArr(int size)
	{
		_ptr = NULL;
		if (size < 0) size = 0;
		alloc(size);
	}

	// Allocates new memory for 'size' elements. All data 
	// possibly stored previously is deallocated and lost. 
	void alloc(int size)
	{
		if (size < 0) size = 0;
		delete[] _ptr;
		_ptr = NULL;
		_count = 0;
		_ptr = new T[size];
		_count = size;
	}

	// Destroys the AutoArr, including the allocated memory
	~AutoArr(void)
	{
		delete[] _ptr;
	}

	// Cast operator to get a pointer to the elements
	T * operator()() { return _ptr; }

	// Const index operator 	 
	const T & operator[] (int i) const { return _ptr[i]; }
	// Index operator

	T & operator[](int i) { return _ptr[i]; }

	// Returns the number of elements in the array
	int size() const { return _count; }

	// Returns a pointer to the array
	T * get() { return _ptr; }

	// Returns a pointer to the const array
	const T * get() const { return _ptr; }

	// (Un)serializes the array
	void exchange(DataFile *pFile)
	{
		// Write or get the number of elements
		int elements;
		pFile->exchange(&elements);

		if (pFile->writing())
		{
			// Write the number of elements
			int elements = _count;
			pFile->exchange(&elements);

			// Write all elements
			pFile->exchange(reinterpret_cast<char*>(_ptr), elements * sizeof(T));
		}
		else
		{
			// Check for a sensible element count
			if (elements < 0 || elements > AUTOARR_MAX_ELEMENT_COUNT)
			{
				throw DataFile::Error("Too many elements in auto array");
			}

			// Allocate the elements and read them back
			alloc(elements);
			pFile->exchange(reinterpret_cast<char*>(_ptr), elements * sizeof(T));
		}
	}

private:
	T * _ptr;		// Pointer to the array allocated (possibly NULL)
	int		_count;		// Number of elements in the array
};

#endif

