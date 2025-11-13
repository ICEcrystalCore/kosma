//
// Created by Jun on 2025/11/5.
//

#ifndef KOSMA_KSSTRINGUTILS_H
#define KOSMA_KSSTRINGUTILS_H

#include "String/KSChar.h"

namespace Kosma::Core {
template<typename CharType>
size_t getLengthOf(const CharType* str)
{
    if (!str)
        return 0;
    size_t length = 0;
    for (;; ++length) {
        if (str[length] == CharType {})
            break;
    }
    return length;
}

int compareBetween(const Char* s1, size_t len1, const Char* s2, size_t len2);

}

#endif  // KOSMA_KSSTRINGUTILS_H