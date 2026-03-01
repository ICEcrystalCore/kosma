//
// Created by Jun on 2025/8/30.
//

#include "Module/KSModuleManager.h"
#include "KSModuleManagerImpl.h"

namespace Kosma::Core {

// Called by Impl (nested class) to inject manager context into a freshly created module.
// ModuleManager is a friend of Module, so this member function can call private setters.
void ModuleManager::injectModule(Module& mod, const Module::Name& name, ModuleManager* mgr)
{
    mod.setName(name);
    mod.setManager(mgr);
}

ModuleManager::ModuleManager() : m_impl(makeUnique<Impl>()) {}

ModuleManager::~ModuleManager()
{
    m_impl->unloadAll(this);
}

bool ModuleManager::registerFactory(Module::Factory::Ptr factory)
{
    return m_impl->registerFactory(std::move(factory));
}

void ModuleManager::unregisterFactory(const Module::Name& name)
{
    m_impl->unregisterFactory(name);
}

bool ModuleManager::load(const Module::Name& name)
{
    return m_impl->load(name, this);
}

void ModuleManager::unload(const Module::Name& name)
{
    m_impl->unload(name, this);
}

void ModuleManager::loadAll()
{
    m_impl->loadAll(this);
}

void ModuleManager::unloadAll()
{
    m_impl->unloadAll(this);
}

bool ModuleManager::isLoaded(const Module::Name& name) const
{
    return m_impl->isLoaded(name);
}

bool ModuleManager::hasFactory(const Module::Name& name) const
{
    return m_impl->hasFactory(name);
}

SharedPtr<Module> ModuleManager::find(const Module::Name& name) const
{
    return m_impl->find(name);
}

}  // namespace Kosma::Core
