#ifndef LIST_H
#define LIST_H

#include <array>
#include <vector>
#include <stdexcept>
#include <string>
#include "TypeTraitsCheck.h"
#include <memory>

// Percentage of the internal array that is populated
enum
{
	DOWNSIZE_THRESHOLD = 55,
	STARTING_SIZE = 75,
	UPSIZE_THRESHOLD = 90,
	GROWTH_FACTOR = 200,
	SHRINK_FACTOR = 70,
	MINIMUM_SIZE = 8
};

template <typename T, bool = std::is_arithmetic_v<T>>
class List
{
public:
	class Iterator
	{
	public:
		explicit Iterator(T* ptr) : ptr(ptr) {}

		T& operator*() const
		{
			return *ptr;
		}

		Iterator& operator++()
		{
			++ptr;
			return *this;
		}

		bool operator!=(const Iterator& other) const
		{
			return ptr != other.ptr;
		}

		T* operator->() const
		{
			return ptr;
		}

		Iterator& operator--()
		{
			--ptr;
			return *this;
		}

	private:
		T* ptr;
	};

	Iterator begin() const
	{
		return Iterator(ptr);
	}

	Iterator end() const
	{
		return Iterator(ptr + elements);
	}

	List() : count(MINIMUM_SIZE), elements(0)
	{
		ptr = new T[MINIMUM_SIZE];
		for(char i = 0; i < MINIMUM_SIZE; i++)
		{
			ptr[i] = T();
		}
	}

	explicit List(const size_t size) : elements(size)
	{
		const size_t temp = elements * STARTING_SIZE / 100;
		count = temp < MINIMUM_SIZE ? MINIMUM_SIZE : temp;
		ptr = new T[count];
		for(size_t i = 0; i < count; i++)
		{
			ptr[i] = T();
		}
	}

	List(T data[], const size_t size) : elements(size)
	{
		const size_t temp = elements * STARTING_SIZE / 100;
		count = temp < MINIMUM_SIZE ? MINIMUM_SIZE : temp;
		ptr = new T[count];
		for(size_t i = 0; i < size; i++)
		{
			ptr[i] = data[i];
		}
		for(size_t i = elements; i < count; i++)
		{
			ptr[i] = T();
		}
	}

	template <size_t N>
	explicit List(std::array<T, N> data) : elements(N)
	{
		const size_t temp = elements * STARTING_SIZE / 100;
		count = temp < MINIMUM_SIZE ? MINIMUM_SIZE : temp;
		ptr = new T[count];
		const T* p = data.data();
		for(int i = 0; i < N; i++)
		{
			ptr[i] = std::move(p[i]);
		}
		for(size_t i = elements; i < count; i++)
		{
			ptr[i] = T();
		}
	}

	explicit List(std::vector<T> data) : elements(data.size())
	{
		const size_t temp = elements * STARTING_SIZE / 100;
		count = temp < MINIMUM_SIZE ? MINIMUM_SIZE : temp;
		ptr = new T[count];
		const T* p = data.data();
		for(size_t i = 0; i < elements; i++)
		{
			ptr[i] = std::move(p[i]);
		}
		for(size_t i = elements; i < count; i++)
		{
			ptr[i] = T();
		}
	}

	List(std::initializer_list<T> data) : elements(data.size())
	{
		const size_t temp = elements * STARTING_SIZE / 100;
		count = temp < MINIMUM_SIZE ? MINIMUM_SIZE : temp;
		ptr = new T[count];
		const T* p = data.begin();
		for(size_t i = 0; i < elements; i++)
		{
			ptr[i] = p[i];
		}
		for(size_t i = elements; i < count; i++)
		{
			ptr[i] = T();
		}
	}

	~List()
	{
		delete[] ptr;
		ptr = nullptr;
	}

	List(const List& other) : count(other.count), elements(other.elements), ptr(nullptr)
	{
		ptr = new T[count];
		for(size_t i = 0; i < count; ++i)
		{
			ptr[i] = other.ptr[i];
		}
	}

