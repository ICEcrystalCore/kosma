//
// Created by Jun on 2025/8/30.
//

#ifndef KOSMA_KSOPTIONAL_H
#define KOSMA_KSOPTIONAL_H

#include <optional>
#include <stdexcept>

namespace Kosma::Core {

template<typename T>
class Optional {
public:
    // Default constructor (indicating no value)
    Optional(): m_hasValue(false) {}

    Optional(std::nullopt_t): m_hasValue(false) {}

    // Constructor that accepts a value
    Optional(const T& value): m_hasValue(true)
    {
        new (&m_storage) T(value);  // Construct the value in the union memory
    }

    Optional(T&& value): m_hasValue(true)
    {
        new (&m_storage) T(std::move(value));  // Move construct the value
    }

    Optional(const std::optional<T>& opt)
    {
        if (opt.has_value()) {
            m_hasValue = true;
            new (&m_storage) T(*opt);
        } else {
            m_hasValue = false;
        }
    }

    Optional(std::optional<T>&& opt)
    {
        if (opt.has_value()) {
            m_hasValue = true;
            new (&m_storage) T(std::move(*opt));
        } else {
            m_hasValue = false;
        }
    }

    // Destructor to release the stored value
    ~Optional() { reset(); }

    // Disable copy constructor and copy assignment
    Optional(const Optional&) = delete;
    Optional& operator=(const Optional&) = delete;

    // Move constructor and move assignment
    Optional(Optional&& other) noexcept: m_hasValue(other.m_hasValue)
    {
        if (m_hasValue) {
            new (&m_storage) T(std::move(other.value()));
        }
    }

    Optional& operator=(Optional&& other) noexcept
    {
        if (this != &other) {
            reset();
            m_hasValue = other.m_hasValue;
            if (m_hasValue) {
                new (&m_storage) T(std::move(other.value()));
            }
        }
        return *this;
    }

    // Check if the value exists
    bool hasValue() const { return m_hasValue; }

    // Get the stored value, throws exception if no value
    T& value()
    {
        if (!m_hasValue) {
            throw std::runtime_error("No value present");
        }
        return *reinterpret_cast<T*>(&m_storage);
    }

    const T& value() const
    {
        if (!m_hasValue) {
            throw std::runtime_error("No value present");
        }
        return *reinterpret_cast<const T*>(&m_storage);
    }

    // Return the value if present, otherwise return the provided default value
    T& valueOr(T&& defaultValue)
    {
        if (m_hasValue) {
            return value();
        }
        return defaultValue;
    }

    // Reset the optional, releasing the stored value
    void reset()
    {
        if (m_hasValue) {
            value().~T();  // Explicitly call the destructor
            m_hasValue = false;
        }
    }

    std::optional<T> toStdOptional() const
    {
        if (m_hasValue) {
            return value();
        }
        return std::nullopt;
    }

    explicit operator bool() const { return hasValue(); }

    T& operator*() { return value(); }

    const T& operator*() const { return value(); }

private:
    bool m_hasValue;                                // Flag indicating whether the value exists
    alignas(T) unsigned char m_storage[sizeof(T)];  // Memory area to store the value
};

}

#endif  // KOSMA_KSOPTIONAL_H