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
	static_assert(is_equatable<U>::value, "Attempted to make a HashMap with a value type that does not have the '==' operator implemented");
	static_assert(has_std_hash<T>::value, "This key type does not have a specialized std::hash<T> implementation");

	struct Entry
	{
		size_t hash;
		U value;
		T key;
		Entry* next;
	};
public:
	HashMap() : capacity_(0), load_factor_(0), ptr_(new Entry*[16])
	{
		for(int i = 0; i < 16; i++)
			ptr_[i] = nullptr;
	}

	~HashMap()
	{
		delete ptr_;
	}

	HashMap(const HashMap& other) : capacity_(other.capacity_), load_factor_(other.load_factor_)
	{
		ptr_ = new Entry* [capacity_];
		for(int i = 0; i < capacity_; i++)
		{
			if(other.ptr_[i] != nullptr)	ptr_[i] = new Entry(*(other.ptr_[i]));
			else ptr_[i] = nullptr;
		}
	}

	HashMap& operator=(const HashMap& other)
	{
		if(this == &other) return *this;

		clear();
		capacity_ = other.capacity_;
		load_factor_ = other.load_factor_;
		ptr_ = new Entry* [capacity_];
		for(int i = 0; i < capacity_; i++)
		{
			if(other.ptr_[i] != nullptr)	ptr_[i] = new Entry(*(other.ptr_[i]));
			else ptr_[i] = nullptr;
		}

		return *this;
	}

	HashMap(HashMap&& other) : capacity_(other.capacity_), load_factor_(other.load_factor_), ptr_(other.ptr_)
	{
		other.capacity_ = 0;
		other.load_factor_ = 0;
		other.ptr_ = nullptr;
	}

	HashMap& operator=(HashMap&& other)
	{
		if(this == &other) return *this;

		clear();

		capacity_ = other.capacity_;
		load_factor_ = other.load_factor_;
		ptr_ = other.ptr_;
	}

	U& operator[](const T& key)
	{
		const size_t hash = hash(key);
		const size_t index = hash % capacity_;
		if(ptr_[index] == nullptr)
		{
			ptr_[index] = new Entry{hash, U(), key};
			rehash();
			return ptr_[index]->value;
		}

		Entry* ret = ptr_[index];
		while(ret->hash != hash)
			ret = ret->next;

		return ret->value;
	}

	void insert(T key, U value)
	{
		const size_t hash = hash(key);
		const size_t index = hash % capacity_;

		Entry* entry = ptr_[index];
		if(entry == nullptr)
			ptr_[index] = new Entry(hash, value, key, nullptr);
		else
			entry->next = new Entry(hash, value, key, nullptr);

		rehash();
	}

	void remove(const T& key)
	{
		const size_t hash = hash(key);
		const size_t index = hash % capacity_;

		Entry* entry = ptr_[index];
		if(entry == nullptr)
			return;
		else
		{
			if(entry.key == key)
			{
				const Entry* temp = entry;
				ptr_[index] = entry->next;
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

		rehash();
	}

	bool contains_key(const T& key)
	{
		const size_t hash = hash(key);
		const size_t index = hash % capacity_;

		Entry* entry = ptr_[index];

		while(entry != nullptr)
		{
			if(entry->key == key) return true;
			entry = entry->next;
		}

		return false;
	}

	bool contains_value(U value)
	{
		if(load_factor_ <= 0) return false;

		for(int i = 0; i < capacity_; i++)
		{
			Entry* entry = ptr_[i];
			while(entry != nullptr)
			{
				if(entry->value == value) return true;
				entry = entry->next;
			}
		}
		
		return false;
	}

	void clear()
	{
		for(int i = 0; i < capacity_; i++)
		{
			delete ptr_[i];
		}
		delete[] ptr_;
		capacity_ = 0;
		load_factor_ = 0;
	}

private:
	size_t capacity_;
	float load_factor_;
	Entry** ptr_;

	size_t hash(const T& key)
	{
		std::hash<T> hash;
		return hash(key);
	}

	void calc_load_fac()
	{
		size_t nodes[capacity_];
		for(int i = 0; i < capacity_; i++)
		{
			size_t count = 0;
			Entry* start = ptr_[i];
			while(start->next != nullptr)
			{
				start = start->next;
				count++;
			}

			nodes[i] = count;
		}

		size_t res = 0;
		for(int i = 0; i < capacity_; i++)
		{
			res = nodes[i] * nodes[i];
		}

		load_factor_ = (double)res / capacity_;
	}

	void rehash(); // Check if resize is needed and do so.
};

#endif // HASHSET_H