	List& operator=(const List& other)
	{
		if(this == &other) return *this;

		delete[] ptr;

		elements = other.elements;
		count = other.count;
		ptr = new T[count];
		for(size_t i = 0; i < count; ++i)
		{
			ptr[i] = other.ptr[i];
		}

		return *this;
	}

	List(List&& other) noexcept
	{
		ptr = other.ptr;
		count = other.count;
		elements = other.elements;
		other.ptr = nullptr;
		other.count = 0;
		other.elements = 0;
	} 

	List& operator=(List&& other) noexcept
	{
		if(this == &other) return *this;

		Clear();

		ptr = other.ptr;
		count = other.count;
		elements = other.elements;

		other.ptr = nullptr;
		other.count = 0;
		other.elements = 0;

		return *this;
	}

	template <typename U = T, std::enable_if_t<is_printable<U>::value, int> = 0>
	friend std::ostream& operator<<(std::ostream& os, const List& list)
	{
		if(list.Empty())
		{
			return os << "{ }" << std::endl;
		}
		os << "{ ";
		for(size_t i = 0; i < list.elements - 1; i++)
		{
			os << list.ptr[i] << ", ";
		}
		os << list.ptr[list.elements - 1] << " }" << std::endl;
		return os;
	}

	template <typename U = T, std::enable_if_t<is_equatable<U>::value, int> = 0>
	bool operator==(const List& other) const
	{
		if(this == &other) return true;
		if(elements != other.elements) return false;

		for(size_t i = 0; i < elements; i++)
		{
			if(ptr[i] != other.ptr[i])
			{
				return false;
			}
		}

		return true;
	}

	T& operator[](size_t index)
	{
		if(index < elements)
		{
			return ptr[index];
		}

		throw std::out_of_range("Index out of bounds\nindex: " + std::to_string(index) + "\nsize: " + std::to_string(elements));
	}

	[[nodiscard]] T At(size_t index) const
	{
		if(index < elements)
		{
			return ptr[index];
		}

		throw std::out_of_range("Index out of bounds\nindex: " + std::to_string(index) + "\nsize: " + std::to_string(elements));
	}

	void Append(const T& data) noexcept
	{
		if(ptr == nullptr)
		{
			ptr = new T[MINIMUM_SIZE];
			count = MINIMUM_SIZE;
			for(char i = 0; i < count; i++)
			{
				ptr[i] = T();
			}
		}
		else if(elements * 100 >= UPSIZE_THRESHOLD * count)
		{
			const size_t temp = GROWTH_FACTOR * count / 100;
			count = temp < MINIMUM_SIZE ? MINIMUM_SIZE : temp;
			T* tempptr = new T[count];
			for(size_t i = 0; i < elements; i++)
			{
				tempptr[i] = std::move(ptr[i]);
			}
			for(size_t i = elements; i < count; i++)
			{
				tempptr[i] = T();
			}
			delete[] ptr;
			ptr = tempptr;
		}

		ptr[elements] = data;
		elements++;
	}

	void Append(T&& data) noexcept
	{
		if(ptr == nullptr)
		{
			ptr = new T[MINIMUM_SIZE];
			count = MINIMUM_SIZE;
			for(char i = 0; i < count; i++)
			{
				ptr[i] = T();
			}
		}
		else if(elements * 100 >= UPSIZE_THRESHOLD * count)
		{
			const size_t temp = GROWTH_FACTOR * count / 100;
			count = temp < MINIMUM_SIZE ? MINIMUM_SIZE : temp;
			T* tempptr = new T[count];
			for(size_t i = 0; i < elements; i++)
			{
				tempptr[i] = std::move(ptr[i]);
			}
			for(size_t i = elements; i < count; i++)
			{
				tempptr[i] = T();
			}
			delete[] ptr;
			ptr = tempptr;
		}

		ptr[elements] = std::move(data);
		elements++;
	}

