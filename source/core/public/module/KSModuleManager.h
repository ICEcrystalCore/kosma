//
// Created by Jun on 2025/8/30.
//

#ifndef KOSMA_KSMODULEMANAGER_H
#define KOSMA_KSMODULEMANAGER_H

#include "KsCoreAPI.h"
#include "memory/KSSharedPtr.h"
#include "memory/KSUniquePtr.h"
#include "module/KSModule.h"

namespace Kosma::Core {

class KS_CORE_API ModuleManager {
public:
    ModuleManager();
    ~ModuleManager();

    // Register a factory. Returns false (and rejects) if the declared
    // dependencies would create a circular dependency cycle.
    bool registerFactory(Module::Factory::Ptr factory);

    void unregisterFactory(const Module::Name& name);

    // Load a module and all of its transitive dependencies.
    // Returns false if the module or any dependency has no registered factory.
    bool load(const Module::Name& name);

    void unload(const Module::Name& name);

    // Load / unload every registered factory.
    void loadAll();
    void unloadAll();

    bool isLoaded(const Module::Name& name) const;
    bool hasFactory(const Module::Name& name) const;

    SharedPtr<Module> find(const Module::Name& name) const;

    template<typename T>
    SharedPtr<T> findAs(const Module::Name& name) const
    {
        return dynamicPointerCast<T>(find(name));
    }

private:
    // Called by Impl after module creation to inject manager context.
    // Accessible from Impl because Impl is a nested class (C++11 §11.7).
    static void injectModule(Module& mod, const Module::Name& name, ModuleManager* mgr);

    struct Impl;
    UniquePtr<Impl> m_impl;
};

}  // namespace Kosma::Core

#endif  // KOSMA_KSMODULEMANAGER_H
