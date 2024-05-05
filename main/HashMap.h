#ifndef HASHSET_H
#define HASHSET_H

#include <string>
#include "TypeTraitsCheck.h"

enum
{
	MAX_LOAD = 70,
	NEW_LOAD = 30
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
		for(int i = 0; i < capacity_; i++)
		{
			delete ptr_[i];
		}
		delete[] ptr_;
	}

	HashMap(const HashMap& other) : capacity_(other.capacity_), load_factor_(other.load_factor_), count_(other.count_)
	{
		ptr_ = new Entry* [capacity_];
		for(int i = 0; i < capacity_; i++)
		{
			if(other.ptr_[i] != nullptr)	
				ptr_[i] = new Entry(*(other.ptr_[i]));
			else 
				ptr_[i] = nullptr;
		}
	}

	HashMap& operator=(const HashMap& other)
	{
		if(this == &other) return *this;

		clear();
		capacity_ = other.capacity_;
		load_factor_ = other.load_factor_;
		count_ = other.count_;
		ptr_ = new Entry* [capacity_];
		for(int i = 0; i < capacity_; i++)
		{
			if(other.ptr_[i] != nullptr)	
				ptr_[i] = new Entry(*(other.ptr_[i]));
			else 
				ptr_[i] = nullptr;
		}

		return *this;
	}

	HashMap(HashMap&& other) : capacity_(other.capacity_), load_factor_(other.load_factor_), ptr_(other.ptr_), count_(other.count_)
	{
		other.capacity_ = 0;
		other.load_factor_ = 0;
		other.count_ = 0;
		other.ptr_ = nullptr;
	}

	HashMap& operator=(HashMap&& other)
	{
		if(this == &other) return *this;

		clear();

		capacity_ = other.capacity_;
		load_factor_ = other.load_factor_;
		ptr_ = other.ptr_;
		count_ = other.count_;

		other.capacity_ = 0;
		other.load_factor_ = 0;
		other.ptr_ = nullptr;
		other.count_ = 0;
	}

	U& operator[](const T& key)
	{
		const size_t hash_value = hash(key);
		const size_t index = hash_value % capacity_;
		if(ptr_[index] == nullptr)
		{
			ptr_[index] = new Entry{hash_value, U(), key};
			count_++;
			rehash();
			return ptr_[index]->value;
		}

		Entry* ret = ptr_[index];
		while(ret->hash != hash_value)
			ret = ret->next;

		return ret->value;
	}

	void insert(T key, U value)
	{
		const size_t hash_value = hash(key);
		const size_t index = hash_value % capacity_;

		Entry* entry = ptr_[index];
		if(entry == nullptr)
			ptr_[index] = new Entry(hash_value, value, key, nullptr);
		else
			entry->next = new Entry(hash_value, value, key, nullptr);

		count_++;
		rehash();
	}

	void remove(const T& key)
	{
		const size_t hash_value = hash(key);
		const size_t index = hash_value % capacity_;

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
				count_--;
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
			count_--;
			delete entry;
		}

		rehash();
	}

	bool contains_key(const T& key)
	{
		const size_t hash_value = hash(key);
		const size_t index = hash_value % capacity_;

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
		count_ = 0;
		ptr_ = new Entry*[1];
	}

private:
	size_t capacity_;
	float load_factor_;
	Entry** ptr_;
	size_t count_;

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

	void rehash()
	{
		calc_load_fac();
		if(load_factor_ >= (float)MAX_LOAD / 100)
		{
			// new capacity -> calculated taking average amount of entries per index.
			// even though load factor is calculated taking the average amount of entries every entry has in its index.
			size_t new_capacity = count_ / ((float)NEW_LOAD / 100) * 100;
			Entry** temp = new Entry * [capacity_];
			for(int i = 0; i < capacity_; i++)
				temp[i] = ptr_[i];

			delete[] ptr_;
		}
		calc_load_fac();
	}
};

#endif // HASHSET_H