//
// Created by Jun on 2025/8/30.
//

#ifndef KOSMA_KSCHAR_H
#define KOSMA_KSCHAR_H

#include "String/KSCharTypes.h"
#include "Utility/KSOptional.h"

namespace Kosma::Core {

class Char {
public:
    Char()
        : m_ch()
    {
    }

    ~Char() = default;

    Char(U16Char u16)
        : m_ch(u16)
    {
    }

    Char(const Char& other) = default;

    Char& operator=(const Char& other) = default;

    bool operator==(const Char&) const = default;

    bool operator!=(const Char&) const = default;

    auto operator<=>(const Char&) const = default;

    bool isASCII() const { return m_ch <= 0x7F; }

    bool isHighSurrogate() const { return 0xD800 <= m_ch && m_ch <= 0xDBFF; }

    bool isLowSurrogate() const { return 0xDC00 <= m_ch && m_ch <= 0xDFFF; }

    bool isSurrogate() const { return isHighSurrogate() || isLowSurrogate(); }

    U16Char get() const { return m_ch; }

    void set(U16Char ch) { m_ch = ch; }

private:
    U16Char m_ch;
};

}

#endif  // KOSMA_KSCHAR_H