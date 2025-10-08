//
// Created by Jun on 2025/8/30.
//

#ifndef KOSMA_KSSTRING_H
#define KOSMA_KSSTRING_H

#include "KsCoreAPI.h"
#include "String/KSChar.h"
#include "String/KSStringView.h"

namespace Kosma::Core {

class KS_CORE_API String {
public:
    // Iterator types
    using iterator = Char*;
    using const_iterator = const Char*;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    static constexpr size_t kNotFound = static_cast<size_t>(-1);

    String() = default;

    explicit String(const CStringView& str)
        : String(str.data(), str.length())
    {
    }

    explicit String(const U8StringView& str)
        : String(str.data(), str.length())
    {
    }

    explicit String(const U16StringView& str)
        : String(str.data(), str.length())
    {
    }

    explicit String(const U32StringView& str)
        : String(str.data(), str.length())
    {
    }

    explicit String(const WStringView& str)
        : String(str.data(), str.length())
    {
    }

    String(const Char* str);

    String(const CChar* str);

    String(const U8Char* str);

    String(const U16Char* str);

    String(const U32Char* str);

    String(const WChar* str);

    String(const Char* begin, size_t len);

    String(const CChar* begin, size_t len);

    String(const U8Char* begin, size_t len);

    String(const U16Char* begin, size_t len);

    String(const U32Char* begin, size_t len);

    String(const WChar* begin, size_t len);

    String(const String& other);

    String(String&& other) noexcept;

    ~String();

    bool isEmpty() const;

    void clear();

    void swap(String& other) noexcept;

    size_t length() const;

    size_t charCount() const;

    size_t capacity() const;

    void shrinkToFit();

    iterator begin() { return getData(); }

    const_iterator begin() const { return getData(); }

    const_iterator cbegin() const { return getData(); }

    iterator end() { return getData() + length(); }

    const_iterator end() const { return getData() + length(); }

    const_iterator cend() const { return getData() + length(); }

    reverse_iterator rbegin() { return reverse_iterator(end()); }

    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }

    const_reverse_iterator crbegin() const { return const_reverse_iterator(cend()); }

    reverse_iterator rend() { return reverse_iterator(begin()); }

    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

    const_reverse_iterator crend() const { return const_reverse_iterator(cbegin()); }

    Char& operator[](size_t pos);

    const Char& operator[](size_t pos) const;

    Char& at(size_t pos);

    const Char& at(size_t pos) const;

    const Char* data() const noexcept;

    Char* data() noexcept;

    const Char* c_str() const noexcept;

    String& operator+=(const String& str);

    String& operator+=(const Char* s);

    String& operator+=(Char c);

    String& append(const String& str);

    String& append(const Char* s);

    String& append(const Char* s, size_t n);

    void pushBack(Char c);

    void popBack();

    String& insert(size_t pos, const String& str);

    String& insert(size_t pos, const Char* s);

    String& insert(size_t pos, const Char* s, size_t n);

    String& erase(size_t pos = 0, size_t len = kNotFound);

    String& replace(size_t pos, size_t len, const String& str);

    String substr(size_t pos = 0, size_t len = kNotFound) const;

    void resize(size_t n);

    void resize(size_t n, Char c);

    void reserve(size_t new_cap);

    int compare(const String& str) const noexcept;

    int compare(const Char* s) const noexcept;

    bool startsWith(const String& str) const noexcept;

    bool endsWith(const String& str) const noexcept;

    bool contains(const String& str) const noexcept;

    size_t find(const String& str, size_t pos = 0) const noexcept;

    size_t find(const Char* s, size_t pos = 0) const noexcept;

    size_t rfind(const String& str, size_t pos = kNotFound) const noexcept;

    size_t rfind(const Char* s, size_t pos = kNotFound) const noexcept;

    // Operators

    String& operator=(const String& other);

    String& operator=(String&& other) noexcept;

    String& operator=(const Char* s);

    bool operator==(const String& rhs) const noexcept;

    bool operator!=(const String& rhs) const noexcept;

    bool operator<(const String& rhs) const noexcept;

    bool operator>(const String& rhs) const noexcept;

    bool operator<=(const String& rhs) const noexcept;

    bool operator>=(const String& rhs) const noexcept;

protected:
    static constexpr size_t kBufferSize = 11;  // SSO buffer size

    static constexpr size_t capacityOfSSO() { return kBufferSize - 1; }

    static constexpr bool needHeapStorage(size_t len) { return len > capacityOfSSO(); }

    bool isSSOMode() const { return !sso.isLong; }

    bool isHeapMode() const { return heap.isLong; }

    bool isCharCountDirty() const { return charCount_.isDirty; }

    void setCharCountDirty() const { charCount_.isDirty = 1; }

    void clearCharCountDirty() const { charCount_.isDirty = 0; }

    void setHeapLength(size_t len) { heap.length = len; }

    void setSSOLength(size_t len) { sso.size = static_cast<uint8_t>(len); }

    void setLength(size_t len);

    void updateCharCount() const;

    void swapWith(String& other);

    const Char* getData() const { return isHeapMode() ? heap.data : sso.buffer; }

    Char* getData() { return const_cast<Char*>(std::as_const(*this).getData()); }

    Char* initSpaceFor(size_t len);

    Char* ensureSpaceFor(size_t len);

    void reallocateHeapFor(size_t size, bool keepContent);

private:
    union {
        struct {  // 堆字符串
            Char* data;
            size_t length;
            size_t capacity : 63;
            size_t isLong : 1;
        } heap;

        struct {                       // SSO 短字符串
            Char buffer[kBufferSize];  // 存储短字符串
            uint8_t padding;
            uint8_t size : 7;
            uint8_t isLong : 1;
        } sso {};
    };

    struct CharCount {
        size_t count : 63;
        size_t isDirty : 1;
    } mutable charCount_ {0, 1};
};

KS_CORE_API String operator+(const String& lhs, const String& rhs);

}

#endif  // KOSMA_KSSTRING_H