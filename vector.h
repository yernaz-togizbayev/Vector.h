#ifndef ADS_SET_H
#define ADS_SET_H

#include <functional>
#include <algorithm>
#include <iostream>
#include <stdexcept>

template <typename Key, size_t N = 10>
class ADS_set
{
public:
    class Iterator;
    using value_type = Key;
    using key_type = Key;
    using reference = value_type &;
    using const_reference = const value_type &;
    using size_type = size_t;
    using difference_type = std::ptrdiff_t;
    using const_iterator = Iterator;
    using iterator = const_iterator;
    using key_equal = std::equal_to<key_type>;
    using hasher = std::hash<key_type>;

 private:
    struct Element
    {
        key_type key;
        Element *next;

        Element(const key_type& key, Element* next = nullptr) : key(key), next(next) {}
    };
    Element **table {nullptr};
    size_type table_size {0};
    size_type current_size {0};
    float max_lf {0.7};
    

    Element *add(const key_type &key);
    Element *locate (const key_type &key) const;
    size_type h(const key_type &key) const { return hasher{}(key) % table_size; }
    void reserve(size_type n);
    void rehash(size_type n);

public:
    ADS_set() { rehash(N); }
    ADS_set(std::initializer_list<key_type> ilist): ADS_set{} { insert(ilist); }
    template <typename InputIt> ADS_set(InputIt first, InputIt last): ADS_set{} { insert(first, last); }
    ADS_set(const ADS_set &other): ADS_set(other.begin(), other.end()) {}
    ~ADS_set();

    ADS_set &operator=(const ADS_set &other);
    ADS_set &operator=(std::initializer_list<key_type> ilist);
    
    size_type size() const { return current_size; }
    bool empty() const { return current_size == 0; }

    void insert(std::initializer_list<key_type> ilist) { insert(ilist.begin(), ilist.end()); }
    std::pair<iterator, bool> insert(const key_type &key);
    template <typename InputIt> void insert(InputIt first, InputIt last);

    void clear();
    size_type erase(const key_type &key);

    size_type count(const key_type &key) const { return locate(key) != nullptr; }
    iterator find(const key_type &key) const;

    void swap(ADS_set &other);

    const_iterator begin() const;
    const_iterator end() const;

    void dump(std::ostream &o = std::cerr) const;

    friend bool operator==(const ADS_set &lhs, const ADS_set &rhs)
    {
        if (lhs.current_size != rhs.current_size) return false;
        for (const auto &key : lhs) if (!rhs.count(key)) return false;
        return true;
    }

    friend bool operator!=(const ADS_set &lhs, const ADS_set &rhs) { return !(lhs == rhs); }
};

template <typename Key, size_t N>
void ADS_set<Key, N>::swap(ADS_set &other)
{
    std::swap(table, other.table);
    std::swap(table_size, other.table_size);
    std::swap(current_size, other.current_size);
    std::swap(max_lf, other.max_lf);
}

template <typename Key, size_t N>
typename ADS_set<Key, N>::iterator ADS_set<Key, N>::find(const key_type &key) const
{
    if (auto elem {locate(key)}) return Iterator(&table[h(key)], &table[table_size], elem);
    return end();
}

template <typename Key, size_t N>
typename ADS_set<Key, N>::size_type ADS_set<Key, N>::erase(const key_type &key)
{
    size_type index = h(key);
    Element *prev = nullptr;
    for (Element *elem = table[index]; elem; prev = elem, elem = elem->next)
    {
        if (key_equal{}(elem->key, key))
        {
            if (prev) prev->next = elem->next;
            else table[index] = elem->next;
            
            delete elem;
            --current_size;
            return 1;
        }
    }
    return 0;
}

template <typename Key, size_t N>
void ADS_set<Key,N>::clear()
{
  ADS_set tmp;
  swap(tmp);
}

template <typename Key, size_t N>
std::pair<typename ADS_set<Key,N>::iterator,bool> ADS_set<Key, N>::insert(const key_type &key)
{
    if (auto elem{locate(key)}) return {Iterator(&table[h(key)], &table[table_size], elem), false};
    reserve(current_size + 1);
    return {iterator(&table[h(key)], &table[table_size], add(key)), true};
}

template<typename Key, size_t N>
ADS_set<Key, N> &ADS_set<Key, N>::operator=(const ADS_set &other)
{
    ADS_set tmp{other};
    swap(tmp);
    return *this;
}

template <typename Key, size_t N>
ADS_set<Key, N> &ADS_set<Key, N>::operator=(std::initializer_list<key_type> ilist)
{
    ADS_set tmp{ilist};
    swap(tmp);
    return *this;
}

