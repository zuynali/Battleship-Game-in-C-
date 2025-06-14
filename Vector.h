#pragma once
#ifndef VECTOR_H
#define VECTOR_H
#include<iostream>
#include<vector>       
#include<utility>      
#include<initializer_list>
#include<stdexcept>   
using namespace std;

template <typename T>
class Vector {
private:
	T* arr;
	int len;
	int cap;
	vector<pair<int, int>> coordinates; 
	// Doubles the current capacity when needed
	void double_capacity()
	{
		int new_cap = cap * 2;
		T* temp = new T[new_cap];

		for (int i = 0; i < len; i++)
		{
			temp[i] = arr[i];
		}

		delete[] arr;
		arr = temp;
		cap = new_cap;
	}

public:
	// Iterator type definitions for range-based for loops
	typedef T* iterator;
	typedef const T* const_iterator;

	// Constructors & Destructor
	Vector()                           // Default constructor
	{
		arr = nullptr;
		len = 0;
		cap = 0;
	}

	Vector(int count, T value)      // Fill constructor
	{
		if (count < 0) count = 0;
		len = count;
		cap = count > 0 ? count : 1;
		arr = new T[cap];
		for (int i = 0; i < count; i++)
		{
			arr[i] = value;
		}
	}

	Vector(const Vector& other) // Copy constructor
	{
		len = other.len;
		cap = other.cap;
		if (cap > 0) {
			arr = new T[cap];
			for (int i = 0; i < len; i++)
			{
				arr[i] = other.arr[i];
			}
		}
		else {
			arr = nullptr;
		}
		coordinates = other.coordinates;
	}

	// Initializer list constructor - enables {value1, value2, value3} syntax
	Vector(std::initializer_list<T> init_list)
	{
		len = init_list.size();
		cap = len > 0 ? len : 1;
		arr = new T[cap];

		int index = 0;
		for (const auto& value : init_list)
		{
			arr[index++] = value;
		}
	}

	virtual ~Vector()                          // Destructor
	{
		delete[] arr;
	}

	// Iterator functions for range-based for loops and STL compatibility
	iterator begin() { return arr; }
	const_iterator begin() const { return arr; }
	iterator end() { return arr + len; }
	const_iterator end() const { return arr + len; }

	// C++11 style const iterators
	const_iterator cbegin() const { return arr; }
	const_iterator cend() const { return arr + len; }

	// Operators
	Vector& operator=(const Vector& other) // Copy assignment
	{
		if (this != &other)
		{
			delete[] arr;
			len = other.len;
			cap = other.cap;
			if (cap > 0) {
				arr = new T[cap];
				for (int i = 0; i < len; i++)
				{
					arr[i] = other.arr[i];
				}
			}
			else {
				arr = nullptr;
			}
			coordinates = other.coordinates;
		}
		return *this;
	}

