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
		return Iterator(head);
	}

	Iterator end() const
	{
		return Iterator(nullptr);
	}

	SLinkedList() : count(0), head(nullptr), tail(nullptr)  {}

	SLinkedList(T data[], const size_t size) : count(size)
	{
		head = new Node<T>(data[0]);
		Node<T>* current = head;
		for(size_t i = 1; i < count; i++)
		{
			current->next = new Node<T>(data[i]);
			current = current->next;
		}
		tail = current;
	}

	explicit SLinkedList(std::vector<T> data) : count(data.Size())
	{
		const T* p = data.begin();
		head = new Node<T>(p[0]);
		Node<T>* current = head;
		for(size_t i = 1; i < count; i++)
		{
			current->next = new Node<T>(std::move(p[i]));
			current = current->next;
		}
		tail = current;
	}

	template <size_t N>
	explicit SLinkedList(std::array<T, N> data) : count(N)
	{
		const T* p = data.begin();
		head = new Node<T>(p[0]);
		Node<T>* current = head;
		for(size_t i = 1; i < N; ++i)
		{
			current->next = new Node<T>(std::move(p[0]));
			current = current->next;
		}
		tail = current;
	}

	SLinkedList(std::initializer_list<T> data) : count(data.size())
	{
		const T* p = data.begin();
		head = new Node<T>(p[0]);
		Node<T>* current = head;
		for(size_t i = 0; i < count; ++i)
		{
			current->next = new Node<T>(data[i]);
			current = current->next;
		}
		tail = current;
	}

	~SLinkedList()
	{
		Node<T>* current = head;
		while(current != nullptr)
		{
			Node<T>* next = current->next;
			delete current;
			current = next;
		}
	}

	SLinkedList(const SLinkedList& other) : count(0), head(nullptr), tail(nullptr)
	{
		Node<T>* currentOther = new Node<T>(*(other.head->data));
		while(currentOther != nullptr)
		{
			Append(*(currentOther->data));
			currentOther = currentOther->next;
		}
	}

	SLinkedList& operator=(const SLinkedList& other)
	{
		if(this == &other) return *this;

		Clear();
		Node<T>* current = new Node<T>(*(other.head->data));
		while(current != nullptr)
		{
			Append(*(current->data));
			current = current->next;
		}

		return *this;
	}

	SLinkedList(SLinkedList&& other) noexcept
	{
		head = other.head;
		tail = other.tail;
		count = other.count;
		other.head = nullptr;
		other.tail = nullptr;
		other.count = 0;
	}

	SLinkedList& operator=(SLinkedList&& other) noexcept
	{
		if(this == &other) return *this;
		
		Clear();
		head = other.head;
		tail = other.tail;
		count = other.count;
		other.head = nullptr;
		other.tail = nullptr;
		other.count = 0;

		return *this;
	}

	template <typename U = T, std::enable_if_t<is_printable<U>::value, int> = 0>
	friend std::ostream& operator<<(std::ostream& os, const SLinkedList& list)
	{
		if(list.head == nullptr) return os << "{ }" << std::endl;
		os << "{ ";
		Node<T>* current = list.head;
		while(current != list.tail)
		{
			os << *(current->data) << ", ";
			current = current->next;
		}
		os << *(list.tail->data) << " }" << std::endl;
		return os;
	}

	template <typename U = T, std::enable_if_t<is_equatable<U>::value, int> = 0>
	bool operator==(const SLinkedList& other) const
	{
		if(this == &other) return true;
		if(count != other.count) return false;
		Node<T>* current = head;
		Node<T>* currentOther = other.head;
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
		if(index < count)
		{
			return *(UnsafeGetPointerAt(index)->data);
		}

		throw std::out_of_range("Index out of bounds\nindex: " + std::to_string(index) + "\nsize: " + std::to_string(count));
	}

	[[nodiscard]] T At(const size_t index) const
	{
		if(index < count)
		{
			return *(UnsafeGetPointerAt(index)->data);
		}

		throw std::out_of_range("Index out of bounds\nindex: " + std::to_string(index) + "\nsize: " + std::to_string(count));
	}

	void Append(const T& data) noexcept
	{
		T* newNode = new Node<T>(data);
		if(head == nullptr)
		{
			head = newNode;
			tail = newNode;
		}
		else
		{
			tail->next = newNode;
			tail = newNode;
		}
		count++;
	}

	void Append(T&& data) noexcept
	{
		T* newNode = new Node<T>(std::move(data));;
		if(head == nullptr)
		{
			head = newNode;
			tail = newNode;
		}
		else
		{
			tail->next = newNode;
			tail = newNode;
		}
		count++;
	}

	void Prepend(const T& data) noexcept
	{
		Node<T>* newNode = new Node<T>(data, head);
		head = newNode;
		count++;
		if(count == 1) tail = head;
	}

	void Prepend(T&& data) noexcept
	{
		Node<T>* newNode = new Node<T>(std::move(data), head);
		head = newNode;
		count++;
		if(count == 1) tail = head;
	}

	[[nodiscard]] T Front() const
	{
		if(head != nullptr) return *(head->data);
		throw std::out_of_range("List is Empty");
	}

	T PopFront()
	{
		if(head != nullptr)
		{
			const Node<T>* temp = head;
			T res = *(head->data);
			if(head == tail)
			{
				head = nullptr;
				tail = nullptr;
			}
			else
				head = head->next;
			delete temp;
			count--;
			return res;
		}

		throw std::out_of_range("List is Empty");
	}

	[[nodiscard]] T Back() const
	{
		if(tail != nullptr) return *(tail->data);
		throw std::out_of_range("List is Empty");
	}

	T PopBack()
	{
		if(tail != nullptr)
		{
			const Node<T>* temp = tail;
			T res = *(tail->data);
			if(head == tail)
			{
				head = nullptr;
				tail = nullptr;
			}
			else
			{
				tail = UnsafeGetPointerAt(count - 2);
				tail->next = nullptr;
			}
				
			delete temp;
			count--;
			return res;
		}

		throw std::out_of_range("List is Empty");
	}

	void InsertAt(const T& data, const size_t index)
	{
		if(index < count)
		{
			Node<T>* newNode = new Node<T>(data);
			if(index == 0)
			{
				newNode->next = head;
				head = newNode;
			}
			else
			{
				Node<T>* temp = UnsafeGetPointerAt(index - 1);
				newNode->next = temp->next;
				temp->next = newNode;
			}
			count++;
		}
		else
			throw std::out_of_range("Index out of bounds\nindex: " + std::to_string(index) + "\nsize: " + std::to_string(count));
	}

	void InsertAt(T&& data, const size_t index)
	{
		if(index < count)
		{
			Node<T>* newNode = new Node<T>(std::move(data));
			if(index == 0)
			{
				newNode->next = head;
				head = newNode;
			}
			else
			{
				Node<T>* temp = UnsafeGetPointerAt(index - 1);
				newNode->next = temp->next;
				temp->next = newNode;
			}
			count++;
		}
		else
			throw std::out_of_range("Index out of bounds\nindex: " + std::to_string(index) + "\nsize: " + std::to_string(count));
	}

	void RemoveAt(const size_t index)
	{
		if(index < count)
		{
			if(index == 0)
			{
				if(head == tail)
				{
					delete head;
					head = nullptr;
					tail = nullptr;
				}
				else
				{
					const Node<T>* temp = head;
					head = head->next;
					delete temp;
				}
			}
			else if(index == count - 1)
			{
				delete tail;
				tail = UnsafeGetPointerAt(index - 1);
				tail->next = nullptr;
			}
			else
			{
				Node<T>* temp = UnsafeGetPointerAt(index - 1);
				const Node<T>* remove = temp->next;
				temp->next = remove->next;
				delete remove;
			}
			count--;
		}
		else
			throw std::out_of_range("Index out of bounds\nindex: " + std::to_string(index) + "\nsize: " + std::to_string(count));
	}

	void Clear() noexcept
	{
		Node<T>* current = head;
		while(current != nullptr)
		{
			Node<T>* next = current->next;
			delete current;
			current = next;
		}
		head = nullptr;
		tail = nullptr;
		count = 0;
	}

	[[nodiscard]] size_t Size() const noexcept
	{
		return count;
	}

	[[nodiscard]] bool Empty() const noexcept
	{
		return count == 0;
	}

private:
	size_t count;
	Node<T>* head;
	Node<T>* tail;

	[[nodiscard]] Node<T>* UnsafeGetPointerAt(const size_t index) const
	{
		Node<T>* res = head;
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