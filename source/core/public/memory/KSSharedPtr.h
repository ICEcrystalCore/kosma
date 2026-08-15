//
// Created by Jun on 2025/8/30.
//

#ifndef KOSMA_KSSHAREDPTR_H
#define KOSMA_KSSHAREDPTR_H

#include <atomic>
#include <new>
#include <type_traits>
#include <utility>

namespace Kosma::Core {

template<typename T>
class SharedPtr;

template<typename T>
class WeakPtr;

template<typename To, typename From>
SharedPtr<To> dynamicPointerCast(const SharedPtr<From>& sp);

template<typename To, typename From>
SharedPtr<To> staticPointerCast(const SharedPtr<From>& sp);

template<typename T, typename... Args>
SharedPtr<T> makeShared(Args&&... args);

namespace detail {

struct ControlBlock {
    std::atomic<int> refCount{1};
    std::atomic<int> weakCount{1};  // +1 shared by all strong refs collectively

    void addRef() noexcept
    {
        refCount.fetch_add(1, std::memory_order_relaxed);
    }

    bool tryAddRef() noexcept
    {
        int count = refCount.load(std::memory_order_relaxed);
        while (count > 0) {
            if (refCount.compare_exchange_weak(count, count + 1,
                    std::memory_order_acquire, std::memory_order_relaxed))
                return true;
        }
        return false;
    }

    void releaseRef() noexcept
    {
        if (refCount.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            deleteObject();
            releaseWeak();
        }
    }

    void addWeak() noexcept
    {
        weakCount.fetch_add(1, std::memory_order_relaxed);
    }

    void releaseWeak() noexcept
    {
        if (weakCount.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            delete this;
        }
    }

    virtual void deleteObject() noexcept = 0;
    virtual ~ControlBlock() = default;
};

template<typename T>
struct ControlBlockPtr final : ControlBlock {
    T* ptr;
    explicit ControlBlockPtr(T* p) noexcept : ptr(p) {}
    void deleteObject() noexcept override { delete ptr; }
};

// Single-allocation control block used by makeShared<T>
template<typename T>
struct ControlBlockInline final : ControlBlock {
    alignas(T) char storage[sizeof(T)];
    T* get() noexcept { return std::launder(reinterpret_cast<T*>(storage)); }
    void deleteObject() noexcept override { get()->~T(); }
};

}  // namespace detail

template<typename T>
class SharedPtr {
public:
    SharedPtr() noexcept : m_ctrl(nullptr), m_ptr(nullptr) {}

    SharedPtr(std::nullptr_t) noexcept : SharedPtr() {}

    explicit SharedPtr(T* ptr) : m_ctrl(nullptr), m_ptr(nullptr)
    {
        if (ptr) {
            m_ctrl = new detail::ControlBlockPtr<T>(ptr);
            m_ptr = ptr;
        }
    }

    SharedPtr(const SharedPtr& other) noexcept
        : m_ctrl(other.m_ctrl), m_ptr(other.m_ptr)
    {
        if (m_ctrl) m_ctrl->addRef();
    }

    SharedPtr(SharedPtr&& other) noexcept : m_ctrl(other.m_ctrl), m_ptr(other.m_ptr)
    {
        other.m_ctrl = nullptr;
        other.m_ptr = nullptr;
    }

    template<typename U, std::enable_if_t<std::is_convertible_v<U*, T*>, int> = 0>
    SharedPtr(const SharedPtr<U>& other) noexcept
        : m_ctrl(other.m_ctrl), m_ptr(other.m_ptr)
    {
        if (m_ctrl) m_ctrl->addRef();
    }

    template<typename U, std::enable_if_t<std::is_convertible_v<U*, T*>, int> = 0>
    SharedPtr(SharedPtr<U>&& other) noexcept : m_ctrl(other.m_ctrl), m_ptr(other.m_ptr)
    {
        other.m_ctrl = nullptr;
        other.m_ptr = nullptr;
    }

    ~SharedPtr()
    {
        if (m_ctrl) m_ctrl->releaseRef();
    }

    SharedPtr& operator=(const SharedPtr& other) noexcept
    {
        SharedPtr tmp(other);
        swap(tmp);
        return *this;
    }

    SharedPtr& operator=(SharedPtr&& other) noexcept
    {
        SharedPtr tmp(std::move(other));
        swap(tmp);
        return *this;
    }

    SharedPtr& operator=(std::nullptr_t) noexcept
    {
        reset();
        return *this;
    }

    T& operator*() const noexcept { return *m_ptr; }
    T* operator->() const noexcept { return m_ptr; }
    T* get() const noexcept { return m_ptr; }
    explicit operator bool() const noexcept { return m_ptr != nullptr; }

