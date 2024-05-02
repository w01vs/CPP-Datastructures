#ifndef HASHSET_H
#define HASHSET_H

#include <string>
#include "TypeTraitsCheck.h"

enum
{
	MAX_LOAD = 70
};

template<typename T, typename U>
class HashMap
{
	static_assert(std::is_default_constructible_v<T>, "Attempted to make a HashMap with a type that does not implement a default constructor");
	static_assert(std::is_copy_constructible_v<T>, "Attempted to make a HashMap with a type that does not implement a copy constructor");
	static_assert(!std::is_pointer_v<T>, "Attempted to make a HashMap with a raw pointer type");
	static_assert(is_equatable<T>::value, "Attempted to make a HashMap with a key type that does not have the '==' operator implemented");
	static_assert(has_std_hash<T>::value, "This key type does not have a specialized std::hash<T> implementation");

	struct Entry
	{
		size_t hash;
		U value;
		T key;
		Entry* next;
	};
public:
	HashMap() : capacity(0), load_factor(0), ptr(new Entry*[16])
	{
		for(int i = 0; i < 16; i++)
			ptr[i] = nullptr;
	}

	~HashMap()
	{
		delete ptr;
	}

	HashMap(const HashMap& other) : capacity(other.capacity), load_factor(other.load_factor)
	{
		ptr = new Entry* [capacity];
		for(int i = 0; i < capacity; i++)
		{
			if(other.ptr[i] != nullptr)	ptr[i] = new Entry(*(other.ptr[i]));
			else ptr[i] = nullptr;
		}
	}

	HashMap& operator=(const HashMap& other)
	{
		if(this == &other) return *this;

		Clear();
		capacity = other.capacity;
		load_factor = other.load_factor;
		ptr = new Entry* [capacity];
		for(int i = 0; i < capacity; i++)
		{
			if(other.ptr[i] != nullptr)	ptr[i] = new Entry(*(other.ptr[i]));
			else ptr[i] = nullptr;
		}

		return *this;
	}

	HashMap(HashMap&& other) : capacity(other.capacity), load_factor(other.load_factor), ptr(other.ptr)
	{
		other.capacity = 0;
		other.load_factor = 0;
		other.ptr = nullptr;
	}

	HashMap& operator=(HashMap&& other)
	{
		if(this == &other) return *this;

		Clear();

		capacity = other.capacity;
		load_factor = other.load_factor;
		ptr = other.ptr;
	}

	U& operator[](const T& key)
	{
		const size_t hash = Hash(key);
		const size_t index = hash % capacity;
		if(ptr[index] == nullptr)
		{
			ptr[index] = new Entry{hash, U(), key};
			return ptr[index]->value;
		}

		Entry* ret = ptr[index];
		while(ret->hash != hash)
			ret = ret->next;

		return ret->value;
	}

	void Insert(T key, U value)
	{
		const size_t hash = Hash(key);
		const size_t index = hash % capacity;

		Entry* entry = ptr[index];
		if(entry == nullptr)
			ptr[index] = new Entry(hash, value, key, nullptr);
		else
			entry->next = new Entry(hash, value, key, nullptr);
	}

	void Remove(const T& key)
	{
		const size_t hash = Hash(key);
		const size_t index = hash % capacity;

		Entry* entry = ptr[index];
		if(entry == nullptr)
			return;
		else
		{
			if(entry.key == key)
			{
				const Entry* temp = entry;
				ptr[index] = entry->next;
				delete temp;
				return;
			}
			Entry* prev = entry;
			entry = entry->next;

			while(entry->key != key)
			{
				prev = entry;
				entry = entry->next;
			}

			prev->next = entry->next;
			delete entry;
		}
	}

	bool ContainsKey(const T& key);

	bool ContainsValue(U value);

	void Clear()
	{
		for(int i = 0; i < capacity; i++)
		{
			delete ptr[i];
		}
		delete[] ptr;
		capacity = 0;
		load_factor = 0;
	}

private:
	size_t capacity;
	float load_factor;
	Entry** ptr;

	size_t Hash(const T& key)
	{
		std::hash<T> hash;
		return hash(key);
	}

	void CalcLoadFac()
	{
		size_t nodes[capacity];
		for(int i = 0; i < capacity; i++)
		{
			size_t count = 0;
			Entry* start = ptr[i];
			while(start->next != nullptr)
			{
				start = start->next;
				count++;
			}

			nodes[i] = count;
		}

		size_t res = 0;
		for(int i = 0; i < capacity; i++)
		{
			res = nodes[i] * nodes[i];
		}

		load_factor = (double)res / capacity;
	}

	void Rehash(); // Check if resize is needed and do so.
};

#endif // HASHSET_H