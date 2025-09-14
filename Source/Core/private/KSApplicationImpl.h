//
// Created by Jun on 2025/8/30.
//

#ifndef KOSMA_KSAPPLICATIONIMPL_H
#define KOSMA_KSAPPLICATIONIMPL_H

#include "KSApplication.h"
#include "String/KSString.h"

namespace Kosma::Core {

class Application::Impl {
public:
    static UniquePtr<Impl> make();

    static UniquePtr<Impl> make(int argc, char** argv);

    static UniquePtr<Impl> make(int argc, wchar_t** argv);

    Impl();

    Impl(int argc, char** argv);

    Impl(int argc, wchar_t** argv);

private:
    String m_args;
};

}

#endif  // KOSMA_KSAPPLICATIONIMPL_H
