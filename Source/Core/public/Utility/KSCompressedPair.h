//
// Created by Jun on 2025/8/31.
//

#ifndef KOSMA_KSCOMPRESSEDPAIR_H
#define KOSMA_KSCOMPRESSEDPAIR_H

namespace Kosma::Core {

template<typename T, typename U>
    requires std::is_empty_v<T>
class CompressedPair : public T {
public:
    template<typename U1, typename U2>
    CompressedPair(U1&& first, U2&& second)
        : T(std::forward<U1>(first))
        , m_second(std::forward<U2>(second))
    {
    }

    const T& first() const { return static_cast<const T&>(*this); }

    const U& second() const { return m_second; }

    T& first() { return static_cast<T&>(*this); }

    U& second() { return m_second; }

private:
    U m_second;
};

}

#endif  // KOSMA_KSCOMPRESSEDPAIR_H