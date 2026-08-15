//
// Created by Jun on 2025/10/26.
//

#ifndef KOSMA_KSVECTOR_H
#define KOSMA_KSVECTOR_H

#include <initializer_list>
#include <vector>

namespace Kosma::Core {

template<typename T>
class Vector {
public:
    using value_type = T;
    using iterator = T*;
    using const_iterator = const T*;
    using reference = T&;
    using const_reference = const T&;

    Vector();

    Vector(const Vector& other);

    Vector(Vector&& other) noexcept;

    Vector(std::initializer_list<T> init);

    explicit Vector(size_t count, const T& value = {});

    Vector(const_iterator begin, const_iterator end);

    ~Vector();

    bool isEmpty() const;

    size_t size() const;

    size_t capacity() const;

    void clear();

    void resize(size_t newSize);

    void shrinkToFit();

    void swap(Vector& other) noexcept;

    void assign(size_t count, const T& value);

    void assign(std::initializer_list<T> init);

    reference at(size_t idx);

    const_reference at(size_t idx) const;

    T* data();

    const T* data() const;

    reference front();

    const_reference front() const;

    reference back();

    const_reference back() const;

    iterator begin() noexcept;

    const_iterator begin() const noexcept;

    const_iterator cbegin() const noexcept;

    iterator end() noexcept;

    const_iterator end() const noexcept;

    const_iterator cend() const noexcept;

    void push_back(const T& value);

    void push_back(T&& value);

    template<typename... Args>
    T& emplace_back(Args&&... args);

    void pop_back();

    void reserve(size_t newCapacity);

    iterator insert(const_iterator pos, const T& value);

    iterator erase(const_iterator pos);

    Vector& operator=(const Vector& other);

    Vector& operator=(Vector&& other) noexcept;

    Vector& operator=(std::initializer_list<T> init);

    reference operator[](size_t idx);

    const_reference operator[](size_t idx) const;

    bool operator==(const Vector& other) const noexcept;

    bool operator!=(const Vector& other) const noexcept;

protected:
    void swapWith(Vector& other) noexcept;

private:
    T* m_data;
    size_t m_size;
    size_t m_capacity;
};

#include "container/KSVector.inl"

}

#endif  // KOSMA_KSVECTOR_H