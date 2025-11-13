//
// Created by Jun on 2025/10/7.
//

#ifndef KOSMA_KSSTRINGVIEW_H
#define KOSMA_KSSTRINGVIEW_H

#include "String/KSChar.h"

namespace Kosma::Core {

class String;

class StringView {
public:
    using value_type = const Char;
    using pointer = const Char*;
    using const_pointer = const Char*;
    using reference = const Char&;
    using const_reference = const Char&;
    using const_iterator = const Char*;
    using iterator = const_iterator;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using reverse_iterator = const_reverse_iterator;
    using size_type = size_t;

    static constexpr size_t kNotFound = static_cast<size_t>(-1);

    StringView() noexcept
        : m_data(nullptr)
        , m_size(0)
    {
    }

    StringView(const Char* data, size_t size)
        : m_data(data)
        , m_size(size)
    {
    }

    StringView(const String& str);

    const Char* data() const noexcept { return m_data; }

    size_t size() const noexcept { return m_size; }

    size_t length() const noexcept { return m_size; }

    bool empty() const noexcept { return m_size == 0; }

    const_iterator begin() const noexcept { return m_data; }

    const_iterator end() const noexcept { return m_data + m_size; }

    const_iterator cbegin() const noexcept { return m_data; }

    const_iterator cend() const noexcept { return m_data + m_size; }

    const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }

    const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }

    const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(cend()); }

    const_reverse_iterator crend() const noexcept { return const_reverse_iterator(cbegin()); }

    const Char& operator[](size_t pos) const { return m_data[pos]; }

    const Char& at(size_t pos) const
    {
        if (pos >= m_size)
            throw std::out_of_range("StringView::at");
        return m_data[pos];
    }

    void remove_prefix(size_t n)
    {
        if (n > m_size)
            n = m_size;
        m_data += n;
        m_size -= n;
    }

    void remove_suffix(size_t n)
    {
        if (n > m_size)
            n = m_size;
        m_size -= n;
    }

    StringView substr(size_t pos, size_t n = kNotFound) const
    {
        if (pos > m_size)
            throw std::out_of_range("StringView::substr");
        if (n == kNotFound || pos + n > m_size)
            n = m_size - pos;
        return {m_data + pos, n};
    }

    int compare(const StringView& other) const noexcept;

    bool starts_with(const StringView& sv) const noexcept;

    bool ends_with(const StringView& sv) const noexcept;

    bool contains(const StringView& sv) const noexcept;

    size_t find(const StringView& sv, size_t pos = 0) const noexcept;

    size_t rfind(const StringView& sv, size_t pos = kNotFound) const noexcept;

    bool operator==(const StringView& rhs) const noexcept;

    bool operator!=(const StringView& rhs) const noexcept;

    bool operator<(const StringView& rhs) const noexcept;

    bool operator>(const StringView& rhs) const noexcept;

    bool operator<=(const StringView& rhs) const noexcept;

    bool operator>=(const StringView& rhs) const noexcept;

    String to_string() const;

private:
    const Char* m_data;
    size_t m_size;
};

}

#endif  // KOSMA_KSSTRINGVIEW_H