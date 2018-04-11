#include <GameLib/FixedSizeVector.h>

#include <vector>

#include "gtest/gtest.h"

TEST(FixedSizeVectorTests, Empty)
{
    FixedSizeVector<int, 6> vec;

    EXPECT_EQ(0u, vec.size());
    EXPECT_TRUE(vec.empty());
}

TEST(FixedSizeVectorTests, PushBack)
{
	FixedSizeVector<int, 6> vec;

    vec.push_back(4);

    EXPECT_EQ(1u, vec.size());
    EXPECT_FALSE(vec.empty());

    vec.push_back(6);

    EXPECT_EQ(2u, vec.size());
    EXPECT_FALSE(vec.empty());
}

TEST(FixedSizeVectorTests, IteratesElements)
{
    FixedSizeVector<int, 6> vec;

    vec.push_back(3);
    vec.push_back(2);
    vec.push_back(1);

    auto it = vec.begin();

    EXPECT_NE(it, vec.end());
    EXPECT_EQ(3, *it);

    ++it;

    EXPECT_NE(it, vec.end());
    EXPECT_EQ(2, *it);

    ++it;

    EXPECT_NE(it, vec.end());
    EXPECT_EQ(1, *it);

    ++it;

    EXPECT_EQ(it, vec.end());
}

TEST(FixedSizeVectorTests, IteratesElements_Const)
{
    FixedSizeVector<int, 6> vec;

    vec.push_back(3);
    vec.push_back(2);
    vec.push_back(1);

    auto const & vecRef = vec;

    auto it = vecRef.begin();

    EXPECT_NE(it, vecRef.end());
    EXPECT_EQ(3, *it);

    ++it;

    EXPECT_NE(it, vecRef.end());
    EXPECT_EQ(2, *it);

    ++it;

    EXPECT_NE(it, vecRef.end());
    EXPECT_EQ(1, *it);

    ++it;

    EXPECT_EQ(it, vecRef.end());
}

TEST(FixedSizeVectorTests, IteratesElements_ForLoop)
{
    FixedSizeVector<int, 6> vec;

    vec.push_back(3);
    vec.push_back(2);
    vec.push_back(1);

    size_t sum = 0;
    for (auto i : vec)
    {
        sum += i;
    }

    EXPECT_EQ(6u, sum);
}

TEST(FixedSizeVectorTests, IteratesElements_ForLoop_Const)
{
    FixedSizeVector<int, 6> vec;

    vec.push_back(3);
    vec.push_back(2);
    vec.push_back(1);

    auto const & vecRef = vec;

    size_t sum = 0;
    for (auto i : vecRef)
    {
        sum += i;
    }

    EXPECT_EQ(6u, sum);
}

TEST(FixedSizeVectorTests, IteratesElements_Index)
{
    FixedSizeVector<int, 6> vec;

    vec.push_back(3);
    vec.push_back(2);
    vec.push_back(1);

    size_t sum = 0;
    for (size_t i = 0; i < vec.size(); ++i)
    {
        sum += vec[i];
    }

    EXPECT_EQ(6u, sum);
}

TEST(FixedSizeVectorTests, IteratesElements_Index_Const)
{
    FixedSizeVector<int, 6> vec;

    vec.push_back(3);
    vec.push_back(2);
    vec.push_back(1);

    auto const & vecRef = vec;

    size_t sum = 0;
    for (size_t i = 0; i < vecRef.size(); ++i)
    {
        sum += vecRef[i];
    }

    EXPECT_EQ(6u, sum);
}

TEST(FixedSizeVectorTests, ThrowsExceptionOnMaxSize)
{
    FixedSizeVector<int, 6> vec;

    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    vec.push_back(4);
    vec.push_back(5);
    vec.push_back(6);

    EXPECT_THROW(
        vec.push_back(1),
        std::runtime_error);    
}

TEST(FixedSizeVectorTests, Erases_Empty)
{
    FixedSizeVector<int, 6> vec;

    bool result = vec.erase_first(3);

    EXPECT_FALSE(result);
    EXPECT_EQ(0u, vec.size());
}

TEST(FixedSizeVectorTests, Erases_BecomesEmpty)
{
    FixedSizeVector<int, 6> vec;

    vec.push_back(3);

    bool result = vec.erase_first(3);

    EXPECT_TRUE(result);
    EXPECT_EQ(0u, vec.size());
}

TEST(FixedSizeVectorTests, Erases_NotFound)
{
    FixedSizeVector<int, 6> vec;

    vec.push_back(3);

    bool result = vec.erase_first(4);

    EXPECT_FALSE(result);
    EXPECT_EQ(1u, vec.size());
}

TEST(FixedSizeVectorTests, Erases_Copies_First)
{
    FixedSizeVector<int, 6> vec;

    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);

    bool result = vec.erase_first(1);

    EXPECT_TRUE(result);

    ASSERT_EQ(2u, vec.size());
    EXPECT_EQ(2, vec[0]);
    EXPECT_EQ(3, vec[1]);
}

TEST(FixedSizeVectorTests, Erases_Copies_Middle)
{
    FixedSizeVector<int, 6> vec;

    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);

    bool result = vec.erase_first(2);

    EXPECT_TRUE(result);

    ASSERT_EQ(2u, vec.size());
    EXPECT_EQ(1, vec[0]);
    EXPECT_EQ(3, vec[1]);
}

TEST(FixedSizeVectorTests, Erases_Copies_Last)
{
    FixedSizeVector<int, 6> vec;

    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);

    bool result = vec.erase_first(3);

    EXPECT_TRUE(result);

    ASSERT_EQ(2u, vec.size());
    EXPECT_EQ(1, vec[0]);
    EXPECT_EQ(2, vec[1]);
}
