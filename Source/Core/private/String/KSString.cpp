//
// Created by Jun on 2025/8/17.
//

#include "String/KSString.h"

#include "String/Codec/KSCharsetCodec.h"

namespace Kosma::Core {

String::String()
    : sso({}, 0, 0)
    , m_charCount(0)
    , m_isDirty(0)
{
}

String::String(const CChar* begin, size_t len)
    : String()
{
    auto [u16, u16Len] = toU16({begin, len});
    if (!u16 || !u16Len)
        return;

    adjustCapacityTo(u16Len, false);
    memcpy(getData(), u16.get(), u16Len * sizeof(Char));
    setCharCountDirty();
}

String::String(const U8Char* begin, size_t len)
{
    auto [u16, u16Len] = toU16({begin, len});
    if (!u16 || !u16Len)
        return;

    adjustCapacityTo(u16Len, false);
    memcpy(getData(), u16.get(), u16Len * sizeof(Char));
    setCharCountDirty();
}

String::String(const U16Char* begin, size_t len)
{
    adjustCapacityTo(len, false);
    memcpy(getData(), begin, len * sizeof(Char));
    setCharCountDirty();
}

String::String(const U32Char* begin, size_t len)
{
    auto [u16, u16Len] = toU16({begin, len});
    if (!u16 || !u16Len)
        return;

    adjustCapacityTo(u16Len, false);
    memcpy(getData(), u16.get(), u16Len * sizeof(Char));
    setCharCountDirty();
}

String::String(const WChar* begin, size_t len)
{
    auto [u16, u16Len] = toU16({begin, len});
    if (!u16 || !u16Len)
        return;

    adjustCapacityTo(u16Len, false);
    memcpy(getData(), u16.get(), u16Len * sizeof(Char));
    setCharCountDirty();
}

String::String(const String& other)
    : m_charCount(other.m_charCount)
    , m_isDirty(other.m_isDirty)
{
    // copy first
    std::memcpy(&heap, &other.heap, sizeof(heap));
    // copy heap data if needed
    if (heap.isLong && heap.length) {
        heap.data = new Char[heap.capacity];
        std::memcpy(heap.data, other.heap.data, heap.length * sizeof(Char));
    }
}

String::String(String&& other) noexcept
    : m_charCount(other.m_charCount)
    , m_isDirty(other.m_isDirty)
{
    // copy first
    std::memcpy(&heap, &other.heap, sizeof(heap));
    // nullify other if needed
    if (heap.isLong) {
        std::memset(&other.heap, 0, sizeof(other.heap));  // clear other heap info
        other.m_charCount = 0;
        other.m_isDirty = 0;
    }
}

String::~String()
{
    if (isHeapMode())
        delete[] heap.data;
}

bool String::isEmpty() const
{
    return 0 == length();
}

size_t String::length() const
{
    return isHeapMode() ? heap.length : sso.size;
}

size_t String::charCount() const
{
    if (isCharCountDirty())
        updateCharCount();
    return m_charCount;
}

size_t String::capacity() const
{
    return isHeapMode() ? heap.capacity : kBufferSize - 1;
}

static size_t calculateHeapCapacity(size_t len)
{
    size_t min = len + 1;
    size_t cap = 16;
    while (cap < min) {
        cap *= 2;
    }
    return cap;
}

void String::updateCharCount() const
{
    size_t newCharCount = 0;
    auto data = getData();
    for (auto i = 0; i < length(); ++i) {
        auto& ch = data[i];
        newCharCount += ch.isHighSurrogate() ? 2 : 1;
    }
    m_charCount = newCharCount;
    m_isDirty = 0;
}

void String::adjustCapacityTo(size_t len, bool copyContent)
{
    bool needHeap = isTooLongForSSO(len);
    bool isHeap = isHeapMode();

    if (needHeap != isHeap) {
        auto toHeap = !isHeap;
        if (toHeap) {
            auto newCapacity = calculateHeapCapacity(len);
            auto newHeap = new Char[newCapacity];

            if (copyContent)
                memcpy(newHeap, heap.data, heap.length * sizeof(Char));

            heap.data = newHeap;
            heap.length = len;
            heap.capacity = newCapacity - 1;
            heap.isLong = true;
        } else {  // to SSO
            auto heapLen = heap.length;
            auto heapData = heap.data;
            size_t contentLen = 0;

            if (copyContent) {
                contentLen = std::min(heapLen, kBufferSize - 1);
                memcpy(sso.buffer, heapData, contentLen * sizeof(Char));
            }

            delete[] heapData;
            sso.size = contentLen;
            sso.isLong = false;
        }
    }
}

}