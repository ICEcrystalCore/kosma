//
// Created by Jun on 2025/8/31.
//

#ifndef KOSMA_KSCHARSETCODEC_H
#define KOSMA_KSCHARSETCODEC_H

#include "Memory/KSUniquePtr.h"
#include "String/KSCharTypes.h"

namespace Kosma::Core {

template<typename T>
struct From {
    const T* data;
    size_t length;
};

template<typename T>
struct To {
    UniquePtr<T[]> data;
    size_t length;
};

To<U16Char> toU16(const From<U8Char>& u8);

To<U16Char> toU16(const From<U32Char>& u32);

To<U16Char> toU16(const From<CChar>& c);

To<U16Char> toU16(const From<WChar>& w);

To<U8Char> toU8(const From<U16Char>& u16);

To<U32Char> toU32(const From<U16Char>& u16);

To<CChar> toU8C(const From<U16Char>& u16);

To<WChar> toWide(const From<U16Char>& u16);

}

#endif  // KOSMA_KSCHARSETCODEC_H