	void InsertAt(const T& data, const size_t index)
	{
		if(index < elements)
		{
			if(count > MINIMUM_SIZE && elements * 100 >= UPSIZE_THRESHOLD * count)
			{
				count = GROWTH_FACTOR * count / 100;
				T* temp = new T[count];
				for(size_t i = 0; i < elements; i++)
				{
					temp[i] = std::move(ptr[i]);
				}
				for(size_t i = elements; i < count; i++)
				{
					temp[i] = T();
				}
				delete[] ptr;
				ptr = temp;
			}
			for(size_t i = elements; i > index; i--)
			{
				ptr[i] = std::move(ptr[i - 1]);
			}

			ptr[index] = data;
			elements++;
			return;
		}

		throw std::out_of_range("Index out of bounds\nindex: " + std::to_string(index) + "\nsize: " + std::to_string(elements));
	}

	void InsertAt(T&& data, const size_t index)
	{
		if(index < elements)
		{
			if(count > MINIMUM_SIZE && elements * 100 >= UPSIZE_THRESHOLD * count)
			{
				count = GROWTH_FACTOR * count / 100;
				T* temp = new T[count];
				for(size_t i = 0; i < elements; i++)
				{
					temp[i] = std::move(ptr[i]);
				}
				for(size_t i = elements; i < count; i++)
				{
					temp[i] = T();
				}
				delete[] ptr;
				ptr = temp;
			}
			for(size_t i = elements; i > index; i--)
			{
				ptr[i] = std::move(ptr[i - 1]);
			}

			ptr[index] = std::move(data);
			elements++;
			return;
		}

		throw std::out_of_range("Index out of bounds\nindex: " + std::to_string(index) + "\nsize: " + std::to_string(elements));
	}

	void RemoveAt(const size_t index)
	{
		if(index < elements)
		{
			if(count > MINIMUM_SIZE && elements * 100 <= DOWNSIZE_THRESHOLD * count)
			{
				const size_t temp = count * SHRINK_FACTOR / 100;
				count = temp < MINIMUM_SIZE ? MINIMUM_SIZE : temp;
				T* tempptr = new T[count];
				for(size_t i = 0; i < elements; i++)
				{
					tempptr[i] = std::move(ptr[i]);
				}
				for(size_t i = elements; i < count; i++)
				{
					tempptr[i] = T();
				}
				delete[] ptr;
				ptr = tempptr;
			}
			for(size_t i = index; i < elements; i++)
			{
				ptr[i] = std::move(ptr[i + 1]);
			}
			elements--;
			return;
		}

		throw std::out_of_range("Index out of bounds\nindex: " + std::to_string(index) + "\nsize: " + std::to_string(elements));
	}

	void Clear() noexcept
	{
		delete[] ptr;
		ptr = nullptr;
		count = 0;
		elements = 0;
	}

	void Resize(const size_t size, const bool allowSmaller = false)
	{
		if(size < elements && !allowSmaller)
		{
			throw std::invalid_argument("New Size must be larger or equal than the amount of elements:" + std::to_string(elements));
		}
		T* temp = new T[size];
		elements = size < elements && allowSmaller ? size : elements;
		for(size_t i = 0; i < elements; i++)
		{
			temp[i] = std::move(ptr[i]);
		}
		for(size_t i = elements; i < count; i++)
		{
			ptr[i] = T();
		}

		delete[] ptr;
		ptr = temp;
		count = size;
		elements = size;
	}

	[[nodiscard]] bool Empty() const noexcept
	{
		return elements == 0;
	}

	[[nodiscard]] size_t Size() const noexcept
	{
		return elements;
	}

private:
	size_t count;
	size_t elements;
	T* ptr;
};