    int useCount() const noexcept
    {
        return m_ctrl ? m_ctrl->refCount.load(std::memory_order_relaxed) : 0;
    }

    void reset() noexcept { SharedPtr().swap(*this); }

    void swap(SharedPtr& other) noexcept
    {
        std::swap(m_ctrl, other.m_ctrl);
        std::swap(m_ptr, other.m_ptr);
    }

    bool operator==(const SharedPtr& other) const noexcept { return m_ptr == other.m_ptr; }
    bool operator!=(const SharedPtr& other) const noexcept { return m_ptr != other.m_ptr; }
    bool operator==(std::nullptr_t) const noexcept { return m_ptr == nullptr; }
    bool operator!=(std::nullptr_t) const noexcept { return m_ptr != nullptr; }

private:
    template<typename U>
    friend class SharedPtr;
    template<typename U>
    friend class WeakPtr;

    template<typename To, typename From>
    friend SharedPtr<To> dynamicPointerCast(const SharedPtr<From>& sp);

    template<typename To, typename From>
    friend SharedPtr<To> staticPointerCast(const SharedPtr<From>& sp);

    template<typename U, typename... Args>
    friend SharedPtr<U> makeShared(Args&&... args);

    SharedPtr(detail::ControlBlock* ctrl, T* ptr) noexcept : m_ctrl(ctrl), m_ptr(ptr) {}

    detail::ControlBlock* m_ctrl;
    T* m_ptr;
};

template<typename T>
class WeakPtr {
public:
    WeakPtr() noexcept : m_ctrl(nullptr), m_ptr(nullptr) {}

    WeakPtr(const SharedPtr<T>& sp) noexcept : m_ctrl(sp.m_ctrl), m_ptr(sp.m_ptr)
    {
        if (m_ctrl) m_ctrl->addWeak();
    }

    WeakPtr(const WeakPtr& other) noexcept : m_ctrl(other.m_ctrl), m_ptr(other.m_ptr)
    {
        if (m_ctrl) m_ctrl->addWeak();
    }

    WeakPtr(WeakPtr&& other) noexcept : m_ctrl(other.m_ctrl), m_ptr(other.m_ptr)
    {
        other.m_ctrl = nullptr;
        other.m_ptr = nullptr;
    }

    ~WeakPtr()
    {
        if (m_ctrl) m_ctrl->releaseWeak();
    }

    WeakPtr& operator=(const WeakPtr& other) noexcept
    {
        WeakPtr tmp(other);
        swap(tmp);
        return *this;
    }

    WeakPtr& operator=(WeakPtr&& other) noexcept
    {
        WeakPtr tmp(std::move(other));
        swap(tmp);
        return *this;
    }

    WeakPtr& operator=(const SharedPtr<T>& sp) noexcept
    {
        WeakPtr tmp(sp);
        swap(tmp);
        return *this;
    }

    bool expired() const noexcept
    {
        return !m_ctrl || m_ctrl->refCount.load(std::memory_order_relaxed) == 0;
    }

    SharedPtr<T> lock() const noexcept
    {
        if (m_ctrl && m_ctrl->tryAddRef())
            return SharedPtr<T>(m_ctrl, m_ptr);
        return SharedPtr<T>();
    }

    void reset() noexcept { WeakPtr().swap(*this); }

    void swap(WeakPtr& other) noexcept
    {
        std::swap(m_ctrl, other.m_ctrl);
        std::swap(m_ptr, other.m_ptr);
    }

private:
    template<typename U>
    friend class SharedPtr;
    template<typename U>
    friend class WeakPtr;

    detail::ControlBlock* m_ctrl;
    T* m_ptr;
};

template<typename T, typename... Args>
SharedPtr<T> makeShared(Args&&... args)
{
    auto* block = new detail::ControlBlockInline<T>();
    try {
        new (block->get()) T(std::forward<Args>(args)...);
    } catch (...) {
        delete block;
        throw;
    }
    return SharedPtr<T>(static_cast<detail::ControlBlock*>(block), block->get());
}

template<typename To, typename From>
SharedPtr<To> dynamicPointerCast(const SharedPtr<From>& sp)
{
    if (auto* p = dynamic_cast<To*>(sp.m_ptr)) {
        if (sp.m_ctrl) sp.m_ctrl->addRef();
        return SharedPtr<To>(sp.m_ctrl, p);
    }
    return SharedPtr<To>();
}

template<typename To, typename From>
SharedPtr<To> staticPointerCast(const SharedPtr<From>& sp)
{
    auto* p = static_cast<To*>(sp.m_ptr);
    if (sp.m_ctrl && p) sp.m_ctrl->addRef();
    return SharedPtr<To>(sp.m_ctrl, p);
}

}  // namespace Kosma::Core

#endif  // KOSMA_KSSHAREDPTR_H
