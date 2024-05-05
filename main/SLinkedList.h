#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <vector>
#include <array>
#include <stdexcept>
#include <string>
#include "TypeTraitsCheck.h"

template <typename T>
class SLinkedList
{
	static_assert(std::is_copy_constructible_v<T>, "Attempted to make a SLinkedList with a type that does not implement a copy constructor");
	static_assert(!std::is_pointer_v<T>, "Attempted to make a SLinkedList with a raw pointer type");
	template <typename Y>
	struct Node
	{
		std::unique_ptr<Y> data;
		Node* next;

		explicit Node(Y data) : data(std::make_unique<Y>(data)), next(nullptr) {}

		explicit Node(Y** data) : data(std::make_unique<Y>(std::move(data))), next(nullptr) {}

		explicit Node(Y data, Node* next) : data(std::make_unique<Y>(data)), next(next) {}

		explicit Node(Y** data, Node* next) : data(std::make_unique<Y>(std::move(data))), next(next) {}
	};

public:

	class Iterator
	{
	public:
		explicit Iterator(Node<T>* node) : node(node) {}

		bool operator!=(const Iterator& other) const
		{
			return node != other.node;
		}

		T& operator*() const
		{
			if(node == nullptr) throw std::out_of_range("Iterator out of bounds");
			return *(node->data);
		}

		Iterator& operator++()
		{
			node = node->next;
			return *this;
		}

		T* operator->() const
		{
			if(node == nullptr) throw std::out_of_range("Iterator out of bounds");
			return node->data;
		}

	private:
		Node<T>* node;
	};

	Iterator begin() const
	{
		return Iterator(head_);
	}

	Iterator end() const
	{
		return Iterator(nullptr);
	}

	SLinkedList() : count_(0), head_(nullptr), tail_(nullptr)  {}

	SLinkedList(T data[], const size_t size) : count_(size)
	{
		head_ = new Node<T>(data[0]);
		Node<T>* current = head_;
		for(size_t i = 1; i < count_; i++)
		{
			current->next = new Node<T>(data[i]);
			current = current->next;
		}
		tail_ = current;
	}

	explicit SLinkedList(std::vector<T> data) : count_(data.size())
	{
		const T* p = data.begin();
		head_ = new Node<T>(p[0]);
		Node<T>* current = head_;
		for(size_t i = 1; i < count_; i++)
		{
			current->next = new Node<T>(std::move(p[i]));
			current = current->next;
		}
		tail_ = current;
	}

	template <size_t N>
	explicit SLinkedList(std::array<T, N> data) : count_(N)
	{
		const T* p = data.begin();
		head_ = new Node<T>(p[0]);
		Node<T>* current = head_;
		for(size_t i = 1; i < N; ++i)
		{
			current->next = new Node<T>(std::move(p[0]));
			current = current->next;
		}
		tail_ = current;
	}

	SLinkedList(std::initializer_list<T> data) : count_(data.size())
	{
		const T* p = data.begin();
		head_ = new Node<T>(p[0]);
		Node<T>* current = head_;
		for(size_t i = 1; i < count_; ++i)
		{
			current->next = new Node<T>(p[i]);
			current = current->next;
		}
		tail_ = current;
	}

	~SLinkedList()
	{
		Node<T>* current = head_;
		while(current != nullptr)
		{
			Node<T>* next = current->next;
			delete current;
			current = next;
		}
	}

	SLinkedList(const SLinkedList& other) : count_(0), head_(nullptr), tail_(nullptr)
	{
		Node<T>* currentOther = new Node<T>(*(other.head_->data));
		while(currentOther != nullptr)
		{
			append(*(currentOther->data));
			currentOther = currentOther->next;
		}
	}

	SLinkedList& operator=(const SLinkedList& other)
	{
		if(this == &other) return *this;

		clear();
		Node<T>* current = new Node<T>(*(other.head_->data));
		while(current != nullptr)
		{
			append(*(current->data));
			current = current->next;
		}

		return *this;
	}

	SLinkedList(SLinkedList&& other) noexcept
	{
		head_ = other.head_;
		tail_ = other.tail_;
		count_ = other.count_;
		other.head_ = nullptr;
		other.tail_ = nullptr;
		other.count_ = 0;
	}

	SLinkedList& operator=(SLinkedList&& other) noexcept
	{
		if(this == &other) return *this;
		
		clear();
		head_ = other.head_;
		tail_ = other.tail_;
		count_ = other.count_;
		other.head_ = nullptr;
		other.tail_ = nullptr;
		other.count_ = 0;

		return *this;
	}

	template <typename U = T, std::enable_if_t<is_printable<U>::value, int> = 0>
	friend std::ostream& operator<<(std::ostream& os, const SLinkedList& list)
	{
		if(list.head_ == nullptr) return os << "{ }" << std::endl;
		os << "{ ";
		Node<T>* current = list.head_;
		while(current != list.tail_)
		{
			os << *(current->data) << ", ";
			current = current->next;
		}
		os << *(list.tail_->data) << " }" << std::endl;
		return os;
	}

	template <typename U = T, std::enable_if_t<is_equatable<U>::value, int> = 0>
	bool operator==(const SLinkedList& other) const
	{
		if(this == &other) return true;
		if(count_ != other.count_) return false;
		Node<T>* current = head_;
		Node<T>* currentOther = other.head_;
		while(current != nullptr)
		{
			if(*(current->data) != *(currentOther->data)) return false;
			current = current->next;
			currentOther = currentOther->next;
		}

		return true;
	}

