#include "String/KSStringView.h"

#include <algorithm>
#include <stdexcept>

#include "String/KSString.h"
#include "String/KSStringUtils.h"

namespace Kosma::Core {

StringView::StringView(const String& str)
    : m_data(str.data())
    , m_size(str.length())
{
}

int StringView::compare(const StringView& other) const noexcept
{
    return compareBetween(m_data, m_size, other.m_data, other.m_size);
}

bool StringView::starts_with(const StringView& sv) const noexcept
{
    if (m_size < sv.m_size)
        return false;
    return 0 == compareBetween(m_data, sv.m_size, sv.m_data, sv.m_size);
}

bool StringView::ends_with(const StringView& sv) const noexcept
{
    if (m_size < sv.m_size)
        return false;
    return 0 == compareBetween(m_data + (m_size - sv.m_size), sv.m_size, sv.m_data, sv.m_size);
}

bool StringView::contains(const StringView& sv) const noexcept
{
    return find(sv) != kNotFound;
}

size_t StringView::find(const StringView& sv, size_t pos) const noexcept
{
    if (sv.m_size == 0 || sv.m_size > m_size || pos > m_size - sv.m_size)
        return kNotFound;
    for (size_t i = pos; i <= m_size - sv.m_size; ++i) {
        if (0 == compareBetween(m_data + i, sv.m_size, sv.m_data, sv.m_size))
            return i;
    }
    return kNotFound;
}

size_t StringView::rfind(const StringView& sv, size_t pos) const noexcept
{
    if (sv.m_size == 0 || sv.m_size > m_size)
        return kNotFound;
    size_t end = std::min(pos, m_size - sv.m_size);
    for (size_t i = end + 1; i-- > 0;) {
        if (0 == compareBetween(m_data + i, sv.m_size, sv.m_data, sv.m_size))
            return i;
    }
    return kNotFound;
}

bool StringView::operator==(const StringView& rhs) const noexcept
{
    return compare(rhs) == 0;
}

bool StringView::operator!=(const StringView& rhs) const noexcept
{
    return compare(rhs) != 0;
}

bool StringView::operator<(const StringView& rhs) const noexcept
{
    return compare(rhs) < 0;
}

bool StringView::operator>(const StringView& rhs) const noexcept
{
    return compare(rhs) > 0;
}

bool StringView::operator<=(const StringView& rhs) const noexcept
{
    return compare(rhs) <= 0;
}

bool StringView::operator>=(const StringView& rhs) const noexcept
{
    return compare(rhs) >= 0;
}

String StringView::to_string() const
{
    return {m_data, m_size};
}

}  // namespace Kosma::Core
