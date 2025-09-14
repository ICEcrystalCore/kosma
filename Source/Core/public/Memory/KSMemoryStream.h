//
// Created by Jun on 2025/8/31.
//

#ifndef KOSMA_KSMEMORYSTREAM_H
#define KOSMA_KSMEMORYSTREAM_H

#include <cstddef>

#include "KsCoreAPI.h"

namespace Kosma::Core {

class KS_CORE_API MemoryStream {
public:
    MemoryStream();

    ~MemoryStream();

    MemoryStream(const MemoryStream&) = delete;

    MemoryStream(MemoryStream&& other) noexcept;

protected:
private:
    char* m_buffer;
    size_t m_size;
    size_t m_capacity;
};

}

#endif  // KOSMA_KSMEMORYSTREAM_H
