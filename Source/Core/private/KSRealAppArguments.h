//
// Created by Jun on 2025/10/26.
//

#ifndef KOSMA_KSREALAPPARGUMENTS_H
#define KOSMA_KSREALAPPARGUMENTS_H

#include "KSAppArguments.h"

namespace Kosma::Core {

class RealAppArguments : public AppArguments {
public:
    void append(const String& arg);

    bool contains(const String& name) override;

    Vector<String> getRawArgs() const override;

    Vector<String> getRawOptions() const override;

    Set<String> getOptionNames() const override;

    Optional<String> getValueOf(const String& name) const override;

    Vector<String> getValuesOf(const String& name) const override;

    size_t countOption(const String& name) const override;

private:
};

}

#endif  // KOSMA_KSREALAPPARGUMENTS_H
