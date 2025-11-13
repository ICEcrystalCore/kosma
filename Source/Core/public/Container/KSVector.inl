#pragma once

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
}

template<typename T>
Vector<T>::Vector(Vector&& other)
    : Vector()
{
    swap(other);
}
