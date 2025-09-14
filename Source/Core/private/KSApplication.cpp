//
// Created by Jun on 2025/8/16.
//

#include "KSApplicationImpl.h"

namespace Kosma::Core {

Application::Application()
    : m_impl(Impl::make())
{
}

Application::Application(int argc, char** argv)
    : m_impl(Impl::make(argc, argv))
{
}

Application::Application(int argc, wchar_t** argv)
    : m_impl(Impl::make(argc, argv))
{
}

Application::~Application() = default;

void Application::run()
{
    initialize();

    while (isRunning()) {
        update();
    }

    shutdown();
}

}
