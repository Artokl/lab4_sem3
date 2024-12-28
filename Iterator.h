#ifndef VECTOR_ITERATOR_HPP
#define VECTOR_ITERATOR_HPP

#include <cstddef>
#include <iterator>
#include <iostream>

template <typename T> class DynamicArray;


template <typename T> class DynamicArray_iterator
{
    private:
        T *p;

    public:
        friend class DynamicArray<T>;
        using difference_type = std::ptrdiff_t;
        using value_type = std::remove_cv_t<T>;
        using pointer = T *;
        using const_pointer = const T *;
        using reference = T &;
        using iterator_category = std::random_access_iterator_tag;

    DynamicArray_iterator(T *p);
    DynamicArray_iterator(const DynamicArray_iterator &it);

    DynamicArray_iterator<T> operator+(const size_t &num) const noexcept;
    DynamicArray_iterator<T> operator-(const size_t &num) const noexcept;

        size_t operator-(const DynamicArray_iterator &other) const;
        bool operator!=(const DynamicArray_iterator &other) const noexcept;
        bool operator==(const DynamicArray_iterator &other) const noexcept;
        bool operator>(const DynamicArray_iterator &other) const noexcept;
        bool operator<(const DynamicArray_iterator &other) const noexcept;

        T &operator*() const noexcept;

    DynamicArray_iterator &operator++() noexcept;
    DynamicArray_iterator &operator--() noexcept;
};

template <typename T> DynamicArray_iterator<T>::DynamicArray_iterator(T *p) : p(p)
{
}

template <typename T> DynamicArray_iterator<T>::DynamicArray_iterator(const DynamicArray_iterator &it)
{
    p = it.p;
}

template <typename T> DynamicArray_iterator<T> DynamicArray_iterator<T>::operator+(const size_t &num) const noexcept
{
    return DynamicArray_iterator<T>(p + num);
}

template <typename T> DynamicArray_iterator<T> DynamicArray_iterator<T>::operator-(const size_t &num) const noexcept
{
    return DynamicArray_iterator<T>(p - num);
}

template <typename T> size_t DynamicArray_iterator<T>::operator-(const DynamicArray_iterator &other) const
{
    if (*this < other)
        throw std::invalid_argument("Cannot subtract iterators from different vectors");
    return p - other.p;
}

template <typename T> bool DynamicArray_iterator<T>::operator!=(const DynamicArray_iterator &other) const noexcept
{
    return p != other.p;
}

template <typename T> bool DynamicArray_iterator<T>::operator==(const DynamicArray_iterator &other) const noexcept
{
    return p == other.p;
}

template <typename T> bool DynamicArray_iterator<T>::operator>(const DynamicArray_iterator &other) const noexcept
{
    return p > other.p;
}


template <typename T> bool DynamicArray_iterator<T>::operator<(const DynamicArray_iterator &other) const noexcept
{
    return p < other.p;
}


template <typename T> T &DynamicArray_iterator<T>::operator*() const noexcept
{
    return *p;
}

template <typename T> DynamicArray_iterator<T> &DynamicArray_iterator<T>::operator++() noexcept
{
    ++p;
    return *this;
}

template <typename T> DynamicArray_iterator<T> &DynamicArray_iterator<T>::operator--() noexcept
{
    --p;
    return *this;
}

#endif // VECTOR_ITERATOR_HPP