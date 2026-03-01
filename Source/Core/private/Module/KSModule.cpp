//
// Created by Jun on 2025/8/30.
//

#include "Module/KSModule.h"

namespace Kosma::Core {

const Module::Name& Module::name() const
{
    return m_name;
}

ModuleManager* Module::manager() const
{
    return m_manager;
}

void Module::setName(const Name& name)
{
    m_name = name;
}

void Module::setManager(ModuleManager* manager)
{
    m_manager = manager;
}

}  // namespace Kosma::Core
