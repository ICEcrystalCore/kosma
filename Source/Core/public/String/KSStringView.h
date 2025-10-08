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

    StringView() noexcept
        : data_(nullptr)
        , size_(0)
    {
    }

    StringView(const Char* data, size_t size)
        : data_(data)
        , size_(size)
    {
    }

    StringView(const String& str);

    const Char* data() const noexcept { return data_; }

    size_t size() const noexcept { return size_; }

    size_t length() const noexcept { return size_; }

    bool empty() const noexcept { return size_ == 0; }

    const_iterator begin() const noexcept { return data_; }

    const_iterator end() const noexcept { return data_ + size_; }

    const_iterator cbegin() const noexcept { return data_; }

    const_iterator cend() const noexcept { return data_ + size_; }

    const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }

    const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }

    const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(cend()); }

    const_reverse_iterator crend() const noexcept { return const_reverse_iterator(cbegin()); }

    const Char& operator[](size_t pos) const { return data_[pos]; }

    const Char& at(size_t pos) const
    {
        if (pos >= size_)
            throw std::out_of_range("StringView::at");
        return data_[pos];
    }

    void remove_prefix(size_t n)
    {
        if (n > size_)
            n = size_;
        data_ += n;
        size_ -= n;
    }

    void remove_suffix(size_t n)
    {
        if (n > size_)
            n = size_;
        size_ -= n;
    }

    StringView substr(size_t pos, size_t n = npos) const
    {
        if (pos > size_)
            throw std::out_of_range("StringView::substr");
        if (n == npos || pos + n > size_)
            n = size_ - pos;
        return {data_ + pos, n};
    }

    int compare(const StringView& other) const noexcept;
    bool starts_with(const StringView& sv) const noexcept;
    bool ends_with(const StringView& sv) const noexcept;
    bool contains(const StringView& sv) const noexcept;
    size_t find(const StringView& sv, size_t pos = 0) const noexcept;
    size_t rfind(const StringView& sv, size_t pos = npos) const noexcept;

    bool operator==(const StringView& rhs) const noexcept;
    bool operator!=(const StringView& rhs) const noexcept;
    bool operator<(const StringView& rhs) const noexcept;
    bool operator>(const StringView& rhs) const noexcept;
    bool operator<=(const StringView& rhs) const noexcept;
    bool operator>=(const StringView& rhs) const noexcept;

    String to_string() const;
    static constexpr size_t npos = static_cast<size_t>(-1);

private:
    const Char* data_;
    size_t size_;
};

}

#endif  // KOSMA_KSSTRINGVIEW_H