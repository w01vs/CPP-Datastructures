#ifndef QUEUE_H
#define QUEUE_H

#include <array>
#include <vector>
#include <stdexcept>
#include "TypeTraitsCheck.h"
#include "DLinkedList.h"

template<typename T>
class Queue
{
	static_assert(std::is_copy_constructible_v<T>, "Attempted to make a Queue with a type that does not implement a copy constructor");
	static_assert(!std::is_pointer_v<T>, "Attempted to make a Queue with a raw pointer type");
public:
	Queue() : list_(DLinkedList<T>()) {}

	explicit Queue(T data[], const size_t size) : list_(DLinkedList<T>(data,size)) {}

	explicit Queue(std::vector<T> data) : list_(DLinkedList<T>(data)) {}

	template <size_t N>
	explicit Queue(std::array<T, N> data) : list_(DLinkedList<T>(data)) {}

	explicit Queue(std::initializer_list<T> data) : list_(DLinkedList<T>(data)) {}

	~Queue() = default;

	Queue(const Queue& other) : list_(DLinkedList<T>(other.list_)) {}

	Queue& operator=(const Queue& other)
	{
		if(this == &other) return *this;

		clear();
		list_ = other.list_;

		return *this;
	}

	Queue(Queue&& other) noexcept
	{
		list_ = other.list_;
	}

	Queue& operator=(Queue&& other) noexcept
	{
		if(this == &other) return *this;

		clear();
		list_ = other.list_;

		return *this;
	}

	template <typename U = T, std::enable_if_t<is_printable<U>::value, int> = 0>
	friend std::ostream& operator<<(std::ostream& os, const Queue& stack)
	{
		os << stack.list_;
		return os;
	}

	template <typename U = T, std::enable_if_t<is_equatable<U>::value, int> = 0>
	bool operator==(const Queue& other) const
	{
		if(this == &other) return true;
		return list_ == other.list_;
	}

	T pop()
	{
		if(empty())
		{
			throw std::out_of_range("Stack is empty");
		}
		return list_.pop_front();
	}

	[[nodiscard]] T peek() const
	{
		if(empty())
		{
			throw std::out_of_range("Stack is empty");
		}
		return list_.front();
	}

	void push(const T& data)
	{
		list_.append(data);
	}
	void clear()
	{
		list_.clear();
	}

	[[nodiscard]] size_t size() const
	{
		return list_.size();
	}

	[[nodiscard]] bool empty() const
	{
		return list_.empty();
	}

private:
	DLinkedList<T> list_;
};

#endif // QUEUE_h