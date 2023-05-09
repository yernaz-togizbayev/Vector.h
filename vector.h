#ifndef VECTOR_H
#define VECTOR_H

#include <iostream>
#include <stdexcept>
#include <initializer_list>

template <typename T>
class Vector;

template <typename T>
class Vector {
public:
	class ConstIterator;
	class Iterator;
    using value_type = T;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	using reference = value_type&;
	using const_reference = const value_type&;
    using pointer = value_type*;
	using const_pointer = const value_type*;
	using iterator = Vector::Iterator;
	using const_iterator = Vector::ConstIterator;
    
private:
	size_type sz;
	size_type max_sz;
	pointer values;
	static constexpr size_type min_sz{5};
public:
	Vector() {
		max_sz = 0;
		sz = 0;
		values = nullptr;
	}

	Vector(const Vector<T>& src) {
		max_sz = src.max_sz;
		sz = src.sz;
		values = new value_type[max_sz];

		for (size_type i{0}; i < sz; ++i)
			values[i] = src.values[i];
	}

	Vector(size_type n) {
		sz = 0;
		max_sz = (n<min_sz) ? min_sz : n;
		values = new value_type[max_sz];
	}

	Vector(const std::initializer_list<value_type>& init_list) {
		size_type init_list_size = init_list.size();
		
		sz = init_list_size;
		max_sz = init_list_size;

		values = new value_type[max_sz];
		
		size_type i{0};
		for(const_pointer iter{init_list.begin()}; iter != init_list.end(); ++i, ++iter)
			values[i] = *iter;
	}

	~Vector() {delete[] values;}

	inline Vector<T>& operator= (Vector<T> src) {
		std::swap(sz, src.sz);
		std::swap(max_sz, src.max_sz);
		std::swap(values, src.values);
		return *this;
	}

	inline size_type size() const {return sz;}

	inline bool empty() const {return sz == 0;}
	
	inline void clear() {sz = 0;}
	
	void reserve(size_type n) {
        if (n > max_sz) {
            value_type* new_values = new value_type[n];
            for (Vector<T>::size_type i{0}; i < sz; ++i)
                new_values[i] = values[i];
            max_sz = n;
            delete[] values;
            values = new_values;
        }
    }

	inline void shrink_to_fit() {
		if (max_sz > sz)
			max_sz = sz;
		else return;
		}

	inline void push_back(const_reference val) {
		if(sz >= max_sz)
			reserve(2*max_sz+1);
		values[sz++] = val;
	}

	inline void pop_back() {
		if (empty())
			throw std::runtime_error("vector is empty");
		--sz;
	}

	inline reference operator[](size_type index) {
		if (index >= 0 && index < sz)
    		return values[index];
		throw std::runtime_error("Out of range");
	}

	inline const_reference operator[](size_type index) const {
		if (index >= 0 && index < sz)
			return values[index];
    	throw std::runtime_error("Out of range");
	}

	inline size_type capacity() const {return max_sz;}
	
	inline iterator begin() {
		pointer end { values + size() };
		iterator itr_begin { values, end };
		return itr_begin;
	}
	
	inline iterator end() {
		pointer end { values + size() };
		iterator itr_end { end, end };
		return itr_end;
	}
	
	inline const_iterator begin() const {
		pointer end { values + size() };
		const_iterator itr_begin { values, end };
		return itr_begin;
	}
	
	inline const_iterator end() const {
		pointer end { values + size() };
		const_iterator itr_end { end, end };
		return itr_end;
	}
	
	iterator insert(const_iterator pos, const_reference val) {
		auto diff = pos - begin();
		if (diff < 0 || static_cast<size_type>(diff) > sz)
			throw std::runtime_error("Iterator out of bounds");
		size_type current{static_cast<size_type>(diff)};
		if (sz >= max_sz)
			reserve(max_sz*2 + 1);
		for (auto i{sz}; i-- > current;)
			values[i+1] = values[i];
		values[current] = val;
		++sz;
		return iterator{values+current};
	}
	
