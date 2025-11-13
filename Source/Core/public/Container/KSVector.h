//
// Created by Jun on 2025/10/26.
//

#ifndef KOSMA_KSVECTOR_H
#define KOSMA_KSVECTOR_H

#include <vector>

namespace Kosma::Core {

template<typename T>
class Vector {
public:
    Vector();

    Vector(const Vector& other);

    Vector(Vector&& other);

    void swap(Vector& other);

private:
    T* m_data;
    size_t m_size;
    size_t m_capacity;
};

#include "Container/KSVector.inl"

}

#endif  // KOSMA_KSVECTOR_H