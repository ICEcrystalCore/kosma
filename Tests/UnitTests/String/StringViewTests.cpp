//
// Created by Jun on 2025/10/7.
//

#include "String/KSString.h"
#include "gtest/gtest.h"

using namespace Kosma::Core;

// TEST(KSStringViewTest, Basic)
// {
//     String s("abcdef", 6);
//     StringView sv(s);
//     EXPECT_EQ(sv.size(), 6);
//     EXPECT_EQ(sv[0], 'a');
//     EXPECT_EQ(sv.at(1), 'b');
//     EXPECT_THROW(sv.at(100), std::out_of_range);
//     StringView sv2 = sv.substr(2, 3);
//     EXPECT_EQ(sv2, StringView("cde", 3));
//     sv2.remove_prefix(1);
//     EXPECT_EQ(sv2, StringView("de", 2));
//     sv2.remove_suffix(1);
//     EXPECT_EQ(sv2, StringView("d", 1));
// }
//
// TEST(KSStringViewTest, FindAndCompare)
// {
//     StringView sv("abcdefabc", 9);
//     EXPECT_EQ(sv.find(StringView("abc", 3)), 0);
//     EXPECT_EQ(sv.find(StringView("def", 3)), 3);
//     EXPECT_EQ(sv.rfind(StringView("abc", 3)), 6);
//     EXPECT_TRUE(sv.starts_with(StringView("abc", 3)));
//     EXPECT_TRUE(sv.ends_with(StringView("abc", 3)));
//     EXPECT_TRUE(sv.contains(StringView("def", 3)));
//     EXPECT_FALSE(sv.contains(StringView("xyz", 3)));
//     EXPECT_EQ(sv.compare(StringView("abcdefabc", 9)), 0);
//     EXPECT_LT(sv.compare(StringView("zzz", 3)), 0);
//     EXPECT_GT(sv.compare(StringView("aaa", 3)), 0);
// }
