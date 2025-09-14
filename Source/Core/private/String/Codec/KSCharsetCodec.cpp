//
// Created by Jun on 2025/8/31.
//

#include "String/Codec/KSCharsetCodec.h"

#include "unicode/ucnv.h"
#include "unicode/ustring.h"
#include "unicode/utypes.h"

namespace Kosma::Core {

constexpr size_t kWcharSize = sizeof(WChar);

To<U16Char> toU16(const From<U8Char>& u8)
{
    int32_t utf16Length = 0;
    UErrorCode status = U_ZERO_ERROR;

    auto& [u8Str, u8Len] = u8;
    // preflight
    u_strFromUTF8(nullptr, 0, &utf16Length, (const char*)u8Str, (int32_t)u8Len, &status);
    if (status != U_BUFFER_OVERFLOW_ERROR || utf16Length == 0) {
        return {};
    }

    auto destBuffer = new U16Char[utf16Length];
    // convert
    U16String utf16Str(utf16Length, 0);
    status = U_ZERO_ERROR;
    u_strFromUTF8((UChar*)destBuffer,
                  utf16Length,
                  nullptr,
                  (const char*)u8Str,
                  (int32_t)u8Len,
                  &status);

    return {destBuffer, (size_t)utf16Length};
}

To<U16Char> toU16(const From<U32Char>& u32)
{
    int32_t utf16Length = 0;
    UErrorCode status = U_ZERO_ERROR;

    auto& [u32Str, u32Len] = u32;

    u_strFromUTF32(nullptr, 0, &utf16Length, (const UChar32*)u32Str, (int32_t)u32Len, &status);

    if (status != U_BUFFER_OVERFLOW_ERROR || utf16Length == 0) {
        return {};
    }

    auto destBuffer = new U16Char[utf16Length];

    status = U_ZERO_ERROR;
    u_strFromUTF32((UChar*)destBuffer,
                   (int32_t)utf16Length,
                   &utf16Length,
                   (const UChar32*)u32Str,
                   (int32_t)u32Len,
                   &status);

    return {destBuffer, (size_t)utf16Length};
}

To<U16Char> toU16(const From<CChar>& c)
{
    From u8 = {(U8Char*)c.data, c.length};
    return toU16(u8);
}

To<U16Char> toU16(const From<WChar>& w)
{
    if constexpr (2 == kWcharSize) {
        auto destBuffer = new U16Char[w.length];
        To<U16Char> u16 = {destBuffer, w.length};
        return u16;
    } else {
        From<U32Char> u32 = {(U32Char*)w.data, w.length};
        return toU16(u32);
    }
}

To<U8Char> toU8(const From<U16Char>& u16)
{
    auto& [u16str, u16Len] = u16;
    if (!u16Len) {
        return {};
    }

    int32_t u8Len = 0;
    UErrorCode status = U_ZERO_ERROR;
    u_strToUTF8(nullptr, 0, &u8Len, u16str, (int32_t)u16Len, &status);

    if (status != U_BUFFER_OVERFLOW_ERROR || u8Len == 0) {
        return {};
    }

    auto destBuffer = new U8Char[u8Len];

    status = U_ZERO_ERROR;
    u_strToUTF8((char*)destBuffer, u8Len, nullptr, u16str, (int32_t)u16Len, &status);

    return {destBuffer, (size_t)u8Len};
}

To<U32Char> toU32(const From<U16Char>& u16)
{
    auto& [u16str, u16Len] = u16;

    int32_t u32Length = 0;
    UErrorCode status = U_ZERO_ERROR;

    // preflight
    u_strToUTF32(nullptr, 0, &u32Length, u16str, (int32_t)u16Len, &status);

    if (status != U_BUFFER_OVERFLOW_ERROR || u32Length == 0) {
        return {};
    }

    auto destBuffer = new U32Char[u32Length];
    // convert
    U32String utf32Str(u32Length, 0);
    status = U_ZERO_ERROR;
    u_strToUTF32((UChar32*)utf32Str.data(), u32Length, nullptr, u16str, (int32_t)u16Len, &status);

    return {destBuffer, (size_t)u32Length};
}

To<CChar> toU8C(const From<U16Char>& u16)
{
    auto [u8data, u8Len] = toU8(u16);
    return {(CChar*)u8data.release(), u8Len};
}

To<WChar> toWide(const From<U16Char>& u16)
{
    if constexpr (2 == kWcharSize) {
        auto destBuffer = new U16Char[u16.length];
        return {(WChar*)destBuffer, u16.length};
    } else {
        auto [u32data, u32Len] = toU32(u16);
        return {(WChar*)u32data.release(), u32Len};
    }
}

}