template <typename T>
class List<T, false>
{
	static_assert(std::is_default_constructible_v<T>, "Attempted to make a List with a type that does not implement a default constructor");
	static_assert(std::is_copy_constructible_v<T>, "Attempted to make a List with a type that does not implement a copy constructor");
	static_assert(!std::is_pointer_v<T>, "Attempted to make a List with a raw pointer type");
public:
	class Iterator
	{
	public:
		explicit Iterator(T** ptr) : ptr(ptr) {}

		T& operator*() const
		{
			return **ptr;
		}

		Iterator& operator++()
		{
			++ptr;
			return *this;
		}

		bool operator!=(const Iterator& other) const
		{
			return ptr != other.ptr;
		}

		T* operator->() const
		{
			return *ptr;
		}

		Iterator& operator--()
		{
			--ptr;
			return *this;
		}

	private:
		T** ptr;
	};

	Iterator begin() const
	{
		return Iterator(ptr);
	}

	Iterator end() const
	{
		return Iterator(ptr + elements);
	}

	List() : count(MINIMUM_SIZE), elements(0)
	{
		ptr = new T*[count];
		for(char i = 0; i < count; i++)
		{
			ptr[i] = nullptr;
		}
	}

	explicit List(const size_t size) : elements(size)
	{
		const size_t temp = elements * STARTING_SIZE / 100;
		count = temp < MINIMUM_SIZE ? MINIMUM_SIZE : temp;
		ptr = new T*[count];
		for(size_t i = 0; i < elements; i++)
		{
			ptr[i] = new T();
		}
		for(size_t i = elements; i < count; i++)
		{
			ptr[i] = nullptr;
		}
	}

	List(T data[], const size_t size) : elements(size)
	{
		const size_t temp = elements * STARTING_SIZE / 100;
		count = temp < MINIMUM_SIZE ? MINIMUM_SIZE : temp;
		ptr = new T*[count];
		for(size_t i = 0; i < size; i++)
		{
			ptr[i] = new T(data[i]);
		}
		for(size_t i = elements; i < count; i++)
		{
			ptr[i] = nullptr;
		}
	}

	template <size_t N>
	explicit List(std::array<T, N> data) : elements(N)
	{
		const size_t temp = elements * STARTING_SIZE / 100;
		count = temp < MINIMUM_SIZE ? MINIMUM_SIZE : temp;
		ptr = new T*[count];
		const T* p = data.data();
		for(int i = 0; i < N; i++)
		{
			ptr[i] = new T(std::move(p[i]));
		}
		for(size_t i = elements; i < count; i++)
		{
			ptr[i] = nullptr;
		}
	}

	explicit List(std::vector<T> data) : elements(data.size())
	{
		const size_t temp = elements * STARTING_SIZE / 100;
		count = temp < MINIMUM_SIZE ? MINIMUM_SIZE : temp;
		ptr = new T*[count];
		T* p = data.data();
		for(size_t i = 0; i < elements; i++)
		{
			ptr[i] = new T(std::move(p[i]));
		}
		for(size_t i = elements; i < count; i++)
		{
			ptr[i] = nullptr;
		}
	}

	List(std::initializer_list<T> data) : elements(data.size())
	{
		const size_t temp = elements * STARTING_SIZE / 100;
		count = temp < MINIMUM_SIZE ? MINIMUM_SIZE : temp;
		ptr = new T*[count];
		const T* p = data.begin();
		for(size_t i = 0; i < elements; i++)
		{
			ptr[i] = new T(p[i]);
		}
		for(size_t i = elements; i < count; i++)
		{
			ptr[i] = nullptr;
		}
	}

	~List()
	{
		for(size_t i = 0; i < count; i++)
		{
			delete ptr[i];
		}
		delete[] ptr;
		ptr = nullptr;
	}

	List(const List& other) : count(other.count), elements(other.elements)
	{
		ptr = new T * [count];
		for(size_t i = 0; i < elements; ++i)
		{
			ptr[i] = new T(*(other.ptr[i]));
		}
	}

