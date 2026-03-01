//
// Created by Jun on 2025/8/30.
//

#ifndef KOSMA_KSMODULESTORE_H
#define KOSMA_KSMODULESTORE_H

#include "Memory/KSSharedPtr.h"
#include "Module/KSModule.h"

#include <map>
#include <set>
#include <vector>

namespace Kosma::Core {

// Stores loaded module instances and tracks inter-module dependencies.
class ModuleStore {
public:
    using Name = Module::Name;

    bool has(const Name& name) const;
    SharedPtr<Module> get(const Name& name) const;

    void add(const Name& name, SharedPtr<Module> mod);
    void remove(const Name& name);

    std::vector<Name> allLoaded() const;

    // Track that 'dependent' depends on 'dep' (i.e., dep must outlive dependent).
    void trackDependent(const Name& dep, const Name& dependent);
    void untrackDependent(const Name& dep, const Name& dependent);

    // Returns the set of currently loaded modules that declared a dependency on 'dep'.
    const std::set<Name>& dependentsOf(const Name& dep) const;

private:
    std::map<Name, SharedPtr<Module>> m_modules;
    std::map<Name, std::set<Name>> m_dependents;

    static const std::set<Name> s_emptySet;
};

}  // namespace Kosma::Core

#endif  // KOSMA_KSMODULESTORE_H
