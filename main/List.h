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
		explicit Iterator(T* ptr) : ptr_(ptr) {}

		T& operator*() const
		{
			return *ptr_;
		}

		Iterator& operator++()
		{
			++ptr_;
			return *this;
		}

		bool operator!=(const Iterator& other) const
		{
			return ptr_ != other.ptr_;
		}

		T* operator->() const
		{
			return ptr_;
		}

		Iterator& operator--()
		{
			--ptr_;
			return *this;
		}

	private:
		T* ptr_;
	};

	Iterator begin() const
	{
		return Iterator(ptr_);
	}

	Iterator end() const
	{
		return Iterator(ptr_ + elements_);
	}

	List() : count_(MINIMUM_SIZE), elements_(0)
	{
		ptr_ = new T[MINIMUM_SIZE];
		for(char i = 0; i < MINIMUM_SIZE; i++) 
			ptr_[i] = T();
	}

	explicit List(const size_t size) : elements_(size)
	{
		const size_t temp = elements_ * STARTING_SIZE / 100;
		count_ = temp < MINIMUM_SIZE ? MINIMUM_SIZE : temp;
		ptr_ = new T[count_];
		for(size_t i = 0; i < count_; i++) 
			ptr_[i] = T();
	}

	List(T data[], const size_t size) : elements_(size)
	{
		const size_t temp = elements_ * STARTING_SIZE / 100;
		count_ = temp < MINIMUM_SIZE ? MINIMUM_SIZE : temp;
		ptr_ = new T[count_];
		for(size_t i = 0; i < size; i++) 
			ptr_[i] = data[i];
		for(size_t i = elements_; i < count_; i++) 
			ptr_[i] = T();
	}

	template <size_t N>
	explicit List(std::array<T, N> data) : elements_(N)
	{
		const size_t temp = elements_ * STARTING_SIZE / 100;
		count_ = temp < MINIMUM_SIZE ? MINIMUM_SIZE : temp;
		ptr_ = new T[count_];
		const T* p = data.data();
		for(int i = 0; i < N; i++)
			ptr_[i] = std::move(p[i]);
		for(size_t i = elements_; i < count_; i++)
			ptr_[i] = T();
	}

	explicit List(std::vector<T> data) : elements_(data.size())
	{
		const size_t temp = elements_ * STARTING_SIZE / 100;
		count_ = temp < MINIMUM_SIZE ? MINIMUM_SIZE : temp;
		ptr_ = new T[count_];
		const T* p = data.data();
		for(size_t i = 0; i < elements_; i++)
			ptr_[i] = std::move(p[i]);
		for(size_t i = elements_; i < count_; i++)
			ptr_[i] = T();
	}

	List(std::initializer_list<T> data) : elements_(data.size())
	{
		const size_t temp = elements_ * STARTING_SIZE / 100;
		count_ = temp < MINIMUM_SIZE ? MINIMUM_SIZE : temp;
		ptr_ = new T[count_];
		const T* p = data.begin();
		for(size_t i = 0; i < elements_; i++)
			ptr_[i] = p[i];
		for(size_t i = elements_; i < count_; i++)
			ptr_[i] = T();
	}

	~List()
	{
		delete[] ptr_;
		ptr_ = nullptr;
	}

	List(const List& other) : count_(other.count_), elements_(other.elements_), ptr_(nullptr)
	{
		ptr_ = new T[count_];
		for(size_t i = 0; i < count_; ++i)
			ptr_[i] = other.ptr_[i];
	}

	List& operator=(const List& other)
	{
		if(this == &other) return *this;

		delete[] ptr_;

		elements_ = other.elements_;
		count_ = other.count_;
		ptr_ = new T[count_];
		for(size_t i = 0; i < count_; ++i)
			ptr_[i] = other.ptr_[i];

		return *this;
	}

	List(List&& other) noexcept
	{
		ptr_ = other.ptr_;
		count_ = other.count_;
		elements_ = other.elements_;
		other.ptr_ = nullptr;
		other.count_ = 0;
		other.elements_ = 0;
	} 

	List& operator=(List&& other) noexcept
	{
		if(this == &other) return *this;

		clear();

		ptr_ = other.ptr_;
		count_ = other.count_;
		elements_ = other.elements_;

		other.ptr_ = nullptr;
		other.count_ = 0;
		other.elements_ = 0;

		return *this;
	}

	template <typename U = T, std::enable_if_t<is_printable<U>::value, int> = 0>
	friend std::ostream& operator<<(std::ostream& os, const List& list)
	{
		if(list.empty()) return os << "{ }" << std::endl;
		os << "{ ";
		for(size_t i = 0; i < list.elements_ - 1; i++)
			os << list.ptr_[i] << ", ";
		os << list.ptr_[list.elements_ - 1] << " }" << std::endl;
		return os;
	}

	template <typename U = T, std::enable_if_t<is_equatable<U>::value, int> = 0>
	bool operator==(const List& other) const
	{
		if(this == &other) return true;
		if(elements_ != other.elements_) return false;

		for(size_t i = 0; i < elements_; i++)
			if(ptr_[i] != other.ptr_[i]) return false;

		return true;
	}

	T& operator[](size_t index)
	{
		if(index < elements_) return ptr_[index];

		throw std::out_of_range("Index out of bounds\nindex: " + std::to_string(index) + "\nsize: " + std::to_string(elements_));
	}

	[[nodiscard]] T at(size_t index) const
	{
		if(index < elements_) return ptr_[index];

		throw std::out_of_range("Index out of bounds\nindex: " + std::to_string(index) + "\nsize: " + std::to_string(elements_));
	}

	void append(const T& data) noexcept
	{
		if(ptr_ == nullptr)
		{
			ptr_ = new T[MINIMUM_SIZE];
			count_ = MINIMUM_SIZE;
			for(char i = 0; i < count_; i++)
				ptr_[i] = T();
		}
		else if(elements_ * 100 >= UPSIZE_THRESHOLD * count_)
		{
			const size_t temp = GROWTH_FACTOR * count_ / 100;
			count_ = temp < MINIMUM_SIZE ? MINIMUM_SIZE : temp;
			T* tempptr = new T[count_];
			for(size_t i = 0; i < elements_; i++)
				tempptr[i] = std::move(ptr_[i]);
			for(size_t i = elements_; i < count_; i++)
				tempptr[i] = T();
			delete[] ptr_;
			ptr_ = tempptr;
		}

		ptr_[elements_] = data;
		elements_++;
	}

	void append(T&& data) noexcept
	{
		if(ptr_ == nullptr)
		{
			ptr_ = new T[MINIMUM_SIZE];
			count_ = MINIMUM_SIZE;
			for(char i = 0; i < count_; i++)
				ptr_[i] = T();
		}
		else if(elements_ * 100 >= UPSIZE_THRESHOLD * count_)
		{
			const size_t temp = GROWTH_FACTOR * count_ / 100;
			count_ = temp < MINIMUM_SIZE ? MINIMUM_SIZE : temp;
			T* tempptr = new T[count_];
			for(size_t i = 0; i < elements_; i++)
				tempptr[i] = std::move(ptr_[i]);
			for(size_t i = elements_; i < count_; i++)
				tempptr[i] = T();
			delete[] ptr_;
			ptr_ = tempptr;
		}
 		ptr_[elements_] = std::move(data);
		elements_++;
	}

	void insert_at(const T& data, const size_t index)
	{
		if(index < elements_)
		{
			if(count_ > MINIMUM_SIZE && elements_ * 100 >= UPSIZE_THRESHOLD * count_)
			{
				count_ = GROWTH_FACTOR * count_ / 100;
				T* temp = new T[count_];
				for(size_t i = 0; i < elements_; i++)
					temp[i] = std::move(ptr_[i]);
				for(size_t i = elements_; i < count_; i++)
					temp[i] = T();
				delete[] ptr_;
				ptr_ = temp;
			}
			for(size_t i = elements_; i > index; i--)
				ptr_[i] = std::move(ptr_[i - 1]);


			ptr_[index] = data;
			elements_++;
			return;
		}

		throw std::out_of_range("Index out of bounds\nindex: " + std::to_string(index) + "\nsize: " + std::to_string(elements_));
	}

	void insert_at(T&& data, const size_t index)
	{
		if(index < elements_)
		{
			if(count_ > MINIMUM_SIZE && elements_ * 100 >= UPSIZE_THRESHOLD * count_)
			{
				count_ = GROWTH_FACTOR * count_ / 100;
				T* temp = new T[count_];
				for(size_t i = 0; i < elements_; i++)
					temp[i] = std::move(ptr_[i]);
				for(size_t i = elements_; i < count_; i++)
					temp[i] = T();
				delete[] ptr_;
				ptr_ = temp;
			}
			for(size_t i = elements_; i > index; i--)
				ptr_[i] = std::move(ptr_[i - 1]);

			ptr_[index] = std::move(data);
			elements_++;
			return;
		}

		throw std::out_of_range("Index out of bounds\nindex: " + std::to_string(index) + "\nsize: " + std::to_string(elements_));
	}

	void remove_at(const size_t index)
	{
		if(index < elements_)
		{
			if(count_ > MINIMUM_SIZE && elements_ * 100 <= DOWNSIZE_THRESHOLD * count_)
			{
				const size_t temp = count_ * SHRINK_FACTOR / 100;
				count_ = temp < MINIMUM_SIZE ? MINIMUM_SIZE : temp;
				T* tempptr = new T[count_];
				for(size_t i = 0; i < elements_; i++)
					tempptr[i] = std::move(ptr_[i]);
				for(size_t i = elements_; i < count_; i++)
					tempptr[i] = T();
				delete[] ptr_;
				ptr_ = tempptr;
			}
			for(size_t i = index; i < elements_; i++)
				ptr_[i] = std::move(ptr_[i + 1]);
			elements_--;
			return;
		}

		throw std::out_of_range("Index out of bounds\nindex: " + std::to_string(index) + "\nsize: " + std::to_string(elements_));
	}

	void clear() noexcept
	{
		delete[] ptr_;
		ptr_ = nullptr;
		count_ = 0;
		elements_ = 0;
	}

	void resize(const size_t size, const bool allowSmaller = false)
	{
		if(size == 0) throw std::invalid_argument("size must be larger than 0");
		if(size < elements_ && !allowSmaller) throw std::invalid_argument("New size must be larger or equal than the amount of elements_:" + std::to_string(elements_));
		T* temp = new T[size];
		elements_ = size < elements_ && allowSmaller ? size : elements_;
		for(size_t i = 0; i < elements_; i++)
			temp[i] = std::move(ptr_[i]);
		for(size_t i = elements_; i < count_; i++)
			ptr_[i] = T();

		delete[] ptr_;
		ptr_ = temp;
		count_ = size;
		elements_ = size;
	}

	[[nodiscard]] bool empty() const noexcept
	{
		return elements_ == 0;
	}

	[[nodiscard]] size_t size() const noexcept
	{
		return elements_;
	}

