//
// Created by Jun on 2025/10/26.
//

#include "KSRealAppArguments.h"

namespace Kosma::Core {

void RealAppArguments::append(const String& arg)
{
    auto debug = true;
}

bool RealAppArguments::contains(const String& name)
{
    return false;
}

Vector<String> RealAppArguments::getRawArgs() const
{
    return {};
}

Vector<String> RealAppArguments::getRawOptions() const
{
    return {};
}

Set<String> RealAppArguments::getOptionNames() const
{
    return {};
}

Optional<String> RealAppArguments::getValueOf(const String& name) const
{
    return std::nullopt;
}

Vector<String> RealAppArguments::getValuesOf(const String& name) const
{
    return {};
}

size_t RealAppArguments::countOption(const String& name) const
{
    return 0;
}

}