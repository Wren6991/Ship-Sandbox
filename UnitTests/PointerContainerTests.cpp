#include <GameLib/PointerContainer.h>

#include <vector>

#include "gtest/gtest.h"

struct TestElement
{
	static size_t DestructorInvocationCount;

	size_t const Id;
	bool IsDeletedFlag;

	explicit TestElement(size_t id)
		: Id(id)
		, IsDeletedFlag(false)
	{}

	~TestElement()
	{
		++DestructorInvocationCount;
	}

	bool IsDeleted() const { return IsDeletedFlag; }
};

size_t TestElement::DestructorInvocationCount;

size_t GetSum(PointerContainer<TestElement> const & pc)
{
	size_t sum = 0;
	for (auto const & elem : pc)
	{
		sum += elem->Id;
	}

	return sum;
}

/////////////////////////////////////////////////////////////////

TEST(PointerContainerTests, ConstructsFromVector)
{
	std::vector<TestElement *> input;
	input.push_back(new TestElement(1));
	input.push_back(new TestElement(2));
	input.push_back(new TestElement(3));
	input.push_back(new TestElement(4));

	PointerContainer<TestElement> pc(std::move(input));

    EXPECT_EQ(4U, pc.size());
}

TEST(PointerContainerTests, IteratesElements)
{
	std::vector<TestElement *> input;
	input.push_back(new TestElement(1));
	input.push_back(new TestElement(2));
	input.push_back(new TestElement(3));

	PointerContainer<TestElement> pc(std::move(input));

	auto it = pc.begin();

	EXPECT_NE(it, pc.end());
	EXPECT_EQ(1, it->Id);
	EXPECT_EQ(1, (*it)->Id);

	++it;

	EXPECT_NE(it, pc.end());
	EXPECT_EQ(2, it->Id);
	EXPECT_EQ(2, (*it)->Id);

	++it;

	EXPECT_NE(it, pc.end());
	EXPECT_EQ(3, it->Id);
	EXPECT_EQ(3, (*it)->Id);

	++it;

	EXPECT_EQ(it, pc.end());
}

TEST(PointerContainerTests, IteratesElements_Const)
{
	std::vector<TestElement *> input;
	input.push_back(new TestElement(1));
	input.push_back(new TestElement(2));
	input.push_back(new TestElement(3));

	PointerContainer<TestElement> const pc(std::move(input));

	auto it = pc.begin();

	EXPECT_NE(it, pc.end());
	EXPECT_EQ(1, it->Id);
	EXPECT_EQ(1, (*it)->Id);

	++it;

	EXPECT_NE(it, pc.end());
	EXPECT_EQ(2, it->Id);
	EXPECT_EQ(2, (*it)->Id);

	++it;

	EXPECT_NE(it, pc.end());
	EXPECT_EQ(3, it->Id);
	EXPECT_EQ(3, (*it)->Id);

	++it;

	EXPECT_EQ(it, pc.end());
}

TEST(PointerContainerTests, IteratesElements_ForLoop)
{
	std::vector<TestElement *> input;
	input.push_back(new TestElement(1));
	input.push_back(new TestElement(2));
	input.push_back(new TestElement(3));

	PointerContainer<TestElement> pc(std::move(input));

	size_t sum = 0;
	for (auto it : pc)
	{
		sum += it->Id;
	}

	EXPECT_EQ(6u, sum);
}

TEST(PointerContainerTests, IteratesElements_ForLoop_Const)
{
	std::vector<TestElement *> input;
	input.push_back(new TestElement(1));
	input.push_back(new TestElement(2));
	input.push_back(new TestElement(3));

	PointerContainer<TestElement> const pc(std::move(input));

	size_t sum = 0;
	for (auto it : pc)
	{
		sum += it->Id;
	}

	EXPECT_EQ(6u, sum);
}

TEST(PointerContainerTests, IteratesElements_Index)
{
	std::vector<TestElement *> input;
	input.push_back(new TestElement(1));
	input.push_back(new TestElement(2));
	input.push_back(new TestElement(3));

	PointerContainer<TestElement> pc(std::move(input));

	size_t sum = 0;
	for (size_t i = 0; i < pc.size(); ++i)
	{
		sum += pc[i]->Id;
	}

	EXPECT_EQ(6u, sum);
}

TEST(PointerContainerTests, IteratesElements_Index_Const)
{
	std::vector<TestElement *> input;
	input.push_back(new TestElement(1));
	input.push_back(new TestElement(2));
	input.push_back(new TestElement(3));

	PointerContainer<TestElement> const pc(std::move(input));

	size_t sum = 0;
	for (size_t i = 0; i < pc.size(); ++i)
	{
		sum += pc[i]->Id;
	}

	EXPECT_EQ(6u, sum);
}

