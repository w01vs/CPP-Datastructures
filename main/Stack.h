#ifndef STACK_H
#define STACK_H

#include <vector>
#include <array>
#include <stdexcept>
#include "SLinkedList.h"
#include "TypeTraitsCheck.h"

template <typename T>
class Stack
{
	static_assert(std::is_copy_constructible_v<T>, "Attempted to make a Stack with a type that does not implement a copy constructor");
	static_assert(!std::is_pointer_v<T>, "Attempted to make a Stack with a raw pointer type");
public:
	Stack() : count(0), list(SLinkedList<T>()) {}

	Stack(T data[], const size_t size) : count(size)
	{
		list = SLinkedList<T>(data, size);
	}

	explicit Stack(std::vector<T> data) : count(data.size())
	{
		list = SLinkedList<T>(data);
	}

	template <size_t N>
	explicit Stack(std::array<T, N> data) : count(N)
	{
		list = SLinkedList<T>(data);
	}

	Stack(std::initializer_list<T> data) : count(data.size())
	{
		list = SLinkedList<T>(data);
	}

	~Stack() = default;

	Stack(const Stack& other)
	{
		count = other.count;
		list = new SLinkedList<T>(*(other.list));
	}

	Stack& operator=(const Stack& other)
	{
		if(this == &other) return *this;

		Clear();
		count = other.count;
		list = new SLinkedList<T>(*(other.list));
		
		return *this;
	}

	Stack(Stack&& other) noexcept
	{
		count = other.count;
		list = other.list;
		other.list = nullptr;
	}

	Stack& operator=(Stack&& other) noexcept
	{
		if(this == &other) return *this;

		Clear();
		count = other.count;
		list = other.list;
		other.list = nullptr;

		return *this;
	}

	template <typename U = T, std::enable_if_t<is_printable<U>::value, int> = 0>
	friend std::ostream& operator<<(std::ostream& os, const Stack& stack)
	{
		os << stack.list;
		return os;
	}

	template <typename U = T, std::enable_if_t<is_equatable<U>::value, int> = 0>
	bool operator==(const Stack& other) const
	{
		if(this == &other) return true;
		return list == other.list;
	}

	T Pop()
	{
		if (Empty())
		{
			throw std::out_of_range("Stack is empty");
		}
		count--;
		return list.PopFront();
	}

	[[nodiscard]] T Peek() const
	{
		if (Empty())
		{
			throw std::out_of_range("Stack is empty");
		}
		return list.Front();
	}

	void Push(const T& data)
	{
		count++;
		list.Prepend(data);
	}

	void Push(T&& data)
	{
		count++;
		list.Prepend(std::move(data));
	}

	void Clear()
	{
		count = 0;
	}

	[[nodiscard]] size_t Size() const
	{
		return count;
	}

	[[nodiscard]] bool Empty() const
	{
		return count == 0;
	}

private:
	size_t count;
	SLinkedList<T> list;
};


#endif // STACK_H