template <typename Key, size_t N>
ADS_set<Key,N>::~ADS_set()
{
    for (size_type i = 0; i < table_size; ++i)
    {
        Element *current = table[i];
        while (current)
        {
            Element *to_delete = current;
            current = current->next;
            delete to_delete;
        }
    }
    delete[] table;
}

template <typename Key, size_t N>
typename ADS_set<Key, N>::Element* ADS_set<Key, N>::add(const key_type &key)
{
    size_type index = h(key);
    Element *new_element = new Element(key, table[index]);
    table[index] = new_element;
    ++current_size;
    return new_element;
}

template <typename Key, size_t N>
typename ADS_set<Key, N>::Element* ADS_set<Key, N>::locate(const key_type &key) const
{
    for (Element *elem = table[h(key)]; elem; elem = elem->next)
        if (key_equal{}(elem->key, key)) return elem;
    return nullptr;
}

template <typename Key, size_t N>
template <typename InputIt> void ADS_set<Key, N>::insert(InputIt first, InputIt last)
{
    for (auto it{first}; it != last; ++it)
    {
        if (!count(*it))
        {
            reserve(current_size + 1);
            add(*it);
        }
    }
}

template <typename Key, size_t N>
void ADS_set<Key,N>::dump(std::ostream &o) const
{
    o << "table size = " << table_size << ", current_size = " << current_size << "\n";
    for (size_type idx {0}; idx < table_size; ++idx)
    {
        o << idx << ": ";
        for (Element *tmp{table[idx]}; tmp; tmp = tmp->next)
            o << " --> " << tmp->key;
        o << "\n";
    }
    o << "\n";
}

template <typename Key, size_t N>
void ADS_set<Key,N>::reserve(size_type n)
{
  if (table_size * max_lf >= n) return;
  size_type new_table_size {table_size};
  while (new_table_size * max_lf < n) new_table_size *= 3;
  rehash(new_table_size);
}

template <typename Key, size_t N>
void ADS_set<Key,N>::rehash(size_type n)
{
  size_type new_table_size {std::max(N,std::max(n,size_type(current_size/max_lf)))};
  Element **new_table {new Element *[new_table_size + 1]()};
  Element **old_table {table};
  size_type old_table_size {table_size};

  table = new_table;
  table_size = new_table_size;
  size_type new_current_size = 0;
  
  for (size_type i {0}; i < old_table_size; ++i)
  {
    Element *current = old_table[i];
    while (current)
    {
        Element *next = current->next;
        size_type new_index = h(current->key);
        current->next = table[new_index];
        table[new_index] = current;
        current = next;
        new_current_size++;
    }
  }
  current_size = new_current_size;
  delete[] old_table;
}

template <typename Key, size_t N>
typename ADS_set<Key, N>::const_iterator ADS_set<Key, N>::begin() const
{
    for (size_type i{0}; i < table_size; ++i)
    {
        if (table[i])
            return const_iterator(&table[i], &table[table_size], table[i]);
    }

    return end();
}

template <typename Key, size_t N>
typename ADS_set<Key, N>::const_iterator ADS_set<Key, N>::end() const
{ 
    return const_iterator(&table[table_size], &table[table_size], nullptr);
}

template <typename Key, size_t N>
class ADS_set<Key, N>::Iterator
{
public:
    using value_type = Key;
    using difference_type = std::ptrdiff_t;
    using reference = const value_type &;
    using pointer = const value_type *;
    using iterator_category = std::forward_iterator_tag;

    explicit Iterator(Element **table = nullptr, Element **end_table = nullptr, Element *current = nullptr) : table(table), end_table(end_table), current(current){}
    
    reference operator*() const { return current->key; }

    pointer operator->() const { return &current->key; }
    
    Iterator &operator++()
    {
        current = current->next;
        skip();
        return *this;
    }
    
    Iterator operator++(int)
    {
        Iterator tmp{*this};
        ++*this;
        return tmp;
    }
    friend bool operator==(const Iterator &lhs, const Iterator &rhs) { return lhs.current == rhs.current; }

    friend bool operator!=(const Iterator &lhs, const Iterator &rhs) { return !(lhs == rhs); }

private:
    Element **table;
    Element **end_table;
    Element *current;

    void skip()
    {
        if (!current && table != end_table)
        {
            ++table;
            while (table != end_table && !(*table)) ++table;
            if (table != end_table) current = *table;
        }
    }
};

template <typename Key, size_t N>
void swap(ADS_set<Key, N> &lhs, ADS_set<Key, N> &rhs) { lhs.swap(rhs); }

#endif  // ADS_SET_H
