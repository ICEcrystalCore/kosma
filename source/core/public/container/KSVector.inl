#pragma once

#include <algorithm>
#include <iterator>
#include <new>
#include <stdexcept>
#include <type_traits>
#include <utility>

template<typename T>
Vector<T>::Vector()
    : m_data(nullptr)
    , m_size(0)
    , m_capacity(0)
{
}

template<typename T>
Vector<T>::Vector(const Vector& other)
    : m_data(nullptr)
    , m_size(0)
    , m_capacity(0)
{
    reserve(other.m_size);
    for (size_t i = 0; i < other.m_size; ++i) {
        push_back(other.m_data[i]);
    }
}

template<typename T>
Vector<T>::Vector(Vector&& other) noexcept
    : Vector()
{
    swapWith(other);
}

template<typename T>
Vector<T>::Vector(std::initializer_list<T> init)
    : Vector()
{
    reserve(init.size());
    for (const T& item : init) {
        push_back(item);
    }
}

template<typename T>
Vector<T>::Vector(size_t count, const T& value)
    : Vector()
{
    reserve(count);
    for (size_t i = 0; i < count; ++i) {
        push_back(value);
    }
}

template<typename T>
Vector<T>::Vector(const_iterator begin, const_iterator end)
    : Vector()
{
    auto distance = std::distance(begin, end);
    if (distance <= 0)
        return;
    reserve(static_cast<size_t>(distance));
    for (const_iterator it = begin; it != end; ++it) {
        push_back(*it);
    }
}

template<typename T>
Vector<T>::~Vector()
{
    // destroy constructed elements
    if (m_data) {
        if (!std::is_trivially_destructible_v<T>) {
            for (size_t i = 0; i < m_size; ++i) {
                m_data[i].~T();
            }
        }
        // deallocate raw storage
        ::operator delete(static_cast<void*>(m_data));
        m_data = nullptr;
    }
    m_size = 0;
    m_capacity = 0;
}

template<typename T>
bool Vector<T>::isEmpty() const
{
    return 0 == m_size;
}

template<typename T>
size_t Vector<T>::size() const
{
    return m_size;
}

template<typename T>
size_t Vector<T>::capacity() const
{
    return m_capacity;
}

template<typename T>
void Vector<T>::clear()
{
    if constexpr (!std::is_trivially_destructible_v<T>) {
        for (size_t i = 0; i < m_size; ++i) {
            m_data[i].~T();
        }
    }
    m_size = 0;
}

template<typename T>
void Vector<T>::reserve(size_t newCapacity)
{
    if (newCapacity <= m_capacity)
        return;

    T* newData = static_cast<T*>(::operator new(sizeof(T) * newCapacity));
    size_t i = 0;
    try {
        for (; i < m_size; ++i) {
            if constexpr (std::is_nothrow_move_constructible_v<T>
                          || !std::is_copy_constructible_v<T>) {
                new (newData + i) T(std::move(m_data[i]));
            } else {
                new (newData + i) T(m_data[i]);
            }
        }
    } catch (...) {
        // destroy any constructed in newData
        if (!std::is_trivially_destructible_v<T>) {
            for (size_t j = 0; j != i; ++j) {
                newData[j].~T();
            }
        }
        ::operator delete(static_cast<void*>(newData));
        throw;
    }

    // destroy old elements and deallocate old storage
    if (m_data) {
        if (!std::is_trivially_destructible_v<T>) {
            for (size_t j = 0; j != m_size; ++j) {
                m_data[j].~T();
            }
        }
        ::operator delete(static_cast<void*>(m_data));
    }

    m_data = newData;
    m_capacity = newCapacity;
}

template<typename T>
void Vector<T>::resize(size_t newSize)
{
    if (newSize == m_size)
        return;
    if (newSize < m_size) {
        if constexpr (!std::is_trivially_destructible_v<T>) {
            for (size_t i = newSize; i < m_size; ++i)
                m_data[i].~T();
        }
        m_size = newSize;
        return;
    }
    // newSize > m_size
    reserve(newSize);
    for (size_t i = m_size; i < newSize; ++i) {
        new (m_data + i) T();
    }
    m_size = newSize;
}

template<typename T>
void Vector<T>::shrinkToFit()
{
    if (m_capacity == m_size)
        return;
    if (m_size == 0) {
        if (m_data) {
            ::operator delete(static_cast<void*>(m_data));
            m_data = nullptr;
            m_capacity = 0;
        }
        return;
    }

    T* newData = static_cast<T*>(::operator new(sizeof(T) * m_size));
    size_t i = 0;
    try {
        for (; i < m_size; ++i) {
            if constexpr (std::is_nothrow_move_constructible_v<T>
                          || !std::is_copy_constructible_v<T>) {
                new (newData + i) T(std::move(m_data[i]));
            } else {
                new (newData + i) T(m_data[i]);
            }
        }
    } catch (...) {
        if (!std::is_trivially_destructible_v<T>) {
            for (size_t j = 0; j != i; ++j) {
                newData[j].~T();
            }
        }
        ::operator delete(static_cast<void*>(newData));
        throw;
    }

    if (m_data) {
        if (!std::is_trivially_destructible_v<T>) {
            for (size_t j = 0; j != m_size; ++j) {
                m_data[j].~T();
            }
        }
        ::operator delete(static_cast<void*>(m_data));
    }

    m_data = newData;
    m_capacity = m_size;
}

template<typename T>
void Vector<T>::swap(Vector& other) noexcept
{
    if (&other == this)
        return;
    swapWith(other);
}

template<typename T>
void Vector<T>::swapWith(Vector& other) noexcept
{
    std::swap(m_data, other.m_data);
    std::swap(m_size, other.m_size);
    std::swap(m_capacity, other.m_capacity);
}

