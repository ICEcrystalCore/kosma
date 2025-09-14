//
// Created by Jun on 2025/9/14.
//

#include "String/KSString.h"
#include "gtest/gtest.h"

using KsString = Kosma::Core::String;

TEST(StringTest, NewStringIsEmpty)
{
    KsString str;
    EXPECT_TRUE(str.isEmpty());
}

TEST(StringTest, NewStringSizeIsZero)
{
    KsString str;
    EXPECT_EQ(0, str.length());
}

TEST(StringTest, NewStringCharCountIsZero)
{
    KsString str;
    EXPECT_EQ(0, str.charCount());
}

TEST(StringTest, StringFromCChar)
{
    KsString str("Hello, World!");
    EXPECT_FALSE(str.isEmpty());
    EXPECT_EQ(13, str.length());
    EXPECT_EQ(13, str.charCount());
}

TEST(StringTest, StringFromU8Char)
{
    KsString str(u8"Hello, World!");
    EXPECT_FALSE(str.isEmpty());
    EXPECT_EQ(13, str.length());
    EXPECT_EQ(13, str.charCount());
}