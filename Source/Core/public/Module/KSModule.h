//
// Created by Jun on 2025/8/30.
//

#ifndef KOSMA_KSMODULE_H
#define KOSMA_KSMODULE_H

#include "Container/KSVector.h"
#include "KsCoreAPI.h"
#include "Memory/KSSharedPtr.h"
#include "Memory/KSUniquePtr.h"
#include "String/KSString.h"

namespace Kosma::Core {

class ModuleManager;

class KS_CORE_API Module {
public:
    using Name = String;

    class KS_CORE_API Factory {
    public:
        using Ptr = UniquePtr<Factory>;

        virtual ~Factory() = default;

        virtual Name name() const = 0;

        virtual Vector<Name> requiredModules() const { return {}; }

        virtual SharedPtr<Module> create() const = 0;
    };

    virtual ~Module() = default;

    virtual void startup() = 0;

    virtual void shutdown() = 0;

    const Name& name() const;

    ModuleManager* manager() const;

protected:
    Module() = default;

private:
    friend class ModuleManager;

    // Injected by ModuleManager after creation
    void setName(const Name& name);
    void setManager(ModuleManager* manager);

    Name m_name;
    ModuleManager* m_manager = nullptr;
};

// CRTP helper base: provides a static make() factory method
template<typename Derived>
class ModuleOf : public Module {
public:
    static SharedPtr<Derived> make() { return makeShared<Derived>(); }
};

}  // namespace Kosma::Core

#endif  // KOSMA_KSMODULE_H