	List& operator=(const List& other)
	{
		if(this == &other) return* this;

		Clear();

		elements = other.elements;
		count = other.count;
		ptr = new T*[count];
		for(size_t i = 0; i < elements; ++i)
		{
			ptr[i] = new T(*(other.ptr[i]));
		}

		return *this;
	}

	List(List&& other) noexcept
	{
		ptr = other.ptr;
		count = other.count;
		elements = other.elements;
		other.ptr = nullptr;
		other.count = 0;
		other.elements = 0;
	}

	List& operator=(List&& other) noexcept
	{
		if(this == &other) return *this;
		
		Clear();

		ptr = other.ptr;
		count = other.count;
		elements = other.elements;

		other.ptr = nullptr;
		other.count = 0;
		other.elements = 0;
		
		return *this;
	}

	template <typename U = T, std::enable_if_t<is_printable<U>::value, int> = 0>
	friend std::ostream& operator<<(std::ostream& os, const List& list)
	{
		if(list.Empty())
		{
			return os << "{ }" << std::endl;
		}
		os << "{ ";
		for(size_t i = 0; i < list.elements - 1; i++)
		{
			os << *(list.ptr[i]) << ", ";
		}
		os << *(list.ptr[list.elements - 1]) << " }" << std::endl;
		return os;
	}

	template <typename U = T, std::enable_if_t<is_equatable<U>::value, int> = 0>
	bool operator==(const List& other) const
	{
		if(this == &other) return true;
		if(elements != other.elements) return false;

		for(size_t i = 0; i < elements; i++)
		{
			if(*ptr[i] != *(other.ptr[i]))
			{
				return false;
			}
		}

		return true;
	}

	T& operator[](size_t index)
	{
		if(index < elements)
		{
			return *ptr[index];
		}

		throw std::out_of_range("Index out of bounds\nindex: " + std::to_string(index) + "\nsize: " + std::to_string(elements));
	}

	[[nodiscard]] T At(size_t index) const
	{
		if(index < elements)
		{
			return *ptr[index];
		}

		throw std::out_of_range("Index out of bounds\nindex: " + std::to_string(index) + "\nsize: " + std::to_string(elements));
	}

	void Append(const T& data) noexcept
	{
		if(ptr == nullptr)
		{
			ptr = new T*[MINIMUM_SIZE];
			count = MINIMUM_SIZE;
			for(char i = 0; i < MINIMUM_SIZE; i++)
			{
				ptr[i] = nullptr;
			}
		}
		else if(elements * 100 >= UPSIZE_THRESHOLD * count)
		{
			const size_t temp = GROWTH_FACTOR * count / 100;
			count = temp < MINIMUM_SIZE ? MINIMUM_SIZE : temp;
			T** tempptr = new T*[count];
			for(size_t i = 0; i < elements; i++)
			{
				tempptr[i] = ptr[i];
			}
			for(size_t i = elements; i < count; i++)
			{
				tempptr[i] = nullptr;
			}
			delete[] ptr;
			ptr = tempptr;
		}

		ptr[elements] = new T(data);
		elements++;
	}

	void Append(T&& data) noexcept
	{
		if(ptr == nullptr)
		{
			ptr = new T * [MINIMUM_SIZE];
			count = MINIMUM_SIZE;
			for(char i = 0; i < MINIMUM_SIZE; i++)
			{
				ptr[i] = nullptr;
			}
		}
		else if(elements * 100 >= UPSIZE_THRESHOLD * count)
		{
			const size_t temp = GROWTH_FACTOR * count / 100;
			count = temp < MINIMUM_SIZE ? MINIMUM_SIZE : temp;
			T** tempptr = new T * [count];
			for(size_t i = 0; i < elements; i++)
			{
				tempptr[i] = ptr[i];
			}
			for(size_t i = elements; i < count; i++)
			{
				tempptr[i] = nullptr;
			}
			delete[] ptr;
			ptr = tempptr;
		}

		ptr[elements] = new T(std::move(data));
		elements++;
	}