template<typename T>
void Vector<T>::assign(size_t count, const T& value)
{
    clear();
    reserve(count);
    for (size_t i = 0; i < count; ++i) {
        push_back(value);
    }
}

template<typename T>
void Vector<T>::assign(std::initializer_list<T> init)
{
    clear();
    reserve(init.size());
    for (const T& item : init) {
        push_back(item);
    }
}

template<typename T>
Vector<T>::reference Vector<T>::at(size_t idx)
{
    if (idx >= m_size) {
        throw std::out_of_range("Index out of range");
    }
    return m_data[idx];
}

template<typename T>
Vector<T>::const_reference Vector<T>::at(size_t idx) const
{
    if (idx >= m_size) {
        throw std::out_of_range("Index out of range");
    }
    return m_data[idx];
}

// --- Newly implemented accessors, iterators and modifiers ---

template<typename T>
T* Vector<T>::data()
{
    return m_data;
}

template<typename T>
const T* Vector<T>::data() const
{
    return m_data;
}

template<typename T>
T& Vector<T>::front()
{
    return m_data[0];
}

template<typename T>
const T& Vector<T>::front() const
{
    return m_data[0];
}

template<typename T>
T& Vector<T>::back()
{
    return m_data[m_size - 1];
}

template<typename T>
const T& Vector<T>::back() const
{
    return m_data[m_size - 1];
}

template<typename T>
T* Vector<T>::begin() noexcept
{
    return m_data;
}

template<typename T>
const T* Vector<T>::begin() const noexcept
{
    return m_data;
}

template<typename T>
const T* Vector<T>::cbegin() const noexcept
{
    return m_data;
}

template<typename T>
T* Vector<T>::end() noexcept
{
    return m_data + m_size;
}

template<typename T>
const T* Vector<T>::end() const noexcept
{
    return m_data + m_size;
}

template<typename T>
const T* Vector<T>::cend() const noexcept
{
    return m_data + m_size;
}

template<typename T>
void Vector<T>::push_back(const T& value)
{
    if (m_size == m_capacity) {
        reserve(m_capacity == 0 ? 1 : m_capacity * 2);
    }
    new (m_data + m_size) T(value);
    ++m_size;
}

template<typename T>
void Vector<T>::push_back(T&& value)
{
    if (m_size == m_capacity) {
        reserve(m_capacity == 0 ? 1 : m_capacity * 2);
    }
    new (m_data + m_size) T(std::move(value));
    ++m_size;
}

template<typename T>
template<typename... Args>
T& Vector<T>::emplace_back(Args&&... args)
{
    if (m_size == m_capacity) {
        reserve(m_capacity == 0 ? 1 : m_capacity * 2);
    }
    new (m_data + m_size) T(std::forward<Args>(args)...);
    ++m_size;
    return m_data[m_size - 1];
}

template<typename T>
void Vector<T>::pop_back()
{
    if (m_size == 0)
        return;
    --m_size;
    if constexpr (!std::is_trivially_destructible_v<T>) {
        m_data[m_size].~T();
    }
}

template<typename T>
T* Vector<T>::insert(const_iterator pos, const T& value)
{
    size_t idx = static_cast<size_t>(pos - m_data);
    if (idx > m_size)
        idx = m_size;

    if (idx == m_size) {
        push_back(value);
        return m_data + (m_size - 1);
    }

    if (m_size == m_capacity) {
        reserve(m_capacity == 0 ? 1 : m_capacity * 2);
    }

    // move-construct last element to make room
    new (m_data + m_size) T(std::move(m_data[m_size - 1]));
    for (size_t i = m_size - 1; i > idx; --i) {
        m_data[i] = std::move(m_data[i - 1]);
    }
    // place the new value
    m_data[idx].~T();
    new (m_data + idx) T(value);
    ++m_size;
    return m_data + idx;
}

template<typename T>
T* Vector<T>::erase(const_iterator pos)
{
    size_t idx = static_cast<size_t>(pos - m_data);
    if (idx >= m_size)
        return m_data + m_size;

    // destroy element at idx
    if constexpr (!std::is_trivially_destructible_v<T>) {
        m_data[idx].~T();
    }

    for (size_t i = idx; i + 1 < m_size; ++i) {
        // move-assign next into current
        m_data[i] = std::move(m_data[i + 1]);
    }

    // destroy duplicate last element
    --m_size;
    if constexpr (!std::is_trivially_destructible_v<T>) {
        m_data[m_size].~T();
    }

    return m_data + idx;
}

template<typename T>
Vector<T>& Vector<T>::operator=(const Vector& other)
{
    if (&other == this)
        return *this;
    Vector tmp(other);
    swapWith(tmp);
    return *this;
}

template<typename T>
Vector<T>& Vector<T>::operator=(Vector&& other) noexcept
{
    if (&other == this)
        return *this;
    swapWith(other);
    return *this;
}

template<typename T>
Vector<T>& Vector<T>::operator=(std::initializer_list<T> init)
{
    assign(init);
    return *this;
}

template<typename T>
T& Vector<T>::operator[](size_t idx)
{
    return m_data[idx];
}

template<typename T>
const T& Vector<T>::operator[](size_t idx) const
{
    return m_data[idx];
}

template<typename T>
bool Vector<T>::operator==(const Vector& other) const noexcept
{
    if (m_size != other.m_size)
        return false;
    for (size_t i = 0; i < m_size; ++i) {
        if (!(m_data[i] == other.m_data[i]))
            return false;
    }
    return true;
}

template<typename T>
bool Vector<T>::operator!=(const Vector& other) const noexcept
{
    return !(*this == other);
}
