//
// Created by Jun on 2025/8/30.
//

#ifndef KOSMA_KSCHARTYPES_H
#define KOSMA_KSCHARTYPES_H

#include <string>

namespace Kosma::Core {

using CChar = char;
using WChar = wchar_t;
using U8Char = char8_t;
using U16Char = char16_t;
using U32Char = char32_t;

using CString = std::string;
using WString = std::wstring;
using U8String = std::u8string;
using U16String = std::u16string;
using U32String = std::u32string;

using CStringView = std::string_view;
using WStringView = std::wstring_view;
using U8StringView = std::u8string_view;
using U16StringView = std::u16string_view;
using U32StringView = std::u32string_view;

}

#endif  // KOSMA_KSCHARTYPES_H