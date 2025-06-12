#pragma once
#ifndef VECTOR_H
#define VECTOR_H
#include<iostream>
#include<utility>  // For std::pair
using namespace std;
template <typename T>
class Vector {
private:
	T* arr;
	int len;
	int cap;

	// Doubles the current capacity when needed
	void double_capacity()
	{
		T* temp = new T[cap];

		for (int i = 0; i < len; i++)
		{
			temp[i] = arr[i];
		}

		cap = cap * 2;
		delete[] arr;
		arr = new T[cap];
		for (int i = 0; i < len; i++)
		{
			arr[i] = temp[i];
		}
		delete[] temp;
	}

public:
	// Constructors & Destructor
	Vector()                           // Default constructor
	{
		arr = nullptr;
		len = 0;
		cap = 0;
	}
	Vector(int count, T value)      // Fill constructor
	{
		arr = new T[count];
		for (int i = 0; i < count; i++)
		{
			arr[i] = value;
		}
		len = count;
		cap = len + 1;
	}
	Vector(const Vector& other) // Copy constructor
	{
		if (this != &other)
		{
			arr = nullptr;
			len = 0;
			cap = 0;
			len = other.len;
			cap = other.cap;
			arr = new T[cap];
			for (int i = 0; i < len; i++)
			{
				arr[i] = other.arr[i];
			}
		}
	}
	virtual ~Vector()                          // Destructor
	{
		delete[] arr;
	}
	// Operators
	Vector& operator=(const Vector& other) // Copy assignment
	{
		if (this != &other)
		{
			delete[] arr;
			len = other.len;
			cap = other.cap;
			arr = new T[cap];
			for (int i = 0; i < len; i++)
			{
				arr[i] = other.arr[i];
			}
		}
		return *this;
	}
	T& operator[](int index)                          // Subscript operator
	{
		return arr[index];
	}
	const T& operator[](int index) const            // Const subscript operator
	{
		return arr[index];
	}
	bool operator==(const Vector& other) const // Equality comparison
	{

		bool flag = false;
		for (int i = 0; i < len; i++)
		{
			flag = false;
			for (int j = 0; j < other.len; j++)
			{
				if (arr[i] == other.arr[j])
				{
					flag = true;
				}
			}
			if (!flag)
			{
				return false;
			}
		}
		return true;
	}
	bool operator!=(const Vector& other) const // Inequality comparison
	{
		bool flag = false;
		for (int i = 0; i < len; i++)
		{
			flag = false;
			for (int j = 0; j < other.len; j++)
			{
				if (arr[i] == other.arr[j])
				{
					flag = true;
				}
			}
			if (flag)
			{
				return false;
			}
		}
		return true;
	}
	Vector operator+(const Vector& other) const // Concatenation
	{
		Vector res;
		res.len = len + other.len;
		res.cap = res.len + 1;
		res.arr = new T[res.cap];
		for (int i = 0; i < len; i++)
		{
			res.arr[i] = arr[i];
		}
		for (int i = 0; i < other.len; i++)
		{
			res.arr[i + len] = other.arr[i];
		}
		return res;
	}
	Vector& operator+=(const Vector& other)     // Append
	{
		Vector res;
		res.len = len + other.len;
		res.cap = res.len + 1;
		res.arr = new T[res.cap];
		for (int i = 0; i < len; i++)
		{
			res.arr[i] = arr[i];
		}
		for (int i = 0; i < other.len; i++)
		{
			res.arr[i + len] = other.arr[i];
		}
		*this = res;
		return *this;
	}
	// Modifiers
	void push_back(T value)                 // Adds an element to the end
	{
		if (len == cap)
		{
			if (cap == 0) cap = 1;
			double_capacity();
		}
		arr[len++] = value;
	}
	void push_back_pair(const T& first, const T& second)  // Adds a pair as two separate elements
	{
		push_back(first);
		push_back(second);
	}
	void pop()         // Removes the last element
	{
		if (len > 0)
		{
			--len;
		}
	}
	pair<T, T> pop_back_pair()    // Removes and returns the last two elements as a pair
	{
		if (len < 2)
		{
			throw std::runtime_error("Vector must have at least 2 elements to pop a pair");
		}

		T second = arr[len - 1];  // Get last element
		T first = arr[len - 2];   // Get second-to-last element

		len -= 2;  // Remove both elements

		return make_pair(first, second);
	}
	void insert_at(int index, T value) // Inserts value at index
	{
		if (len == cap)
		{
			if (cap == 0) cap = 1;
			double_capacity();
		}
		if (index < 0 || index > len)
		{
			return;
		}
		for (int i = len; i > index; i--)
		{
			arr[i] = arr[i - 1];
		}
		arr[index] = value;
		len++;
	}
	void delete_at(int index)            // Deletes value at index
	{
		if (index < 0 || index >= len)
		{
			return;
		}
		for (int i = index; i < len - 1; i++)
		{
			arr[i] = arr[i + 1];
		}
		len--;
	}
	void reserve(int new_capacity)       // Reserves a new capacity
	{
		if (new_capacity <= cap)
		{
			return;
		}
		T* temp = new T[new_capacity];
		for (int i = 0; i < len; i++)
		{
			temp[i] = arr[i];
		}
		delete[] arr;
		arr = temp;
		cap = new_capacity;
	}
	void clear()                         // Clears all elements
	{
		delete[] arr;
		arr = nullptr;
		len = 0;
		cap = 0;
	}
	void swap(Vector& other)      // Swaps with another array
	{
		T* temp_arr = arr;
		int temp_len = len;
		int temp_cap = cap;

		arr = other.arr;
		len = other.len;
		cap = other.cap;

		other.arr = temp_arr;
		other.len = temp_len;
		other.cap = temp_cap;
	}
	void shrink_to_fit()                 // Reduces capacity to match size
	{
		if (cap > len)
		{
			T* temp = new T[len];
			for (int i = 0; i < len; i++)
			{
				temp[i] = arr[i];
			}
			delete[] arr;
			arr = temp;
			cap = len;
		}
	}
	void reverse()                       // Reverses the array elements
	{
		for (int i = 0; i < len / 2; i++)
		{
			T temp = arr[i];
			arr[i] = arr[len - 1 - i];
			arr[len - 1 - i] = temp;
		}
	}
	// Accessors
	bool empty() const // Checks if array is empty
	{
		return (arr == nullptr || len == 0);
	}
	int size() const   // Returns current size
	{
		return len;
	}
	T& at(int index) // Bounds-checked element access
	{
		if (index < 0 || index >= len)
		{
			throw std::out_of_range("Index out of range");
		}
		return arr[index];
	}
	const T& at(int index) const
	{
		if (index < 0 || index >= len)
		{
			throw std::out_of_range("Index out of range");
		}
		return arr[index];
	}
	T& front() // Returns first element
	{
		return arr[0];
	}
	const T& front() const
	{
		return arr[0];
	}
	T& back() // Returns last element
	{
		return arr[len - 1];
	}
	const T& back() const
	{
		return arr[len - 1];
	}
	int find(T value) const
	{
		for (int i = 0; i < len; i++)
		{
			if (arr[i] == value)
			{
				return i;
			}
		}
		return -1;
	}

	// Returns a pair containing the minimum and maximum elements
	pair<T, T> min_max_pair() const
	{
		if (len == 0)
		{
			throw std::runtime_error("Vector is empty");
		}

		T min_val = arr[0];
		T max_val = arr[0];

		for (int i = 1; i < len; i++)
		{
			if (arr[i] < min_val)
			{
				min_val = arr[i];
			}
			if (arr[i] > max_val)
			{
				max_val = arr[i];
			}
		}

		return make_pair(min_val, max_val);
	}

	// Return index of first instance, or -1 if non-existent*/
	void print() const {
		cout << "Array elements: ";
		for (int i = 0; i < this->len; ++i) {
			cout << this->arr[i] << " ";
		}
		cout << endl;
	}
};

#endif // VECTOR_H