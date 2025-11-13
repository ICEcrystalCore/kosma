//
// Created by Jun on 2025/10/26.
//

#ifndef KOSMA_KSAPPARGUMENTS_H
#define KOSMA_KSAPPARGUMENTS_H

#include "Container/KSSet.h"
#include "Container/KSVector.h"
#include "String/KSString.h"
#include "Utility/KSOptional.h"

namespace Kosma::Core {

class AppArguments {
public:
    virtual ~AppArguments() = default;

    virtual bool contains(const String& name) = 0;

    virtual Vector<String> getRawArgs() const = 0;

    virtual Vector<String> getRawOptions() const = 0;

    virtual Set<String> getOptionNames() const = 0;

    virtual Optional<String> getValueOf(const String& name) const = 0;

    virtual Vector<String> getValuesOf(const String& name) const = 0;

    virtual size_t countOption(const String& name) const = 0;
};

}

#endif  // KOSMA_KSAPPARGUMENTS_H
