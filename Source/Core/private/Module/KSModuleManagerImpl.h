//
// Created by Jun on 2025/8/30.
//

#ifndef KOSMA_KSMODULEMANAGERIMPL_H
#define KOSMA_KSMODULEMANAGERIMPL_H

#include "KSLoadHelper.h"
#include "KSLoopChecker.h"
#include "KSModuleGraph.h"
#include "KSModuleStore.h"
#include "Module/KSModuleManager.h"

#include <map>

namespace Kosma::Core {

struct ModuleManager::Impl {
    using FactoryMap = std::map<Module::Name, Module::Factory::Ptr>;

    ModuleGraph graph;
    ModuleStore store;
    FactoryMap factories;

    bool registerFactory(Module::Factory::Ptr factory);
    void unregisterFactory(const Module::Name& name);

    bool load(const Module::Name& name, ModuleManager* mgr);
    void unload(const Module::Name& name, ModuleManager* mgr);
    void loadAll(ModuleManager* mgr);
    void unloadAll(ModuleManager* mgr);

    bool isLoaded(const Module::Name& name) const;
    bool hasFactory(const Module::Name& name) const;
    SharedPtr<Module> find(const Module::Name& name) const;
};

}  // namespace Kosma::Core

#endif  // KOSMA_KSMODULEMANAGERIMPL_H