	// Initializer list assignment operator
	Vector& operator=(std::initializer_list<T> init_list)
	{
		delete[] arr;
		len = init_list.size();
		cap = len > 0 ? len : 1;
		arr = new T[cap];

		int index = 0;
		for (const auto& value : init_list)
		{
			arr[index++] = value;
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

	// Fixed equality comparison - checks if vectors have same elements in same order
	bool operator==(const Vector& other) const
	{
		if (len != other.len) {
			return false;
		}

		for (int i = 0; i < len; i++) {
			if (arr[i] != other.arr[i]) {
				return false;
			}
		}
		return true;
	}

	bool operator!=(const Vector& other) const
	{
		return !(*this == other);
	}

	Vector operator+(const Vector& other) const // Concatenation
	{
		Vector res;
		res.len = len + other.len;
		res.cap = res.len > 0 ? res.len : 1;
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
		int old_len = len;
		len = len + other.len;

		if (len > cap) {
			// Need to expand capacity
			cap = len;
			T* temp = new T[cap];
			for (int i = 0; i < old_len; i++) {
				temp[i] = arr[i];
			}
			delete[] arr;
			arr = temp;
		}

		for (int i = 0; i < other.len; i++) {
			arr[old_len + i] = other.arr[i];
		}

		return *this;
	}

	// Modifiers
	void push_back(T value)                 // Adds an element to the end
	{
		if (len == cap)
		{
			if (cap == 0) cap = 1;
			else cap = cap * 2;

			T* temp = new T[cap];
			for (int i = 0; i < len; i++) {
				temp[i] = arr[i];
			}
			delete[] arr;
			arr = temp;
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
		if (index < 0 || index > len)
		{
			return;
		}

		if (len == cap)
		{
			if (cap == 0) cap = 1;
			else cap = cap * 2;

			T* temp = new T[cap];
			for (int i = 0; i < len; i++) {
				temp[i] = arr[i];
			}
			delete[] arr;
			arr = temp;
		}

		for (int i = len; i > index; i--)
		{
			arr[i] = arr[i - 1];
		}
		arr[index] = value;
		len++;
	}

	// STL-style insert function
	iterator insert(iterator pos, const T& value)
	{
		int index = pos - arr;
		insert_at(index, value);
		return arr + index;
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

	// Fixed erase function
	iterator erase(iterator pos)
	{
		int index = pos - arr;
		if (index < 0 || index >= len)
		{
			return end();
		}

		delete_at(index);

		// Return iterator to the next element, or end() if we erased the last element
		if (index >= len) {
			return end();
		}
		return arr + index;
	}

	// Fixed range erase function
	iterator erase(iterator first, iterator last)
	{
		int start_index = first - arr;
		int end_index = last - arr;

		if (start_index < 0 || end_index > len || start_index >= end_index)
		{
			return end();
		}

		int elements_to_remove = end_index - start_index;

		// Shift elements left
		for (int i = start_index; i < len - elements_to_remove; i++)
		{
			arr[i] = arr[i + elements_to_remove];
		}

		len -= elements_to_remove;

		// Return iterator to the element that's now at the start position
		if (start_index >= len) {
			return end();
		}
		return arr + start_index;
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

	// Resize function - perfect for grids and dynamic arrays
	void resize(int new_size)
	{
		if (new_size < 0)
		{
			return;
		}

		if (new_size <= len)
		{
			// Shrinking - just update length
			len = new_size;
		}
		else
		{
			// Growing - need to add elements
			if (new_size > cap)
			{
				// Need more capacity
				int new_cap = new_size * 2; // Give some extra space
				T* temp = new T[new_cap];
				for (int i = 0; i < len; i++)
				{
					temp[i] = arr[i];
				}
				delete[] arr;
				arr = temp;
				cap = new_cap;
			}

			// Initialize new elements with default constructor
			for (int i = len; i < new_size; i++)
			{
				arr[i] = T();
			}
			len = new_size;
		}
	}

	// Resize with fill value - useful for grids
	void resize(int new_size, const T& fill_value)
	{
		if (new_size < 0)
		{
			return;
		}

		if (new_size <= len)
		{
			// Shrinking - just update length
			len = new_size;
		}
		else
		{
			// Growing - need to add elements
			if (new_size > cap)
			{
				// Need more capacity
				int new_cap = new_size * 2; // Give some extra space
				T* temp = new T[new_cap];
				for (int i = 0; i < len; i++)
				{
					temp[i] = arr[i];
				}
				delete[] arr;
				arr = temp;
				cap = new_cap;
			}

			// Initialize new elements with fill value
			for (int i = len; i < new_size; i++)
			{
				arr[i] = fill_value;
			}
			len = new_size;
		}
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

		// Also swap coordinates
		coordinates.swap(other.coordinates);
	}

	void shrink_to_fit()                 // Reduces capacity to match size
	{
		if (cap > len && len > 0)
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
		else if (len == 0) {
			delete[] arr;
			arr = nullptr;
			cap = 0;
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
		return (len == 0);
	}

	int size() const   // Returns current size
	{
		return len;
	}

	int capacity() const   // Returns current capacity
	{
		return cap;
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
		if (len == 0) {
			throw std::runtime_error("Vector is empty");
		}
		return arr[0];
	}

	const T& front() const
	{
		if (len == 0) {
			throw std::runtime_error("Vector is empty");
		}
		return arr[0];
	}

	T& back() // Returns last element
	{
		if (len == 0) {
			throw std::runtime_error("Vector is empty");
		}
		return arr[len - 1];
	}

	const T& back() const
	{
		if (len == 0) {
			throw std::runtime_error("Vector is empty");
		}
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

	// Print function
	void print() const {
		cout << "Array elements: ";
		for (int i = 0; i < this->len; ++i) {
			cout << this->arr[i] << " ";
		}
		cout << endl;
	}

	// Grid-specific utility functions
	void print_as_grid(int rows, int cols) const {
		if (rows * cols != len) {
			cout << "Error: Grid dimensions don't match vector size" << endl;
			return;
		}

		cout << "Grid (" << rows << "x" << cols << "):" << endl;
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				cout << arr[i * cols + j] << " ";
			}
			cout << endl;
		}
	}

	// Get element at 2D grid position (row, col)
	T& at_grid(int row, int col, int total_cols) {
		int index = row * total_cols + col;
		if (index < 0 || index >= len) {
			throw std::out_of_range("Grid position out of range");
		}
		return arr[index];
	}

	const T& at_grid(int row, int col, int total_cols) const {
		int index = row * total_cols + col;
		if (index < 0 || index >= len) {
			throw std::out_of_range("Grid position out of range");
		}
		return arr[index];
	}

	// Fill entire vector with a specific value
	void fill(const T& value) {
		for (int i = 0; i < len; i++) {
			arr[i] = value;
		}
	}

	// Conversion to std::vector (useful for compatibility)
	std::vector<T> to_std_vector() const {
		std::vector<T> result;
		result.reserve(len);
		for (int i = 0; i < len; i++) {
			result.push_back(arr[i]);
		}
		return result;
	}

	// Conversion operator to std::vector (implicit conversion)
	operator std::vector<T>() const {
		return to_std_vector();
	}

	// Coordinate management functions
	void setCoordinates(const vector<pair<int, int>>& coords) {
		coordinates = coords;
	}

	// Get all coordinates
	vector<pair<int, int>>& getCoordinates() {
		return coordinates;
	}

	const vector<pair<int, int>>& getCoordinates() const {
		return coordinates;
	}

	// Add a single coordinate
	void addCoordinate(int x, int y) {
		coordinates.push_back(make_pair(x, y));
	}

	void addCoordinate(const pair<int, int>& coord) {
		coordinates.push_back(coord);
	}

	// Get coordinate at specific index
	pair<int, int>& getCoordinate(int index) {
		if (index < 0 || index >= static_cast<int>(coordinates.size())) {
			throw std::out_of_range("Coordinate index out of range");
		}
		return coordinates[index];
	}

	const pair<int, int>& getCoordinate(int index) const {
		if (index < 0 || index >= static_cast<int>(coordinates.size())) {
			throw std::out_of_range("Coordinate index out of range");
		}
		return coordinates[index];
	}

	// Get number of stored coordinates
	int getCoordinateCount() const {
		return static_cast<int>(coordinates.size());
	}

	// Clear all coordinates
	void clearCoordinates() {
		coordinates.clear();
	}

	// Check if coordinates are set and match vector size
	bool coordinatesMatch() const {
		return static_cast<int>(coordinates.size()) == len;
	}

	// Sync coordinates with vector size (useful after resize)
	void syncCoordinates(int default_x = 0, int default_y = 0) {
		if (static_cast<int>(coordinates.size()) < len) {
			// Add missing coordinates with default values
			for (int i = static_cast<int>(coordinates.size()); i < len; i++) {
				coordinates.push_back(make_pair(default_x, default_y));
			}
		}
		else if (static_cast<int>(coordinates.size()) > len) {
			// Remove excess coordinates
			coordinates.resize(len);
		}
	}

	// Print vector with coordinates
	void printWithCoordinates() const {
		cout << "Vector with coordinates:" << endl;
		int coord_size = static_cast<int>(coordinates.size());
		for (int i = 0; i < len; i++) {
			cout << "Index " << i << ": " << arr[i];
			if (i < coord_size) {
				cout << " at (" << coordinates[i].first << ", " << coordinates[i].second << ")";
			}
			cout << endl;
		}
	}

	// Find element by coordinate
	int findByCoordinate(int x, int y) const {
		for (int i = 0; i < static_cast<int>(coordinates.size()); i++) {
			if (coordinates[i].first == x && coordinates[i].second == y) {
				return i;
			}
		}
		return -1;
	}

	// Get value at specific coordinate
	T* getValueAtCoordinate(int x, int y) {
		int index = findByCoordinate(x, y);
		if (index != -1 && index < len) {
			return &arr[index];
		}
		return nullptr;
	}

	const T* getValueAtCoordinate(int x, int y) const {
		int index = findByCoordinate(x, y);
		if (index != -1 && index < len) {
			return &arr[index];
		}
		return nullptr;
	}
};

#endif // VECTOR_H