	iterator erase(const_iterator pos) {
		auto diff = pos - begin();
		if (diff < 0 || static_cast<size_type>(diff) >= sz)
			throw std::runtime_error("Iterator out of bounds");
		size_type current{static_cast<size_type>(diff)};
		for (auto i{current}; i < sz-1; ++i)
			values[i] = values[i+1];
		--sz;
		return iterator{values+current};
	}
	

	template <typename U>
	friend std::ostream& operator<< (std::ostream& o, const Vector<U>& v);
	
	
	
	class Iterator {
		public:
			using value_type = Vector::value_type;
			using reference = Vector::reference;
			using pointer = Vector::pointer;
			using difference_type = Vector::difference_type;
			using iterator_category = std::forward_iterator_tag;
		private:
			pointer ptr, end_ptr;
		public:
			Iterator() : ptr{nullptr}, end_ptr{nullptr} {}

			Iterator(pointer ptr, pointer end_ptr = nullptr) : ptr{ptr}, end_ptr{end_ptr} {}

			inline reference operator*() const {
				if(ptr == end_ptr)
					throw std::runtime_error("out of bounds");
				return *ptr;
			}
			
			inline pointer operator->() const {
				if (ptr == end_ptr)
					throw std::runtime_error("out of bounds");
				return ptr;
			}
			
			inline bool operator== (const const_iterator& x) const {
				return static_cast<ConstIterator>(*this) == x;
			}
			
			inline bool operator!= (const const_iterator& x) const {
				return static_cast<ConstIterator>(*this) != x;
			}
			
			inline iterator& operator++() {
				if (ptr == end_ptr)
					return *this;
				++ptr;
				return *this;
			}
			
			inline iterator operator++(int) {
				if (ptr == end_ptr)
					return *this;
				iterator temp{*this};
				++ptr;
				return temp;
			}
			
			inline operator const_iterator() const {
				return ConstIterator( this->ptr, this->end_ptr );
			}
			
			inline difference_type operator- (const const_iterator& x) {
				return static_cast<ConstIterator>(*this) - x;
			}
	};
	
	
	
	class ConstIterator {
		public:
			using value_type = Vector::value_type;
			using reference = Vector::const_reference;
			using pointer = Vector::const_pointer;
			using difference_type = Vector::difference_type;
			using iterator_category = std::forward_iterator_tag;
		private:
			pointer ptr, end_ptr;
		public:			
			ConstIterator() : ptr{nullptr}, end_ptr{nullptr} {}

			ConstIterator(pointer ptr, pointer end_ptr = nullptr): ptr{ptr}, end_ptr{end_ptr} {}
			
			inline reference operator*() const {
				if(ptr == end_ptr)
					throw std::runtime_error("out of bounds");
				return *ptr;
			}
			
			inline pointer operator->() const {
				if (ptr == end_ptr)
					throw std::runtime_error("out of bounds");
				return ptr;
			}
			
			inline bool operator== (const const_iterator& x) const {return ptr == x.ptr;}
			
			inline bool operator!= (const const_iterator& x) const {return ptr != x.ptr;}
			
			inline const_iterator& operator++() {
				if (ptr == end_ptr)
					return *this;
				++ptr;
				return *this;
			}
			
			inline const_iterator operator++(int) {
				if (ptr == end_ptr)
					return *this;
				const_iterator temp{*this};
				++ptr;
				return temp;
			}
			
			inline difference_type operator- (const const_iterator& x) const {
				return ptr - x.ptr;
			}
	};
};

template <typename T>
std::ostream& operator<< (std::ostream& o, const Vector<T>& v) {
	o << '[';
	bool first{true};
	for (size_t i{0}; i < v.size(); ++i)
	{
		if (first)
			first = false;
		else o << ", ";
		o << v.values[i];
	}
		
	o << ']';
	return o;
}

#endif
