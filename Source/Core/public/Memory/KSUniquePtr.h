//
// Created by Jun on 2025/8/30.
//

#ifndef KOSMA_KSUNIQUEPTR_H
#define KOSMA_KSUNIQUEPTR_H

#include <memory>

#include "Utility/KSCompressedPair.h"

namespace Kosma::Core {

template<typename T, typename Deleter = std::default_delete<T>>
class UniquePtr {
public:
    UniquePtr(T* ptr = nullptr) noexcept
        : m_ptr(Deleter(), ptr)
    {
    }

    UniquePtr(nullptr_t)
        : UniquePtr(nullptr)
    {
    }

    ~UniquePtr() { reset(); }

    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    UniquePtr(UniquePtr&& other) noexcept
        : m_ptr(other.m_ptr)
    {
        other.m_ptr = nullptr;
    }

    UniquePtr& operator=(UniquePtr&& other) noexcept
    {
        if (this != &other) {
            reset(other.m_ptr.first());
            other.m_ptr = nullptr;
        }
        return *this;
    }

    const T& operator*() const noexcept { return *m_ptr.second(); }

    T& operator*() noexcept { return *m_ptr.second(); }

    const T* operator->() const noexcept { return m_ptr.second(); }

    T* operator->() noexcept { return m_ptr.second(); }

    const T* get() const noexcept { return m_ptr.second(); }

    T* get() noexcept { return m_ptr; }

    T* release() noexcept
    {
        T* temp = m_ptr;
        m_ptr = nullptr;
        return temp;
    }

    void reset(T* ptr = nullptr) noexcept
    {
        m_ptr.first()(m_ptr.second());
        m_ptr.second() = ptr;
    }

    explicit operator bool() const { return m_ptr != nullptr; }

private:
    CompressedPair<Deleter, T*> m_ptr;
};

template<typename T, typename Deleter>
class UniquePtr<T[], Deleter> {
public:
    UniquePtr(T* ptr = nullptr) noexcept
        : m_ptr(Deleter(), std::move(ptr))
    {
    }

    UniquePtr(nullptr_t)
        : UniquePtr()
    {
    }

    UniquePtr& operator=(const UniquePtr&) = delete;

    UniquePtr(UniquePtr&& other) noexcept
        : m_ptr(std::move(other.m_ptr))
    {
        other.m_ptr.second() = nullptr;
    }

    UniquePtr& operator=(UniquePtr&& other) noexcept
    {
        if (this != &other) {
            reset(other.release());
            m_ptr.m_first = std::move(other.m_ptr.m_first);
        }
        return *this;
    }

    ~UniquePtr() { reset(); }

    T* get() const noexcept { return m_ptr.second(); }

    T& operator[](std::size_t i) const noexcept { return m_ptr.second()[i]; }

    explicit operator bool() const noexcept { return m_ptr.second() != nullptr; }

    T* release() noexcept
    {
        auto tmp = m_ptr.second();
        m_ptr.second() = nullptr;
        return tmp;
    }

    void reset(T* p = nullptr) noexcept
    {
        if (m_ptr.second())
            m_ptr.first()(m_ptr.second());  // 调用 deleter，这里默认可以 delete[]
        m_ptr.second() = p;
    }

private:
    CompressedPair<Deleter, T*> m_ptr;
};

template<typename T, typename... Args>
UniquePtr<T> makeUnique(Args&&... args)
{
    return UniquePtr<T>(new T(std::forward<Args>(args)...));
}

}

#endif  // KOSMA_KSUNIQUEPTR_H