private:
	size_t count_;
	size_t elements_;
	T* ptr_;
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
		explicit Iterator(T** ptr) : ptr_(ptr) {}

		T& operator*() const
		{
			return **ptr_;
		}

		Iterator& operator++()
		{
			++ptr_;
			return *this;
		}

		bool operator!=(const Iterator& other) const
		{
			return ptr_ != other.ptr_;
		}

		T* operator->() const
		{
			return *ptr_;
		}

		Iterator& operator--()
		{
			--ptr_;
			return *this;
		}

	private:
		T** ptr_;
	};

	Iterator begin() const
	{
		return Iterator(ptr_);
	}

	Iterator end() const
	{
		return Iterator(ptr_ + elements_);
	}

	List() : count_(MINIMUM_SIZE), elements_(0)
	{
		ptr_ = new T*[count_];
		for(char i = 0; i < count_; i++)
			ptr_[i] = nullptr;
	}

	explicit List(const size_t size) : elements_(size)
	{
		const size_t temp = elements_ * STARTING_SIZE / 100;
		count_ = temp < MINIMUM_SIZE ? MINIMUM_SIZE : temp;
		ptr_ = new T*[count_];
		for(size_t i = 0; i < elements_; i++)
			ptr_[i] = new T();
		for(size_t i = elements_; i < count_; i++)
			ptr_[i] = nullptr;
	}

	List(T data[], const size_t size) : elements_(size)
	{
		const size_t temp = elements_ * STARTING_SIZE / 100;
		count_ = temp < MINIMUM_SIZE ? MINIMUM_SIZE : temp;
		ptr_ = new T*[count_];
		for(size_t i = 0; i < size; i++)
			ptr_[i] = new T(data[i]);
		for(size_t i = elements_; i < count_; i++)
			ptr_[i] = nullptr;
	}

	template <size_t N>
	explicit List(std::array<T, N> data) : elements_(N)
	{
		const size_t temp = elements_ * STARTING_SIZE / 100;
		count_ = temp < MINIMUM_SIZE ? MINIMUM_SIZE : temp;
		ptr_ = new T*[count_];
		const T* p = data.data();
		for(int i = 0; i < N; i++)
			ptr_[i] = new T(std::move(p[i]));
		for(size_t i = elements_; i < count_; i++)
			ptr_[i] = nullptr;
	}

	explicit List(std::vector<T> data) : elements_(data.size())
	{
		const size_t temp = elements_ * STARTING_SIZE / 100;
		count_ = temp < MINIMUM_SIZE ? MINIMUM_SIZE : temp;
		ptr_ = new T*[count_];
		T* p = data.data();
		for(size_t i = 0; i < elements_; i++)
			ptr_[i] = new T(std::move(p[i]));
		for(size_t i = elements_; i < count_; i++)
			ptr_[i] = nullptr;
	}

	List(std::initializer_list<T> data) : elements_(data.size())
	{
		const size_t temp = elements_ * STARTING_SIZE / 100;
		count_ = temp < MINIMUM_SIZE ? MINIMUM_SIZE : temp;
		ptr_ = new T*[count_];
		const T* p = data.begin();
		for(size_t i = 0; i < elements_; i++)
			ptr_[i] = new T(p[i]);
		for(size_t i = elements_; i < count_; i++)
			ptr_[i] = nullptr;
	}

	~List()
	{
		for(size_t i = 0; i < elements_; i++)
			delete ptr_[i];
		delete[] ptr_;
		ptr_ = nullptr;
	}

	List(const List& other) : count_(other.count_), elements_(other.elements_)
	{
		ptr_ = new T * [count_];
		for(size_t i = 0; i < elements_; ++i)
			ptr_[i] = new T(*(other.ptr_[i]));
	}

	List& operator=(const List& other)
	{
		if(this == &other) return* this;

		clear();

		elements_ = other.elements_;
		count_ = other.count_;
		ptr_ = new T*[count_];
		for(size_t i = 0; i < elements_; ++i)
			ptr_[i] = new T(*(other.ptr_[i]));

		return *this;
	}

	List(List&& other) noexcept
	{
		ptr_ = other.ptr_;
		count_ = other.count_;
		elements_ = other.elements_;
		other.ptr_ = nullptr;
		other.count_ = 0;
		other.elements_ = 0;
	}

	List& operator=(List&& other) noexcept
	{
		if(this == &other) return *this;
		
		clear();

		ptr_ = other.ptr_;
		count_ = other.count_;
		elements_ = other.elements_;

		other.ptr_ = nullptr;
		other.count_ = 0;
		other.elements_ = 0;
		
		return *this;
	}

	template <typename U = T, std::enable_if_t<is_printable<U>::value, int> = 0>
	friend std::ostream& operator<<(std::ostream& os, const List& list)
	{
		if(list.empty()) return os << "{ }" << std::endl;
		os << "{ ";
		for(size_t i = 0; i < list.elements_ - 1; i++)
			os << *(list.ptr_[i]) << ", ";
		os << *(list.ptr_[list.elements_ - 1]) << " }" << std::endl;
		return os;
	}

	template <typename U = T, std::enable_if_t<is_equatable<U>::value, int> = 0>
	bool operator==(const List& other) const
	{
		if(this == &other) return true;
		if(elements_ != other.elements_) return false;

		for(size_t i = 0; i < elements_; i++)
			if(*ptr_[i] != *(other.ptr_[i])) return false;

		return true;
	}

	T& operator[](size_t index)
	{
		if(index < elements_) return *ptr_[index];

		throw std::out_of_range("Index out of bounds\nindex: " + std::to_string(index) + "\nsize: " + std::to_string(elements_));
	}

	[[nodiscard]] T at(size_t index) const
	{
		if(index < elements_) return *ptr_[index];

		throw std::out_of_range("Index out of bounds\nindex: " + std::to_string(index) + "\nsize: " + std::to_string(elements_));
	}

	void append(const T& data) noexcept
	{
		if(ptr_ == nullptr)
		{
			ptr_ = new T*[MINIMUM_SIZE];
			count_ = MINIMUM_SIZE;
			for(char i = 0; i < MINIMUM_SIZE; i++)
				ptr_[i] = nullptr;
		}
		else if(elements_ * 100 >= UPSIZE_THRESHOLD * count_)
		{
			const size_t temp = GROWTH_FACTOR * count_ / 100;
			count_ = temp < MINIMUM_SIZE ? MINIMUM_SIZE : temp;
			T** tempptr = new T*[count_];
			for(size_t i = 0; i < elements_; i++)
				tempptr[i] = ptr_[i];
			for(size_t i = elements_; i < count_; i++)
				tempptr[i] = nullptr;
			delete[] ptr_;
			ptr_ = tempptr;
		}

		ptr_[elements_] = new T(data);
		elements_++;
	}

	void append(T&& data) noexcept
	{
		if(ptr_ == nullptr)
		{
			ptr_ = new T * [MINIMUM_SIZE];
			count_ = MINIMUM_SIZE;
			for(char i = 0; i < MINIMUM_SIZE; i++)
				ptr_[i] = nullptr;
		}
		else if(elements_ * 100 >= UPSIZE_THRESHOLD * count_)
		{
			const size_t temp = GROWTH_FACTOR * count_ / 100;
			count_ = temp < MINIMUM_SIZE ? MINIMUM_SIZE : temp;
			T** tempptr = new T * [count_];
			for(size_t i = 0; i < elements_; i++)
				tempptr[i] = ptr_[i];
			for(size_t i = elements_; i < count_; i++)
				tempptr[i] = nullptr;
			delete[] ptr_;
			ptr_ = tempptr;
		}

		ptr_[elements_] = new T(std::move(data));
		elements_++;
	}

	void insert_at(const T& data, size_t index)
	{
		if(index < elements_ || index == 0)
		{
			if(count_ > MINIMUM_SIZE && elements_ * 100 >= UPSIZE_THRESHOLD * count_)
			{
				count_ = GROWTH_FACTOR * count_ / 100;
				T** temp = new T*[count_];
				for(size_t i = 0; i < elements_; i++)
					temp[i] = ptr_[i];
				for(size_t i = elements_; i < count_; i++)
					temp[i] = nullptr;
				delete[] ptr_;
				ptr_ = temp;
			}
			for(size_t i = elements_; i > index; i--)
				ptr_[i] = ptr_[i - 1];

			ptr_[index] = new T(data);
			elements_++;
			return;
		}

		throw std::out_of_range("Index out of bounds\nindex: " + std::to_string(index) + "\nsize: " + std::to_string(elements_));
	}

	void insert_at(T&& data, size_t index)
	{
		if(index < elements_ || index == 0)
		{
			if(count_ > MINIMUM_SIZE && elements_ * 100 >= UPSIZE_THRESHOLD * count_)
			{
				count_ = GROWTH_FACTOR * count_ / 100;
				T** temp = new T * [count_];
				for(size_t i = 0; i < elements_; i++)
					temp[i] = ptr_[i];
				for(size_t i = elements_; i < count_; i++)
					temp[i] = nullptr;
				delete[] ptr_;
				ptr_ = temp;
			}
			for(size_t i = elements_; i > index; i--)
				ptr_[i] = ptr_[i - 1];

			ptr_[index] = new T(std::move(data));
			elements_++;
			return;
		}

		throw std::out_of_range("Index out of bounds\nindex: " + std::to_string(index) + "\nsize: " + std::to_string(elements_));
	}

	void remove_at(const size_t index)
	{
		if(index < elements_)
		{
			if(count_ > MINIMUM_SIZE && elements_ * 100 <= DOWNSIZE_THRESHOLD * count_)
			{
				const size_t temp = count_ * SHRINK_FACTOR / 100;
				count_ = temp < MINIMUM_SIZE ? MINIMUM_SIZE : temp;
				T** tempptr = new T*[count_];
				for(size_t i = 0; i < elements_; i++)
					tempptr[i] = ptr_[i];
				for(size_t i = elements_; i < count_; i++)
					tempptr[i] = nullptr;
				delete[] ptr_;
				ptr_ = tempptr;
			}
			delete ptr_[index];
			for(size_t i = index; i < elements_; i++)
					ptr_[i] = ptr_[i + 1];
			elements_--;
			return;
		}

		throw std::out_of_range("Index out of bounds\nindex: " + std::to_string(index) + "\nsize: " + std::to_string(elements_));
	}

	void clear() noexcept
	{
		for(size_t i = 0; i < count_; i++)
			delete ptr_[i];
		delete[] ptr_;
		ptr_ = nullptr;
		count_ = 0;
		elements_ = 0;
	}

	void resize(const size_t size, const bool allowSmaller = false)
	{
		if(size == 0) throw std::invalid_argument("size must be larger than 0");
		if(size < elements_ && !allowSmaller) throw std::invalid_argument("New size must be larger or equal than the amount of elements_:" + std::to_string(elements_));
		T** temp = new T*[size];
		if(size < elements_ && allowSmaller)
		{
			for(size_t i = 0; i < size; i++) temp[i] = std::move(ptr_[i]);
			for(size_t i = size; i < elements_; i++)
			{
				delete ptr_[i];
				ptr_[i] = nullptr;
			}
		}
		else
			for(size_t i = 0; i < count_; i++) temp[i] = std::move(ptr_[i]);

		delete[] ptr_;
		ptr_ = temp;
		count_ = size;
		elements_ = size;
	}

	[[nodiscard]] size_t size() const noexcept
	{
		return elements_;
	}

	[[nodiscard]] bool empty() const noexcept
	{
		return elements_ == 0;
	}


private:
	size_t count_;
	size_t elements_;
	T** ptr_;
};
#endif // LIST_H