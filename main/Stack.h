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
	Stack() : count_(0), list_(SLinkedList<T>()) {}

	Stack(T data[], const size_t size) : count_(size), list_(SLinkedList<T>(data)) {}

	explicit Stack(std::vector<T> data) : count_(data.size()), list_(SLinkedList<T>(data)) {}

	template <size_t N>
	explicit Stack(std::array<T, N> data) : count_(N), list_(SLinkedList<T>(data)) {}

	Stack(std::initializer_list<T> data) : count_(data.size()), list_(SLinkedList<T>(data)) {}

	~Stack() = default;

	Stack(const Stack& other) : count_(other.count_), list_(other.list_) {}

	Stack& operator=(const Stack& other)
	{
		if(this == &other) return *this;

		clear();
		count_ = other.count_;
		list_ = other.list_;
		
		return *this;
	}

	Stack(Stack&& other) noexcept
	{
		count_ = other.count_;
		list_ = other.list_;
	}

	Stack& operator=(Stack&& other) noexcept
	{
		if(this == &other) return *this;

		clear();
		count_ = other.count_;
		list_ = other.list_;

		return *this;
	}

	template <typename U = T, std::enable_if_t<is_printable<U>::value, int> = 0>
	friend std::ostream& operator<<(std::ostream& os, const Stack& stack)
	{
		os << stack.list_;
		return os;
	}

	template <typename U = T, std::enable_if_t<is_equatable<U>::value, int> = 0>
	bool operator==(const Stack& other) const
	{
		if(this == &other) return true;
		return list_ == other.list_;
	}

	T pop()
	{
		if (empty())
		{
			throw std::out_of_range("Stack is empty");
		}
		count_--;
		return list_.pop_front();
	}

	[[nodiscard]] T peek() const
	{
		if (empty())
		{
			throw std::out_of_range("Stack is empty");
		}
		return list_.front();
	}

	void push(const T& data)
	{
		count_++;
		list_.prepend(data);
	}

	void push(T&& data)
	{
		count_++;
		list_.prepend(std::move(data));
	}

	void clear()
	{
		count_ = 0;
		list_.clear();
	}

	[[nodiscard]] size_t size() const
	{
		return count_;
	}

	[[nodiscard]] bool empty() const
	{
		return count_ == 0;
	}

private:
	size_t count_;
	SLinkedList<T> list_;
};


#endif // STACK_H