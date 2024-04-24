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
	Queue() : list(DLinkedList<T>()) {}

	explicit Queue(T data[], const size_t size) : list(DLinkedList<T>(data,size)) {}

	explicit Queue(std::vector<T> data) : list(DLinkedList<T>(data)) {}

	template <size_t N>
	explicit Queue(std::array<T, N> data) : list(DLinkedList<T>(data)) {}

	explicit Queue(std::initializer_list<T> data) : list(DLinkedList<T>(data)) {}

	~Queue() = default;

	Queue(const Queue& other) : list(DLinkedList<T>(other.list)) {}

	Queue& operator=(const Queue& other)
	{
		if(this == &other) return *this;

		Clear();
		list = other.list;

		return *this;
	}

	Queue(Queue&& other) noexcept
	{
		list = other.list;
	}

	Queue& operator=(Queue&& other) noexcept
	{
		if(this == &other) return *this;

		Clear();
		list = other.list;

		return *this;
	}

	template <typename U = T, std::enable_if_t<is_printable<U>::value, int> = 0>
	friend std::ostream& operator<<(std::ostream& os, const Queue& stack)
	{
		os << stack.list;
		return os;
	}

	template <typename U = T, std::enable_if_t<is_equatable<U>::value, int> = 0>
	bool operator==(const Queue& other) const
	{
		if(this == &other) return true;
		return list == other.list;
	}

	T Pop()
	{
		if(Empty())
		{
			throw std::out_of_range("Stack is empty");
		}
		return list.PopFront();
	}

	[[nodiscard]] T Peek() const
	{
		if(Empty())
		{
			throw std::out_of_range("Stack is empty");
		}
		return list.Front();
	}

	void Push(const T& data)
	{
		list.Append(data);
	}
	void Clear()
	{
		list.Clear();
	}

	[[nodiscard]] size_t Size() const
	{
		return list.Size();
	}

	[[nodiscard]] bool Empty() const
	{
		return list.Empty();
	}

private:
	DLinkedList<T> list;
};

#endif // QUEUE_h