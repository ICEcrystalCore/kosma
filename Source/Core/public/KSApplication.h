//
// Created by Jun on 2025/8/30.
//

#ifndef KOSMA_KSAPPLICATION_H
#define KOSMA_KSAPPLICATION_H

#include "KSAppArguments.h"
#include "KsCoreAPI.h"
#include "Memory/KSUniquePtr.h"

namespace Kosma::Core {

class KS_CORE_API Application {
public:
    Application();

    Application(int argc, char** argv);

    Application(int argc, wchar_t** argv);

    virtual ~Application();

    virtual void initialize() = 0;

    virtual bool isRunning() const = 0;

    virtual void update() = 0;

    virtual void shutdown() = 0;

    const AppArguments& arguments() const;

    void run();

private:
    class Impl;

    UniquePtr<Impl> m_impl;
};

}

#endif  // KOSMA_KSAPPLICATION_H