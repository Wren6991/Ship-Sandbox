#include <GameLib/ElementRepository.h>

#include <string>
#include <vector>

#include "gtest/gtest.h"

struct TestStruct
{
    float a;
    std::string b;

    TestStruct(
        float _a,
        std::string _b)
        : a(_a)
        , b(_b)
    {}
};

TEST(ElementRepositoryTests, Emplaces)
{
    ElementRepository<TestStruct> repo(2);

    repo.emplace_back(1.0f, "1.0f");

    EXPECT_EQ(1u, repo.size());

    repo.emplace_back(2.0f, "2.0f");

    EXPECT_EQ(2u, repo.size());
}


TEST(ElementRepositoryTests, IteratesElements)
{
    ElementRepository<TestStruct> repo(3);

    repo.emplace_back(1.0f, "1.0f");
    repo.emplace_back(2.0f, "2.0f");
    repo.emplace_back(3.0f, "3.0f");

    auto it = repo.begin();

    EXPECT_NE(it, repo.end());
    EXPECT_EQ(1.0f, (*it).a);
    EXPECT_EQ(1.0f, it->a);

    ++it;

    EXPECT_NE(it, repo.end());
    EXPECT_EQ(2.0f, (*it).a);
    EXPECT_EQ(2.0f, it->a);

    ++it;

    EXPECT_NE(it, repo.end());
    EXPECT_EQ(3.0f, (*it).a);
    EXPECT_EQ(3.0f, it->a);

    ++it;

    EXPECT_EQ(it, repo.end());
}

TEST(ElementRepositoryTests, IteratesElements_Const)
{
    ElementRepository<TestStruct> repo(3);

    repo.emplace_back(1.0f, "1.0f");
    repo.emplace_back(2.0f, "2.0f");
    repo.emplace_back(3.0f, "3.0f");

    auto const & repoRef = repo;

    auto it = repoRef.begin();

    EXPECT_NE(it, repoRef.end());
    EXPECT_EQ(1.0f, (*it).a);
    EXPECT_EQ(1.0f, it->a);

    ++it;

    EXPECT_NE(it, repoRef.end());
    EXPECT_EQ(2.0f, (*it).a);
    EXPECT_EQ(2.0f, it->a);

    ++it;

    EXPECT_NE(it, repoRef.end());
    EXPECT_EQ(3.0f, (*it).a);
    EXPECT_EQ(3.0f, it->a);

    ++it;

    EXPECT_EQ(it, repoRef.end());
}

TEST(ElementRepositoryTests, IteratesElements_ForLoop)
{
    ElementRepository<TestStruct> repo(3);

    repo.emplace_back(1.0f, "1.0f");
    repo.emplace_back(2.0f, "2.0f");
    repo.emplace_back(3.0f, "3.0f");

    float sum = 0.0f;
    for (auto i : repo)
    {
        sum += i.a;
    }

    EXPECT_EQ(6.0f, sum);
}

TEST(ElementRepositoryTests, IteratesElements_ForLoop_Const)
{
    ElementRepository<TestStruct> repo(3);

    repo.emplace_back(1.0f, "1.0f");
    repo.emplace_back(2.0f, "2.0f");
    repo.emplace_back(3.0f, "3.0f");

    auto const & repoRef = repo;

    float sum = 0.0f;
    for (auto i : repoRef)
    {
        sum += i.a;
    }

    EXPECT_EQ(6.0f, sum);
}

TEST(ElementRepositoryTests, IteratesElements_Index)
{
    ElementRepository<TestStruct> repo(3);
    
    repo.emplace_back(1.0f, "1.0f");
    repo.emplace_back(2.0f, "2.0f");
    repo.emplace_back(3.0f, "3.0f");

    float sum = 0.0f;
    for (size_t i = 0; i < repo.size(); ++i)
    {
        sum += repo[i].a;
    }

    EXPECT_EQ(6.0f, sum);
}

TEST(ElementRepositoryTests, IteratesElements_Index_Const)
{
    ElementRepository<TestStruct> repo(3);

    repo.emplace_back(1.0f, "1.0f");
    repo.emplace_back(2.0f, "2.0f");
    repo.emplace_back(3.0f, "3.0f");

    auto const & repoRef = repo;

    float sum = 0.0f;
    for (size_t i = 0; i < repoRef.size(); ++i)
    {
        sum += repo[i].a;
    }

    EXPECT_EQ(6.0f, sum);
}

TEST(ElementRepositoryTests, ThrowsExceptionOnMaxSize)
{
    ElementRepository<TestStruct> repo(3);

    repo.emplace_back(1.0f, "1.0f");
    repo.emplace_back(2.0f, "2.0f");
    repo.emplace_back(3.0f, "3.0f");

    EXPECT_THROW(
        repo.emplace_back(4.0f, "4.0f"),
        std::runtime_error);    
}

