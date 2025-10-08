//
// Created by Jun on 2025/9/14.
//

#include "String/KSString.h"
#include "gtest/gtest.h"

using namespace Kosma::Core;

TEST(StringTest, NewStringIsEmpty)
{
    String str;
    EXPECT_TRUE(str.isEmpty());
}

TEST(StringTest, NewStringSizeIsZero)
{
    String str;
    EXPECT_EQ(0, str.length());
}

TEST(StringTest, NewStringCharCountIsZero)
{
    String str;
    EXPECT_EQ(0, str.charCount());
}

TEST(StringTest, StringFromCChar)
{
    String str("Hello, World!");
    EXPECT_FALSE(str.isEmpty());
    EXPECT_EQ(13, str.length());
    EXPECT_EQ(13, str.charCount());
}

TEST(StringTest, StringFromU8Char)
{
    String str(u8"Hello, World!");
    EXPECT_FALSE(str.isEmpty());
    EXPECT_EQ(13, str.length());
    EXPECT_EQ(13, str.charCount());
}

TEST(StringTest, StringFromU16Char)
{
    String str(u"Hello, World!");
    EXPECT_FALSE(str.isEmpty());
    EXPECT_EQ(13, str.length());
    EXPECT_EQ(13, str.charCount());
}

TEST(StringTest, StringFromU32Char)
{
    String str(U"Hello, World!");
    EXPECT_FALSE(str.isEmpty());
    EXPECT_EQ(13, str.length());
    EXPECT_EQ(13, str.charCount());
}

TEST(StringTest, SurrogateString)
{
    String str(U"hello,𪚥");
    EXPECT_FALSE(str.isEmpty());
    EXPECT_EQ(8, str.length());
    EXPECT_EQ(7, str.charCount());
}

TEST(StringTest, ElementAccess)
{
    String s("hello", 5);
    EXPECT_EQ(s[0], 'h');
    EXPECT_EQ(s.at(1), 'e');
    s[0] = 'H';
    EXPECT_EQ(s[0], 'H');
    EXPECT_THROW(s.at(100), std::out_of_range);
}

TEST(StringTest, Modifiers)
{
    String s("abc", 3);
    s += "def";
    EXPECT_EQ(s, String("abcdef", 6));
    s.pushBack('g');
    EXPECT_EQ(s, String("abcdefg", 7));
    s.popBack();
    EXPECT_EQ(s, String("abcdef", 6));
    s.insert(3, (Char*)u"123", 3);
    EXPECT_EQ(s, String("abc123def", 9));
    s.erase(3, 3);
    EXPECT_EQ(s, String("abcdef", 6));
    s.replace(3, 3, String("XYZ", 3));
    EXPECT_EQ(s, String("abcXYZ", 6));
}

TEST(StringTest, FindAndCompare)
{
    String s("abcdefabc", 9);
    EXPECT_EQ(s.find("abc"), 0);
    EXPECT_EQ(s.find("def"), 3);
    EXPECT_EQ(s.rfind("abc"), 6);
    EXPECT_TRUE(s.startsWith("abc"));
    EXPECT_TRUE(s.endsWith("abc"));
    EXPECT_TRUE(s.contains("def"));
    EXPECT_FALSE(s.contains("xyz"));
    EXPECT_EQ(s.compare("abcdefabc"), 0);
    EXPECT_LT(s.compare("zzz"), 0);
    EXPECT_GT(s.compare("aaa"), 0);
}
