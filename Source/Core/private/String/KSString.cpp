//
// Created by Jun on 2025/8/17.
//

#include "String/KSString.h"

#include "String/Codec/KSCharsetCodec.h"
#include "String/KSStringUtils.h"

namespace Kosma::Core {

static size_t calcHeapSize(size_t len)
{
    size_t required = len + 1;
    size_t idealSize = 16;
    while (idealSize < required) {
        idealSize *= 2;
    }
    return idealSize;
}

String::String(const Char* str)
    : String(str, getLengthOf(str))
{
}

String::String(const CChar* str)
    : String(str, getLengthOf(str))
{
}

String::String(const U8Char* str)
    : String(str, getLengthOf(str))
{
}

String::String(const U16Char* str)
    : String(str, getLengthOf(str))
{
}

String::String(const U32Char* str)
    : String(str, getLengthOf(str))
{
}

String::String(const WChar* str)
    : String(str, getLengthOf(str))
{
}

String::String(const Char* begin, size_t len)
{
    memcpy(initSpaceFor(len), begin, len * sizeof(Char));
    setLength(len);
}

String::String(const CChar* begin, size_t len)
    : String()
{
    auto [u16, u16Len] = toU16({begin, len});
    if (!u16 || !u16Len)
        return;

    memcpy(initSpaceFor(u16Len), u16.get(), u16Len * sizeof(Char));
    setLength(u16Len);
}

String::String(const U8Char* begin, size_t len)
{
    auto [u16, u16Len] = toU16({begin, len});
    if (!u16 || !u16Len)
        return;

    memcpy(initSpaceFor(u16Len), u16.get(), u16Len * sizeof(Char));
    setLength(u16Len);
}

String::String(const U16Char* begin, size_t len)
{
    memcpy(initSpaceFor(len), begin, len * sizeof(Char));
    setLength(len);
}

String::String(const U32Char* begin, size_t len)
{
    auto [u16, u16Len] = toU16({begin, len});
    if (!u16 || !u16Len)
        return;

    memcpy(initSpaceFor(u16Len), u16.get(), u16Len * sizeof(Char));
    setLength(u16Len);
}

String::String(const WChar* begin, size_t len)
{
    auto [u16, u16Len] = toU16({begin, len});
    if (!u16 || !u16Len)
        return;

    memcpy(initSpaceFor(u16Len), u16.get(), u16Len * sizeof(Char));
    setLength(u16Len);
}

String::String(const String& other)
{
    // copy first
    std::memcpy(&m_heap, &other.m_heap, sizeof(m_heap));
    // copy heap data if needed
    if (isHeapMode()) {
        m_heap.data = new Char[m_heap.capacity + 1];
        std::memcpy(m_heap.data, other.m_heap.data, m_heap.length * sizeof(Char));
    }
}

String::String(String&& other) noexcept
{
    swapWith(other);
}

String::~String()
{
    if (isHeapMode())
        delete[] m_heap.data;
}

bool String::isEmpty() const
{
    return 0 == length();
}

void String::clear()
{
    String().swap(*this);
}

void String::swap(String& other) noexcept
{
    if (&other != this)
        swapWith(other);
}

size_t String::length() const
{
    return isHeapMode() ? m_heap.length : m_sso.size;
}

size_t String::charCount() const
{
    if (isCharCountDirty())
        updateCharCount();
    return m_charCount.count;
}

size_t String::capacity() const
{
    return isHeapMode() ? m_heap.capacity : capacityOfSSO();
}

void String::shrinkToFit()
{
    if (isSSOMode())
        return;
    if (length() == capacity())
        return;
    reallocateHeapFor(length() + 1, true);  /// real size is length + 1 (for null terminator)
}

void String::setLength(size_t len)
{
    isHeapMode() ? setHeapLength(len) : setSSOLength(len);
    setCharCountDirty();
}

void String::updateCharCount() const
{
    size_t newCharCount = 0;
    for (auto ch : *this) {
        if (ch.isLowSurrogate())
            continue;
        newCharCount++;
    }
    m_charCount.count = newCharCount;
    clearCharCountDirty();
}

void String::swapWith(String& other)
{
    std::swap(m_heap, other.m_heap);
    std::swap(m_charCount, other.m_charCount);
}

Char* String::initSpaceFor(size_t len)
{
    if (needHeapStorage(len))
        reallocateHeapFor(calcHeapSize(len), false);
    return getData();
}

Char* String::ensureSpaceFor(size_t len)
{
    if (capacity() >= len)
        return getData();
    reallocateHeapFor(calcHeapSize(len), true);
    return getData();
}

void String::reallocateHeapFor(size_t size, bool keepContent)
{
    if (isHeapMode() && capacity() + 1 == size)
        return;

    Char* newHeap = new Char[size];
    size_t newCapacity = size ? size - 1 : 0;
    size_t newLen = 0;

    if (keepContent) {
        newLen = std::min(newCapacity, length());
        memcpy(newHeap, getData(), newLen * sizeof(Char));
    }

    if (isHeapMode())
        delete[] m_heap.data;

    m_heap.data = newHeap;
    m_heap.capacity = newCapacity;
    m_heap.length = newLen;
    m_heap.isLong = 1;
}

// Element access
Char& String::operator[](size_t pos)
{
    return getData()[pos];
}

const Char& String::operator[](size_t pos) const
{
    return getData()[pos];
}

Char& String::at(size_t pos)
{
    if (pos >= length())
        throw std::out_of_range("String::at");
    return getData()[pos];
}

const Char& String::at(size_t pos) const
{
    if (pos >= length())
        throw std::out_of_range("String::at");
    return getData()[pos];
}

const Char* String::data() const noexcept
{
    return getData();
}

Char* String::data() noexcept
{
    return getData();
}

const Char* String::c_str() const noexcept
{
    return getData();
}

String& String::operator+=(const String& str)
{
    return append(str);
}

String& String::operator+=(const Char* s)
{
    return append(s);
}

String& String::operator+=(Char c)
{
    pushBack(c);
    return *this;
}

String& String::operator+=(CChar c)
{
    pushBack(c);
    return *this;
}

String& String::operator+=(U8Char c)
{
    pushBack(c);
    return *this;
}

String& String::operator+=(U16Char c)
{
    pushBack(c);
    return *this;
}

String& String::operator+=(U32Char c)
{
    pushBack(c);
    return *this;
}

String& String::append(const String& str)
{
    return append(str.data(), str.length());
}

String& String::append(const Char* s)
{
    return append(s, getLengthOf(s));
}

String& String::append(const CChar* s)
{
    return append(s, 1);
}

String& String::append(const U8Char* s)
{
    return append(s, 1);
}

String& String::append(const U16Char* s)
{
    return append(s, 1);
}

String& String::append(const U32Char* s)
{
    return append(s, 1);
}

String& String::append(const Char* s, size_t n)
{
    size_t oldLen = length();
    resize(oldLen + n);
    std::memcpy(getData() + oldLen, s, n * sizeof(Char));
    return *this;
}

String& String::append(const CChar* s, size_t n)
{
    return append(reinterpret_cast<const U8Char*>(s), n);
}

String& String::append(const U8Char* s, size_t n)
{
    auto [u16, u16Len] = toU16({s, n});
    size_t oldLen = length();
    resize(oldLen + u16Len);
    std::memcpy(getData() + oldLen, u16.get(), u16Len * sizeof(Char));
    return *this;
}

String& String::append(const U16Char* s, size_t n)
{
    return append(reinterpret_cast<const Char*>(s), n);
}

String& String::append(const U32Char* s, size_t n)
{
    auto [u16, u16Len] = toU16({s, n});
    size_t oldLen = length();
    resize(oldLen + u16Len);
    std::memcpy(getData() + oldLen, u16.get(), u16Len * sizeof(Char));
    return *this;
}

void String::pushBack(Char c)
{
    append(&c);
}

void String::pushBack(CChar c)
{
    append(&c);
}

void String::pushBack(U8Char c)
{
    append(&c);
}

void String::pushBack(U16Char c)
{
    append(&c);
}

void String::pushBack(U32Char c)
{
    append(&c);
}

void String::popBack()
{
    if (!isEmpty())
        resize(length() - 1);
}

String& String::insert(size_t pos, const String& str)
{
    return insert(pos, str.data(), str.length());
}

String& String::insert(size_t pos, const Char* s)
{
    return insert(pos, s, getLengthOf(s));
}

String& String::insert(size_t pos, const CChar* s)
{
    return insert(pos, s, getLengthOf(s));
}

String& String::insert(size_t pos, const U8Char* s)
{
    return insert(pos, s, getLengthOf(s));
}

String& String::insert(size_t pos, const U16Char* s)
{
    return insert(pos, s, getLengthOf(s));
}

String& String::insert(size_t pos, const U32Char* s)
{
    return insert(pos, s, getLengthOf(s));
}

String& String::insert(size_t pos, const Char* s, size_t n)
{
    if (pos > length())
        throw std::out_of_range("String::insert");
    size_t oldLen = length();
    resize(oldLen + n);
    std::memmove(getData() + pos + n, getData() + pos, (oldLen - pos) * sizeof(Char));
    std::memcpy(getData() + pos, s, n * sizeof(Char));
    return *this;
}

String& String::insert(size_t pos, const CChar* s, size_t n)
{
    return insert(pos, reinterpret_cast<const U8Char*>(s), n);
}

String& String::insert(size_t pos, const U8Char* s, size_t n)
{
    if (pos > length())
        throw std::out_of_range("String::insert");
    auto [u16, u16Len] = toU16({s, n});
    size_t oldLen = length();
    resize(oldLen + n);
    std::memmove(getData() + pos + u16Len, getData() + pos, (oldLen - pos) * sizeof(Char));
    std::memcpy(getData() + pos, u16.get(), u16Len * sizeof(Char));
    return *this;
}

String& String::insert(size_t pos, const U16Char* s, size_t n)
{
    return insert(pos, reinterpret_cast<const Char*>(s), n);
}

String& String::insert(size_t pos, const U32Char* s, size_t n)
{
    if (pos > length())
        throw std::out_of_range("String::insert");
    auto [u16, u16Len] = toU16({s, n});
    size_t oldLen = length();
    resize(oldLen + u16Len);
    std::memmove(getData() + pos + u16Len, getData() + pos, (oldLen - pos) * sizeof(Char));
    std::memcpy(getData() + pos, u16.get(), u16Len * sizeof(Char));
    return *this;
}

String& String::erase(size_t pos, size_t len)
{
    if (pos > length())
        throw std::out_of_range("String::erase");
    if (len == kNotFound || pos + len > length())
        len = length() - pos;
    std::memmove(getData() + pos, getData() + pos + len, (length() - pos - len) * sizeof(Char));
    resize(length() - len);
    return *this;
}

String& String::replace(size_t pos, size_t len, const String& str)
{
    erase(pos, len);
    insert(pos, str);
    return *this;
}

String String::substr(size_t pos, size_t len) const
{
    if (pos > length())
        throw std::out_of_range("String::substr");
    if (len == kNotFound || pos + len > length())
        len = length() - pos;
    return {getData() + pos, len};
}

void String::resize(size_t n)
{
    resize(n, {});
}

void String::resize(size_t n, Char c)
{
    if (n < length()) {
        setLength(n);
    } else if (n > length()) {
        ensureSpaceFor(n);
        std::fill(getData() + length(), getData() + n, c);
        setLength(n);
    }
}

void String::resize(size_t n, CChar c)
{
    resize(n, Char(c));
}

void String::reserve(size_t new_cap)
{
    if (new_cap > capacity())
        reallocateHeapFor(new_cap, true);
}

int String::compare(const String& other) const noexcept
{
    return compareBetween(getData(), length(), other.getData(), other.length());
}

int String::compare(const Char* other) const noexcept
{
    return compareBetween(getData(), length(), other, getLengthOf(other));
}

bool String::startsWith(const String& str) const noexcept
{
    if (length() < str.length())
        return false;
    return 0 == compareBetween(getData(), str.length(), str.getData(), str.length());
}

bool String::endsWith(const String& str) const noexcept
{
    if (length() < str.length())
        return false;
    return 0 == compareBetween(end() - str.length(), str.length(), str.getData(), str.length());
}

bool String::contains(const String& str) const noexcept
{
    return find(str) != kNotFound;
}

size_t String::find(const String& str, size_t pos) const noexcept
{
    if (str.length() == 0 || str.length() > length() || pos > length() - str.length())
        return kNotFound;
    for (size_t i = pos; i <= length() - str.length(); ++i) {
        if (!compareBetween(getData() + i, str.length(), str.getData(), str.length()))
            return i;
    }
    return kNotFound;
}

size_t String::find(const Char* s, size_t pos) const noexcept
{
    return find(String(s), pos);
}

size_t String::rfind(const String& str, size_t pos) const noexcept
{
    if (str.length() == 0 || str.length() > length())
        return kNotFound;
    size_t end = std::min(pos, length() - str.length());
    for (size_t i = end + 1; i-- > 0;) {
        if (!compareBetween(getData() + i, str.length(), str.getData(), str.length()))
            return i;
    }
    return kNotFound;
}

size_t String::rfind(const Char* s, size_t pos) const noexcept
{
    return rfind(String(s), pos);
}

String& String::operator=(const String& other)
{
    if (this != &other)
        String(other).swap(*this);
    return *this;
}

String& String::operator=(String&& other) noexcept
{
    swapWith(other);
    return *this;
}

String& String::operator=(const Char* s)
{
    String(s).swap(*this);
    return *this;
}

bool String::operator==(const String& rhs) const noexcept
{
    return compare(rhs) == 0;
}

bool String::operator!=(const String& rhs) const noexcept
{
    return compare(rhs) != 0;
}

bool String::operator<(const String& rhs) const noexcept
{
    return compare(rhs) < 0;
}

bool String::operator>(const String& rhs) const noexcept
{
    return compare(rhs) > 0;
}

bool String::operator<=(const String& rhs) const noexcept
{
    return compare(rhs) <= 0;
}

bool String::operator>=(const String& rhs) const noexcept
{
    return compare(rhs) >= 0;
}

StringView String::toView() const
{
    return {getData(), length()};
}

}