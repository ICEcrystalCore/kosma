//
// Created by Jun on 2025/11/5.
//

#include "String/KSStringUtils.h"

#include <algorithm>

namespace Kosma::Core {

int compareBetween(const Char* s1, size_t len1, const Char* s2, size_t len2)
{
    auto minLen = std::min(len1, len2);
    auto cmp = std::memcmp(s1, s2, minLen * sizeof(Char));
    if (cmp != 0)
        return cmp;
    return std::clamp(int(len1 - len2), -1, 1);
}

}