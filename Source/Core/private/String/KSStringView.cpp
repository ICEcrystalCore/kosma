#include "String/KSStringView.h"

#include <algorithm>
#include <cstring>
#include <stdexcept>

#include "String/KSString.h"

namespace Kosma::Core {

StringView::StringView(const String& str)
    : data_(str.data())
    , size_(str.length())
{
}

int StringView::compare(const StringView& other) const noexcept
{
    size_t minLen = std::min(size_, other.size_);
    int cmp = std::char_traits<Char>::compare(data_, other.data_, minLen);
    if (cmp != 0)
        return cmp;
    if (size_ < other.size_)
        return -1;
    if (size_ > other.size_)
        return 1;
    return 0;
}

bool StringView::starts_with(const StringView& sv) const noexcept
{
    return size_ >= sv.size_ && std::char_traits<Char>::compare(data_, sv.data_, sv.size_) == 0;
}

bool StringView::ends_with(const StringView& sv) const noexcept
{
    return size_ >= sv.size_
           && std::char_traits<Char>::compare(data_ + size_ - sv.size_, sv.data_, sv.size_) == 0;
}

bool StringView::contains(const StringView& sv) const noexcept
{
    return find(sv) != npos;
}

size_t StringView::find(const StringView& sv, size_t pos) const noexcept
{
    if (sv.size_ == 0 || sv.size_ > size_ || pos > size_ - sv.size_)
        return npos;
    for (size_t i = pos; i <= size_ - sv.size_; ++i) {
        if (std::char_traits<Char>::compare(data_ + i, sv.data_, sv.size_) == 0)
            return i;
    }
    return npos;
}

size_t StringView::rfind(const StringView& sv, size_t pos) const noexcept
{
    if (sv.size_ == 0 || sv.size_ > size_)
        return npos;
    size_t end = std::min(pos, size_ - sv.size_);
    for (size_t i = end + 1; i-- > 0;) {
        if (std::char_traits<Char>::compare(data_ + i, sv.data_, sv.size_) == 0)
            return i;
    }
    return npos;
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
    return {data_, size_};
}

}  // namespace Kosma::Core