TEST(PointerContainerTests, Initializes)
{
	std::vector<TestElement *> input;
	input.push_back(new TestElement(1));
	input.push_back(new TestElement(2));
	input.push_back(new TestElement(3));

	PointerContainer<TestElement> pc;

	EXPECT_TRUE(pc.empty());

	pc.initialize(std::move(input));

	EXPECT_FALSE(pc.empty());

	EXPECT_EQ(3u, pc.size());

	auto it = pc.begin();

	EXPECT_NE(it, pc.end());
	EXPECT_EQ(1, it->Id);
	EXPECT_EQ(1, (*it)->Id);

	++it;

	EXPECT_NE(it, pc.end());
	EXPECT_EQ(2, it->Id);
	EXPECT_EQ(2, (*it)->Id);

	++it;

	EXPECT_NE(it, pc.end());
	EXPECT_EQ(3, it->Id);
	EXPECT_EQ(3, (*it)->Id);

	++it;

	EXPECT_EQ(it, pc.end());
}

TEST(PointerContainerTests, FreesPointersOnDestructor)
{
	TestElement::DestructorInvocationCount = 0;

	std::vector<TestElement *> input;
	input.push_back(new TestElement(1));
	input.push_back(new TestElement(2));
	input.push_back(new TestElement(3));

	{
		PointerContainer<TestElement> pc(std::move(input));

		EXPECT_EQ(0u, TestElement::DestructorInvocationCount);
	}

	EXPECT_EQ(3u, TestElement::DestructorInvocationCount);
}

///////////////////////////////////////////
// ShrinkToFit
///////////////////////////////////////////

TEST(PointerContainerTests, ShrinkToFit_Empty)
{
	std::vector<TestElement *> input;
	PointerContainer<TestElement> pc(std::move(input));

	EXPECT_EQ(0U, pc.size());

	TestElement::DestructorInvocationCount = 0u;

	pc.shrink_to_fit();

	EXPECT_EQ(0U, pc.size());

	EXPECT_EQ(0u, TestElement::DestructorInvocationCount);
}

TEST(PointerContainerTests, ShrinkToFit_NoDeletions)
{
	std::vector<TestElement *> input;
	input.push_back(new TestElement(1));
	input.push_back(new TestElement(2));
	input.push_back(new TestElement(3));
	input.push_back(new TestElement(4));

	PointerContainer<TestElement> pc(std::move(input));

	EXPECT_EQ(4U, pc.size());

	TestElement::DestructorInvocationCount = 0u;

	pc.shrink_to_fit();

	EXPECT_EQ(4U, pc.size());

	EXPECT_EQ(0u, TestElement::DestructorInvocationCount);
}

TEST(PointerContainerTests, ShrinkToFit_BecomesEmpty)
{
	std::vector<TestElement *> input;
	input.push_back(new TestElement(1));
	PointerContainer<TestElement> pc(std::move(input));

	EXPECT_EQ(1U, pc.size());

	TestElement::DestructorInvocationCount = 0u;

	pc[0]->IsDeletedFlag = true;
	pc.register_deletion();

	pc.shrink_to_fit();

	EXPECT_EQ(0U, pc.size());

	EXPECT_EQ(1u, TestElement::DestructorInvocationCount);
}

TEST(PointerContainerTests, ShrinkToFit_DoesNotShrinkWhenNoDeletionRegistered)
{
	std::vector<TestElement *> input;
	input.push_back(new TestElement(1));
	PointerContainer<TestElement> pc(std::move(input));

	EXPECT_EQ(1U, pc.size());

	TestElement::DestructorInvocationCount = 0u;

	pc[0]->IsDeletedFlag = true;

	pc.shrink_to_fit();

	EXPECT_EQ(1U, pc.size());

	EXPECT_EQ(0u, TestElement::DestructorInvocationCount);
}

TEST(PointerContainerTests, ShrinkToFit_1Deletion)
{
	for (size_t i = 0; i < 4; ++i)
	{
		std::vector<TestElement *> input;
		input.push_back(new TestElement(1));
		input.push_back(new TestElement(2));
		input.push_back(new TestElement(3));
		input.push_back(new TestElement(4));

		PointerContainer<TestElement> pc(std::move(input));

		EXPECT_EQ(4U, pc.size());

		TestElement::DestructorInvocationCount = 0u;

		pc[i]->IsDeletedFlag = true;
		pc.register_deletion();

		pc.shrink_to_fit();

		EXPECT_EQ(3U, pc.size());

		EXPECT_EQ(1u, TestElement::DestructorInvocationCount);

		EXPECT_EQ(10 - (i + 1), GetSum(pc));
	}
}

