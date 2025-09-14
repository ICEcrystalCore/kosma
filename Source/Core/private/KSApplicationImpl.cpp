//
// Created by Jun on 2025/8/30.
//

#include "KSApplicationImpl.h"

namespace Kosma::Core {

UniquePtr<Application::Impl> Application::Impl::make()
{
    return makeUnique<Impl>();
}

UniquePtr<Application::Impl> Application::Impl::make(int argc, char** argv)
{
    return makeUnique<Impl>(argc, argv);
}

UniquePtr<Application::Impl> Application::Impl::make(int argc, wchar_t** argv)
{
    return makeUnique<Impl>(argc, argv);
}

Application::Impl::Impl()
{
}

Application::Impl::Impl(int argc, char** argv)
{
}

Application::Impl::Impl(int argc, wchar_t** argv)
{
}

}