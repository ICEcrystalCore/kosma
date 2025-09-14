//
// Created by Jun on 2025/8/30.
//

#ifndef KOSMA_KSSTRING_H
#define KOSMA_KSSTRING_H

#include "KsCoreAPI.h"
#include "String/KSChar.h"

namespace Kosma::Core {

class KS_CORE_API String {
public:
    String();

    String(const CStringView& str)
        : String(str.data(), str.length())
    {
    }

    String(const U8StringView& str)
        : String(str.data(), str.length())
    {
    }

    String(const U16StringView& str)
        : String(str.data(), str.length())
    {
    }

    String(const U32StringView& str)
        : String(str.data(), str.length())
    {
    }

    String(const WStringView& str)
        : String(str.data(), str.length())
    {
    }

    String(const CChar* begin, size_t len);

    String(const U8Char* begin, size_t len);

    String(const U16Char* begin, size_t len);

    String(const U32Char* begin, size_t len);

    String(const WChar* begin, size_t len);

    String(const String& other);

    String(String&& other) noexcept;

    ~String();

    bool isEmpty() const;

    size_t length() const;

    size_t charCount() const;

    size_t capacity() const;

protected:
    static constexpr size_t kBufferSize = 11;  // SSO buffer size

    static constexpr bool isTooLongForSSO(size_t len) { return len + 1 > 11; }

    bool isHeapMode() const { return heap.isLong; }

    bool isCharCountDirty() const { return m_isDirty; }

    void setCharCountDirty() const { m_isDirty = 1; }

    const Char* getData() const { return isHeapMode() ? heap.data : sso.buffer; }

    void updateCharCount() const;

    void adjustCapacityTo(size_t len, bool copyContent);

    Char* getData() { return const_cast<Char*>(std::as_const(*this).getData()); }

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
        } sso;
    };

    mutable size_t m_charCount : 63;
    mutable size_t m_isDirty : 1;
};

}

#endif  // KOSMA_KSSTRING_H