TEST(PointerContainerTests, ShrinkToFit_2Deletions_Contiguous)
{
	for (size_t i = 0; i < 3; ++i)
	{
		std::vector<TestElement *> input;
		input.push_back(new TestElement(1));
		input.push_back(new TestElement(2));
		input.push_back(new TestElement(3));
		input.push_back(new TestElement(4));

		PointerContainer<TestElement> pc(std::move(input));

		EXPECT_EQ(4U, pc.size());

		TestElement::DestructorInvocationCount = 0u;

		pc[i]->IsDeletedFlag = true;
		pc.register_deletion();
		pc[i + 1]->IsDeletedFlag = true;
		pc.register_deletion();

		pc.shrink_to_fit();

		EXPECT_EQ(2U, pc.size());

		EXPECT_EQ(2U, TestElement::DestructorInvocationCount);

		EXPECT_EQ(10 - ((i + 1) + (i + 1 + 1)), GetSum(pc));
	}
}

TEST(PointerContainerTests, ShrinkToFit_2Deletions_SpacedByOne)
{
	for (size_t i = 0; i < 2; ++i)
	{
		std::vector<TestElement *> input;
		input.push_back(new TestElement(1));
		input.push_back(new TestElement(2));
		input.push_back(new TestElement(3));
		input.push_back(new TestElement(4));

		PointerContainer<TestElement> pc(std::move(input));

		EXPECT_EQ(4U, pc.size());

		TestElement::DestructorInvocationCount = 0u;

		pc[i]->IsDeletedFlag = true;
		pc.register_deletion();
		pc[i + 2]->IsDeletedFlag = true;
		pc.register_deletion();

		pc.shrink_to_fit();

		EXPECT_EQ(2U, pc.size());

		EXPECT_EQ(2U, TestElement::DestructorInvocationCount);

		EXPECT_EQ(10 - ((i + 1) + (i + 2 + 1)), GetSum(pc));
	}
}

TEST(PointerContainerTests, ShrinkToFit_2Deletions_SpacedByTwo)
{
	std::vector<TestElement *> input;
	input.push_back(new TestElement(1));
	input.push_back(new TestElement(2));
	input.push_back(new TestElement(3));
	input.push_back(new TestElement(4));

	PointerContainer<TestElement> pc(std::move(input));

	EXPECT_EQ(4U, pc.size());

	TestElement::DestructorInvocationCount = 0u;

	pc[0]->IsDeletedFlag = true;
	pc.register_deletion();
	pc[3]->IsDeletedFlag = true;
	pc.register_deletion();

	pc.shrink_to_fit();

	EXPECT_EQ(2U, pc.size());

	EXPECT_EQ(2U, TestElement::DestructorInvocationCount);

	EXPECT_EQ(5u, GetSum(pc));
}

TEST(PointerContainerTests, ShrinkToFit_3Deletions_Contiguous)
{
	for (size_t i = 0; i < 2; ++i)
	{
		std::vector<TestElement *> input;
		input.push_back(new TestElement(1));
		input.push_back(new TestElement(2));
		input.push_back(new TestElement(3));
		input.push_back(new TestElement(4));

		PointerContainer<TestElement> pc(std::move(input));

		EXPECT_EQ(4U, pc.size());

		TestElement::DestructorInvocationCount = 0u;

		pc[i]->IsDeletedFlag = true;
		pc.register_deletion();
		pc[i + 1]->IsDeletedFlag = true;
		pc.register_deletion();
		pc[i + 2]->IsDeletedFlag = true;
		pc.register_deletion();

		pc.shrink_to_fit();

		EXPECT_EQ(1U, pc.size());

		EXPECT_EQ(3U, TestElement::DestructorInvocationCount);

		EXPECT_EQ(10 - ((i + 1) + (i + 1 + 1) + (i + 2 + 1)), GetSum(pc));
	}
}

TEST(PointerContainerTests, ShrinkToFit_AllDeleted)
{
	std::vector<TestElement *> input;
	input.push_back(new TestElement(1));
	input.push_back(new TestElement(2));
	input.push_back(new TestElement(3));
	input.push_back(new TestElement(4));

	PointerContainer<TestElement> pc(std::move(input));

	EXPECT_EQ(4U, pc.size());

	TestElement::DestructorInvocationCount = 0u;

	pc[0]->IsDeletedFlag = true;
	pc.register_deletion();
	pc[1]->IsDeletedFlag = true;
	pc.register_deletion();
	pc[2]->IsDeletedFlag = true;
	pc.register_deletion();
	pc[3]->IsDeletedFlag = true;
	pc.register_deletion();

	pc.shrink_to_fit();

	EXPECT_EQ(0U, pc.size());

	EXPECT_EQ(4U, TestElement::DestructorInvocationCount);
}