	T& operator[](const size_t index)
	{
		if(index < count_)
		{
			return *(unsafe_get_pointer_at(index)->data);
		}

		throw std::out_of_range("Index out of bounds\nindex: " + std::to_string(index) + "\nsize: " + std::to_string(count_));
	}

	[[nodiscard]] T at(const size_t index) const
	{
		if(index < count_)
		{
			return *(unsafe_get_pointer_at(index)->data);
		}

		throw std::out_of_range("Index out of bounds\nindex: " + std::to_string(index) + "\nsize: " + std::to_string(count_));
	}

	void append(const T& data) noexcept
	{
		Node<T>* newNode = new Node<T>(data);
		if(head_ == nullptr)
		{
			head_ = newNode;
			tail_ = newNode;
		}
		else
		{
			tail_->next = newNode;
			tail_ = newNode;
		}
		count_++;
	}

	void append(T&& data) noexcept
	{
		Node<T>* newNode = new Node<T>(std::move(data));;
		if(head_ == nullptr)
		{
			head_ = newNode;
			tail_ = newNode;
		}
		else
		{
			tail_->next = newNode;
			tail_ = newNode;
		}
		count_++;
	}

	void prepend(const T& data) noexcept
	{
		Node<T>* newNode = new Node<T>(data);
		if(head_ == nullptr)
		{
			head_ = newNode;
			tail_ = newNode;
		}
		else
		{	newNode->next = head_;
			head_ = newNode;
		}
		count_++;
	}

	void prepend(T&& data) noexcept
	{
		Node<T>* newNode = new Node<T>(std::move(data));
		if(head_ == nullptr)
		{
			head_ = newNode;
			tail_ = newNode;
		}
		else
		{
			newNode->next = head_;
			head_ = newNode;
		}
		count_++;
	}

	[[nodiscard]] T front() const
	{
		if(head_ != nullptr) return *(head_->data);
		throw std::out_of_range("List is empty");
	}

	T pop_front()
	{
		if(head_ != nullptr)
		{
			const Node<T>* temp = head_;
			T res = *(head_->data);
			if(head_ == tail_)
			{
				head_ = nullptr;
				tail_ = nullptr;
			}
			else
				head_ = head_->next;
			delete temp;
			count_--;
			return res;
		}

		throw std::out_of_range("List is empty");
	}

	[[nodiscard]] T back() const
	{
		if(tail_ != nullptr) return *(tail_->data);
		throw std::out_of_range("List is empty");
	}

	T pop_back()
	{
		if(tail_ != nullptr)
		{
			const Node<T>* temp = tail_;
			T res = *(tail_->data);
			if(head_ == tail_)
			{
				head_ = nullptr;
				tail_ = nullptr;
			}
			else
			{
				tail_ = unsafe_get_pointer_at(count_ - 2);
				tail_->next = nullptr;
			}
				
			delete temp;
			count_--;
			return res;
		}

		throw std::out_of_range("List is empty");
	}

	void insert_at(const T& data, const size_t index)
	{
		if(index < count_)
		{
			Node<T>* newNode = new Node<T>(data);
			if(index == 0)
			{
				newNode->next = head_;
				head_ = newNode;
			}
			else
			{
				Node<T>* temp = unsafe_get_pointer_at(index - 1);
				newNode->next = temp->next;
				temp->next = newNode;
			}
			count_++;
		}
		else
			throw std::out_of_range("Index out of bounds\nindex: " + std::to_string(index) + "\nsize: " + std::to_string(count_));
	}

	void insert_at(T&& data, const size_t index)
	{
		if(index < count_)
		{
			Node<T>* newNode = new Node<T>(std::move(data));
			if(index == 0)
			{
				newNode->next = head_;
				head_ = newNode;
			}
			else
			{
				Node<T>* temp = unsafe_get_pointer_at(index - 1);
				newNode->next = temp->next;
				temp->next = newNode;
			}
			count_++;
		}
		else
			throw std::out_of_range("Index out of bounds\nindex: " + std::to_string(index) + "\nsize: " + std::to_string(count_));
	}

	void remove_at(const size_t index)
	{
		if(index < count_)
		{
			if(index == 0)
			{
				if(head_ == tail_)
				{
					delete head_;
					head_ = nullptr;
					tail_ = nullptr;
				}
				else
				{
					const Node<T>* temp = head_;
					head_ = head_->next;
					delete temp;
				}
			}
			else if(index == count_ - 1)
			{
				delete tail_;
				tail_ = unsafe_get_pointer_at(index - 1);
				tail_->next = nullptr;
			}
			else
			{
				Node<T>* temp = unsafe_get_pointer_at(index - 1);
				const Node<T>* remove = temp->next;
				temp->next = remove->next;
				delete remove;
			}
			count_--;
		}
		else
			throw std::out_of_range("Index out of bounds\nindex: " + std::to_string(index) + "\nsize: " + std::to_string(count_));
	}

	void clear() noexcept
	{
		Node<T>* current = head_;
		while(current != nullptr)
		{
			Node<T>* next = current->next;
			delete current;
			current = next;
		}
		head_ = nullptr;
		tail_ = nullptr;
		count_ = 0;
	}

	[[nodiscard]] size_t size() const noexcept
	{
		return count_;
	}

	[[nodiscard]] bool empty() const noexcept
	{
		return count_ == 0;
	}

private:
	size_t count_;
	Node<T>* head_;
	Node<T>* tail_;

	[[nodiscard]] Node<T>* unsafe_get_pointer_at(const size_t index) const
	{
		Node<T>* res = head_;
		size_t pos = 0;
		while(pos != index)
		{
			res = res->next;
			pos++;
		}
		return res;
	}
};

#endif // LINKEDLIST_H