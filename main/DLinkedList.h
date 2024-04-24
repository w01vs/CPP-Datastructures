#ifndef DOUBLY_LINKED_LIST_H
#define DOUBLY_LINKED_LIST_H

#include <vector>
#include <array>
#include <stdexcept>
#include "TypeTraitsCheck.h"
#include <string>

template <typename T>
class DLinkedList
{
	static_assert(std::is_copy_constructible_v<T>, "Attempted to make a DLinkedList with a type that does not implement a copy constructor");
	static_assert(!std::is_pointer_v<T>, "Attempted to make a DLinkedList with a raw pointer type");
	template <typename Y>
	struct Node
	{
		std::unique_ptr<Y> data;
		Node* next;
		Node* prev;

		explicit Node(Y data) : data(std::make_unique<Y>(data)), next(nullptr), prev(nullptr) {}

		explicit Node(Y** data) : data(std::make_unique<Y>(std::move(data))), next(nullptr), prev(nullptr) {}

		explicit Node(Y data, Node* next, Node* prev) : data(std::make_unique<Y>(data)), next(next), prev(prev) {}

		explicit Node(Y** data, Node* next, Node* prev) : data(std::make_unique<Y>(std::move(data))), next(next), prev(prev) {}
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

		Iterator& operator--()
		{
			node = node->prev;
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

	DLinkedList() : count(0), head(nullptr), tail(nullptr)  {}

	explicit DLinkedList(T data[], const size_t size) : count(size)
	{
		head = new Node<T>(data[0]);
		Node<T>* current = head;
		for(size_t i = 0; i < count; i++)
		{
			current->next = new Node<T>(data[i], nullptr, current);
			current = current->next;
		}
		tail = current;
	}

	explicit DLinkedList(std::vector<T> data) : count(data.size())
	{
		const T* p = data.data();
		head = new Node<T>(p[0]);
		Node<T>* current = head;
		for(size_t i = 0; i < count; i++)
		{
			current->next = new Node<T>(std::move(p[i]), nullptr, current);
			current = current->next;
		}
		tail = current;
	}

	template <size_t N>
	explicit DLinkedList(std::array<T, N> data) : count(data.size())
	{
		T* p = data.data();
		head = new Node<T>(p[0]);
		Node<T>* current = head;
		for(size_t i = 0; i < count; i++)
		{
			current->next = new Node<T>(std::move(p[i]), nullptr, current);
			current = current->next;
		}
		tail = current;
	}

	explicit DLinkedList(std::initializer_list<T> data) : count(data.size())
	{
		const T* p = data.begin();
		head = new Node<T>(p[0]);
		Node<T>* current = head;
		for(size_t i = 1; i < count; i++)
		{
			current->next = new Node<T>(p[i], nullptr, current);
			current = current->next;
		}
		tail = current;
	}

	~DLinkedList()
	{
		Node<T>* current = head;
		while(current != nullptr)
		{
			Node<T>* next = current->next;
			delete current;
			current = next;
		}
	}

	DLinkedList(const DLinkedList& other) : count(other.count), head(nullptr), tail(nullptr)
	{
		Node<T>* current = new Node<T>(*(other.head->data));

		while(current != nullptr)
		{
			Append(*(current->data));
			current = current->next;
		}
	}

	DLinkedList& operator=(const DLinkedList& other)
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

	DLinkedList(DLinkedList&& other) noexcept
	{
		head = other.head;
		tail = other.tail;
		count = other.count;
		other.count = 0;
		other.head = nullptr;
		other.tail = nullptr;
	}

	DLinkedList& operator=(DLinkedList&& other) noexcept
	{
		if(this == other) return *this;

		Clear();

		head = other.head;
		tail = other.tail;
		count = other.count;
		other.count = 0;
		other.head = nullptr;
		other.tail = nullptr;

		return *this;
	}

	template <typename U = T, std::enable_if_t<is_printable<U>::value, int> = 0>
	friend std::ostream& operator<<(std::ostream& os, const DLinkedList& list)
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
	bool operator==(const DLinkedList& other) const
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
		Node<T>* newNode = new Node<T>(data);
		if(head == nullptr)
		{
			head = newNode;
			tail = newNode;
		}
		else
		{
			tail->next = newNode;
			newNode->prev = tail;
			tail = newNode;
		}
		count++;
	}

	void Append(const T&& data) noexcept
	{
		Node<T>* newNode = new Node<T>(std::move(data));
		if(head == nullptr)
		{
			head = newNode;
			tail = newNode;
		}
		else
		{
			tail->next = newNode;
			newNode->prev = tail;
			tail = newNode;
		}
		count++;
	}

	void Prepend(const T& data) noexcept
	{
		Node<T>* newNode = new Node<T>(data, nullptr, head);
		if(head == nullptr)
		{
			head = newNode;
			tail = newNode;
		}
		{
			head->prev = newNode;
			newNode->next = head;
			head = newNode;
		}
		count++;
	}

	void Prepend(T&& data) noexcept
	{
		Node<T>* newNode = new Node<T>(std::move(data), nullptr, head);
		if(head == nullptr)
		{
			head = newNode;
			tail = newNode;
		}
		{
			head->prev = newNode;
			newNode->next = head;
			head = newNode;
		}
		count++;
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
			{
				head = head->next;
				head->prev = nullptr;
			}
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

	void InstertAt(const T& data, const size_t index)
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
				temp->next->prev = newNode;
				temp->next = newNode;
				newNode->prev = temp;
				
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
				temp->next->prev = newNode;
				temp->next = newNode;
				newNode->prev = temp;
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
					head->prev = nullptr;
					delete temp;
				}
			}
			else if(index == count - 1)
			{
				Node<T>* temp = tail->prev;
				delete tail;
				tail = temp;
				tail->next = nullptr;
			}
			else
			{
				const Node<T>* remove = UnsafeGetPointerAt(index);
				remove->prev->next = remove->next;
				remove->next->prev = remove->prev;
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

	Node<T>* UnsafeGetPointerAt(const size_t index) const
	{
		Node<T>* res;
		size_t pos;
		if(index <= count / 2)
		{
			pos = 0;
			res = head;
			while(pos != index)
			{
				res = res->next;
				pos++;
			}
		}
		else
		{
			pos = count - 1;
			res = tail;
			while(pos != index)
			{
				res = res->prev;
				pos--;
			}
		}

		return res;
	}
};

#endif // DOUBLY_LINKED_LIST_H