#pragma once
#include "../ErrorLogger.h"
//Implements const-sized ring buffer. Requires that type T has default constructor.
template <typename T>
struct SyRingBuffer
{
	SyRingBuffer(unsigned size_) : CAPACITY(size_), data(new T[size_]) {};
	~SyRingBuffer()
	{
		delete[] data;
	}
	//Returns index of the head element in ring buffer
	unsigned Head() const
	{
		return (size == CAPACITY) ? tail : 0;
	}
	//Returns index of the element after data[index_]. If new (!) index is out of range, returns UINT32_MAX.
	unsigned Next(unsigned index_) const
	{
		if (index_ >= CAPACITY)
			return UINT32_MAX;
		return (index_ == CAPACITY - 1) ? 0 : index_ + 1;
	}
	//Returns true if index_ points at or after the end of buffer. Useful for loops
	bool Stopped(unsigned index_) const
	{
		static bool isHead = true;
		if (size == CAPACITY)
		{
			if (index_ == tail)
				if (isHead == true)
				{
					isHead = false;
					return false;
				}
				else
				{
					isHead = true;
					return true;
				}
		}
		else
			return index_ >= size;
	}
	//Returns ref to the element data[index_] (as if in array, without respect to head advance). If index_ > size, a dummy element is returned and error is thrown.
	T& operator[](unsigned index_)
	{
		if (index_ >= size)
		{
			SY_LOG_CORE(SY_LOGLEVEL_ERROR, "Index %d is out of range of the ring buffer! ");
			dummy = T();
			return dummy;
		}
		return data[index_];
	}
	//Appends element_ to the buffer. If ring buffer is full, an existing element will be replaced (and, if pointer, delete will be called)  with element_.
	void Push(const T& element_)
	{
		data[tail] = element_;
		unsigned oldSize = size;
		size = (oldSize == CAPACITY) ? oldSize : oldSize + 1;
		tail = (tail == CAPACITY - 1) ? 0 : tail + 1;
	}
	//Empties the buffer. IMPORTANT: If the buffer contained unfreed memory, using this method will result in memory leak, so prior you have to manually free the memory!
	void Clear()
	{
		delete[] data;
		data = new T[CAPACITY];
		size = 0;
		tail = 0;
	}
private:

	unsigned tail = 0;
	unsigned size = 0;
	const unsigned CAPACITY;
	T* data;

	T dummy = T();
};