	void InsertAt(const T& data, size_t index)
	{
		if(index < elements || index == 0)
		{
			if(count > MINIMUM_SIZE && elements * 100 >= UPSIZE_THRESHOLD * count)
			{
				count = GROWTH_FACTOR * count / 100;
				T** temp = new T*[count];
				for(size_t i = 0; i < elements; i++)
				{
					temp[i] = ptr[i];
				}
				for(size_t i = elements; i < count; i++)
				{
					temp[i] = nullptr;
				}
				delete[] ptr;
				ptr = temp;
			}
			for(size_t i = elements; i > index; i--)
			{
				ptr[i] = ptr[i - 1];
			}

			ptr[index] = new T(data);
			elements++;
			return;
		}

		throw std::out_of_range("Index out of bounds\nindex: " + std::to_string(index) + "\nsize: " + std::to_string(elements));
	}

	void InsertAt(T&& data, size_t index)
	{
		if(index < elements || index == 0)
		{
			if(count > MINIMUM_SIZE && elements * 100 >= UPSIZE_THRESHOLD * count)
			{
				count = GROWTH_FACTOR * count / 100;
				T** temp = new T * [count];
				for(size_t i = 0; i < elements; i++)
				{
					temp[i] = ptr[i];
				}
				for(size_t i = elements; i < count; i++)
				{
					temp[i] = nullptr;
				}
				delete[] ptr;
				ptr = temp;
			}
			for(size_t i = elements; i > index; i--)
			{
				ptr[i] = ptr[i - 1];
			}

			ptr[index] = new T(std::move(data));
			elements++;
			return;
		}

		throw std::out_of_range("Index out of bounds\nindex: " + std::to_string(index) + "\nsize: " + std::to_string(elements));
	}

	void RemoveAt(const size_t index)
	{
		if(index < elements)
		{
			if(count > MINIMUM_SIZE && elements * 100 <= DOWNSIZE_THRESHOLD * count)
			{
				const size_t temp = count * SHRINK_FACTOR / 100;
				count = temp < MINIMUM_SIZE ? MINIMUM_SIZE : temp;
				T** tempptr = new T*[count];
				for(size_t i = 0; i < elements; i++)
				{
					tempptr[i] = ptr[i];
				}
				for(size_t i = elements; i < count; i++)
				{
					tempptr[i] = nullptr;
				}
				delete[] ptr;
				ptr = tempptr;
			}
			delete ptr[index];
			for(size_t i = index; i < elements; i++)
			{
					ptr[i] = ptr[i + 1];
			}
			elements--;
			return;
		}

		throw std::out_of_range("Index out of bounds\nindex: " + std::to_string(index) + "\nsize: " + std::to_string(elements));
	}

	void Clear() noexcept
	{
		for(size_t i = 0; i < count; i++)
		{
			delete ptr[i];
		}
		delete[] ptr;
		ptr = nullptr;
		count = 0;
		elements = 0;
	}

	void Resize(const size_t size, const bool allowSmaller = false)
	{
		if(size < elements && !allowSmaller)
		{
			throw std::invalid_argument("New Size must be larger or equal than the amount of elements:" + std::to_string(elements));
		}
		T** temp = new T*[size];
		if(size < elements && allowSmaller)
		{
			for(size_t i = 0; i < size; i++)
			{
				temp[i] = std::move(ptr[i]);
			}
			for(size_t i = size; i < elements; i++)
			{
				delete ptr[i];
				ptr[i] = nullptr;
			}
		}
		else
		{
			for(size_t i = 0; i < count; i++)
			{
				temp[i] = std::move(ptr[i]);
			}
		}

		delete[] ptr;
		ptr = temp;
		count = size;
		elements = size;
	}

	[[nodiscard]] size_t Size() const noexcept
	{
		return elements;
	}

	[[nodiscard]] bool Empty() const noexcept
	{
		return elements == 0;
	}


private:
	size_t count;
	size_t elements;
	T** ptr;
};
#endif // LIST_H