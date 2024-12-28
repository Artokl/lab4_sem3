#ifndef VECTOR_HPP
#define VECTOR_HPP
#include "Iterator.h"
#include <algorithm>

template <typename T>
class DynamicArray
{
    protected:
        T *data = nullptr;
        size_t size = 0;
        size_t capacity = 0;

    public:
        typedef DynamicArray_iterator<T> iterator;

    DynamicArray();
    DynamicArray(const size_t capacity);
    DynamicArray(const size_t size, T default_value);
    DynamicArray(const std::initializer_list<T> list);
    DynamicArray(const DynamicArray<T> &input_vector);
    DynamicArray(DynamicArray<T> &&input_vector);

        ~DynamicArray() noexcept;

        void push_back(const T &value);
        void push_front(const T &value);
        void pop_back();
        void pop_front();

        iterator insert(iterator position, const T &value);
        iterator insert(iterator position, iterator first, iterator last);
        iterator erase(iterator position);
        iterator erase(iterator first, iterator last);

        void resize(const size_t new_size);
        bool is_empty() const noexcept;

        size_t get_size() const noexcept;

        T &operator[](const size_t index);
        T operator[](const size_t index) const;
    DynamicArray &operator=(const DynamicArray<T> &other);
    DynamicArray &operator=(DynamicArray<T> &&other);

        iterator begin() noexcept;
        iterator end() noexcept;

        iterator cbegin() const noexcept;
        iterator cend() const noexcept;
};

template <typename T> DynamicArray<T>::DynamicArray()
{
    data = new T[1];
    capacity = 1;
}

template <typename T> DynamicArray<T>::DynamicArray(const size_t capacity) : capacity(capacity), size(capacity)
{
    data = new T[capacity];
}

template <typename T> DynamicArray<T>::DynamicArray(const size_t size, T default_value) : size(size)
{
    data = new T[size * 2];
    if (data == nullptr)
        throw std::bad_alloc();
    capacity = size * 2;
    std::fill(data, data + size, default_value);
}

template <typename T> DynamicArray<T>::DynamicArray(const std::initializer_list<T> list)
{
    size = list.size();
    data = new T[size];
    capacity = size;
    std::copy(list.begin(), list.end(), data);
}

template <typename T> DynamicArray<T>::DynamicArray(const DynamicArray<T> &input_vector)
{
    *this = input_vector;
}

template <typename T> DynamicArray<T>::DynamicArray(DynamicArray<T> &&input_vector)
{
    *this = std::move(input_vector);
}

template <typename T> DynamicArray<T>::~DynamicArray() noexcept
{
    delete[] data;
}

template <typename T> void DynamicArray<T>::push_back(const T &value)
{
    if (size == capacity)
        resize(capacity * 2);
    data[size++] = value;
}

template <typename T> void DynamicArray<T>::push_front(const T &value)
{
    if (size == capacity)
        resize(capacity * 2);
    std::copy_backward(data, data + size, data + size + 1);
    data[0] = value;
}

template <typename T> void DynamicArray<T>::pop_back()
{
    --size;
}

template <typename T> void DynamicArray<T>::pop_front()
{
    std::copy(data + 1, data + size, data);
    --size;
}

template <typename T> typename DynamicArray<T>::iterator DynamicArray<T>::insert(iterator position, const T &value)
{
    if (position > end() || position < begin())
        throw std::invalid_argument("Invalid position");
    size_t pos_index = position - begin();
    if (size == capacity)
        resize(capacity * 2);
    std::copy_backward(data + pos_index, data + size, data + size + 1);
    DynamicArray<T>::iterator new_pos = iterator(data + pos_index);
    *new_pos = value;
    ++size;
    return new_pos;
}

template <typename T> typename DynamicArray<T>::iterator DynamicArray<T>::insert(iterator position, iterator first, iterator last)
{
    if (first > last || position > end() || position < begin())
        throw std::invalid_argument("Invalid range");
    size_t dist = last - first;
    size_t pos_index = position - begin();
    while (capacity - size < dist)
        resize(capacity * 2);
    std::copy_backward(data + pos_index, data + size, data + size + dist);
    std::copy(first, last, data + pos_index);
    size += dist;
    return iterator(data + pos_index);
}

template <typename T> typename DynamicArray<T>::iterator DynamicArray<T>::erase(iterator position)
{
    if (position > end() || position < begin())
        throw std::invalid_argument("Invalid position");
    std::copy(position + 1, end(), position);
    --size;
    return iterator(position);
}

template <typename T> typename DynamicArray<T>::iterator DynamicArray<T>::erase(iterator first, iterator last)
{
    if (first > last || first < begin() || last > end())
        throw std::invalid_argument("Invalid range");
    size_t dist = last - first;
    std::copy(last, end(), first);
    size -= dist;
    return iterator(first);
}

template <typename T> void DynamicArray<T>::resize(const size_t new_size)
{
    if (new_size < size)
        throw std::runtime_error("Cannot resize to a smaller size");
    T *new_data = new T[new_size];
    std::copy(data, data + size, new_data);
    capacity = new_size;
    delete[] data;
    data = new_data;
}

template <typename T> bool DynamicArray<T>::is_empty() const noexcept
{
    return size == 0;
}

template <typename T> size_t DynamicArray<T>::get_size() const noexcept
{
    return size;
}

template <typename T> T &DynamicArray<T>::operator[](const size_t index)
{
    if (index >= size)
        throw std::out_of_range("Index out of range!");
    return data[index];
}

template <typename T> T DynamicArray<T>::operator[](const size_t index) const
{
    if (index >= size)
        throw std::out_of_range("Index out of range!");
    return data[index];
}

template <typename T> DynamicArray<T> &DynamicArray<T>::operator=(const DynamicArray<T> &other)
{
    if (this == &other)
        return *this;
    delete[] data;
    size = other.size;
    capacity = other.capacity;
    data = new T[capacity];
    if (data == nullptr)
        throw std::bad_alloc();
    std::copy(other.cbegin(), other.cend(), data);
    return *this;
}

template <typename T> DynamicArray<T> &DynamicArray<T>::operator=(DynamicArray<T> &&other)
{
    if (this == &other)
        return *this;
    delete[] data;
    data = other.data;
    size = other.size;
    capacity = other.capacity;
    other.data = nullptr;
    other.size = 0;
    other.capacity = 0;
    return *this;
}

template <typename T> typename DynamicArray<T>::iterator DynamicArray<T>::begin() noexcept
{
    return iterator(data);
}

template <typename T> typename DynamicArray<T>::iterator DynamicArray<T>::end() noexcept
{
    return iterator(data + size);
}

template <typename T> typename DynamicArray<T>::iterator DynamicArray<T>::cbegin() const noexcept
{
    return iterator(data);
}


template <typename T> typename DynamicArray<T>::iterator DynamicArray<T>::cend() const noexcept
{
    return iterator(data + size);
}


#endif // VECTOR_HPP