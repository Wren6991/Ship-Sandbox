#include <GameLib/GameEventDispatcher.h>
#include <GameLib/GameParameters.h>
#include <GameLib/Physics.h>

#include <initializer_list>
#include <iterator>

#include "gtest/gtest.h"

#include <algorithm>
#include <memory>

class ShipTests : public ::testing::Test
{
    virtual void SetUp() override
    {
        std::shared_ptr<GameEventDispatcher> dispatcher = std::make_shared<GameEventDispatcher>();
        mWorld = std::make_unique<Physics::World>(dispatcher);
    }

    virtual void TearDown() override
    {
    }

protected:

	static std::vector<std::unique_ptr<Material const>> MakeMaterials(std::vector<Material> && materials)	
	{
		std::vector<std::unique_ptr<Material const>> res;
		for (Material const & m : materials)
			res.emplace_back(new Material(m));

		return res;
	}

    std::unique_ptr<Physics::World> mWorld;
};


///////////////////////////////////////////////////////////////////////
// Points
///////////////////////////////////////////////////////////////////////

TEST_F(ShipTests, BuildsPoints_OnePoint)
{
	auto materials = MakeMaterials(
		{
			{ "Mat1", 1.0f, 1.0f, { 1, 1, 1 }, { 1, 1, 1 }, false, std::nullopt, std::nullopt },
			{ "Mat2", 1.0f, 1.0f, { 25, 30, 35 }, { 25, 30, 35 }, false, std::nullopt, std::nullopt }
		}
	);

	unsigned char imageData[] = {
		0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,
		0x00, 0x00, 0x00,	25, 30, 35,			0x00, 0x00, 0x00,	0x00, 0x00, 0x00,
		0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,
		0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,
		0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,
	};

	auto ship = Physics::Ship::Create(
		mWorld.get(),
		imageData,
		4,
		5,
		materials);

	ASSERT_TRUE(!!ship);

	ASSERT_EQ(1u, ship->GetPoints().size());

	Physics::Point const & pt = *(ship->GetPoints().begin());
	EXPECT_EQ(vec2f(-1.0f, 3.0f), pt.GetPosition());
	EXPECT_EQ(materials[1].get(), pt.GetMaterial());
    EXPECT_EQ(0u, pt.GetConnectedSprings().size());
	EXPECT_EQ(0u, pt.GetConnectedTriangles().size());
    EXPECT_EQ(nullptr, pt.GetConnectedElectricalElement());
	EXPECT_TRUE(pt.IsLeaking());
}

TEST_F(ShipTests, BuildsPoints_TwoPoints)
{

	auto materials = MakeMaterials(
		{
            { "Mat1", 1.0f, 1.0f, { 40, 45, 50 }, { 40, 45, 50 }, true, std::nullopt, std::nullopt },
            { "Mat2", 1.0f, 1.0f, { 25, 30, 35 }, { 25, 30, 35 }, false, std::nullopt, std::nullopt }
		}
	);

	unsigned char imageData[] = {
		0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,
		0x00, 0x00, 0x00,	25, 30, 35,			40, 45, 50,			0x00, 0x00, 0x00,
		0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,
		0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,
		0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,
	};

	auto ship = Physics::Ship::Create(
        mWorld.get(),
		imageData,
		4,
		5,
		materials);

	ASSERT_TRUE(!!ship);

	ASSERT_EQ(2u, ship->GetPoints().size());

    auto pointIt = ship->GetPoints().begin();

	Physics::Point const & pt1 = *pointIt;
	EXPECT_EQ(vec2f(-1.0f, 3.0f), pt1.GetPosition());
	EXPECT_EQ(materials[1].get(), pt1.GetMaterial());
    EXPECT_EQ(1u, pt1.GetConnectedSprings().size());
	EXPECT_EQ(0u, pt1.GetConnectedTriangles().size());
	EXPECT_TRUE(pt1.IsLeaking());

    ++pointIt;

	Physics::Point const & pt2 = *pointIt;
	EXPECT_EQ(vec2f(0.0f, 3.0f), pt2.GetPosition());
	EXPECT_EQ(materials[0].get(), pt2.GetMaterial());
    EXPECT_EQ(1u, pt2.GetConnectedSprings().size());
	EXPECT_EQ(0u, pt2.GetConnectedTriangles().size());
	EXPECT_FALSE(pt2.IsLeaking());
}

TEST_F(ShipTests, BuildsPoints_EmptyShip)
{
	auto materials = MakeMaterials(
		{
			{ "Mat1", 1.0f, 1.0f, { 1, 1, 1 }, { 1, 1, 1 }, false, std::nullopt, std::nullopt },
			{ "Mat2", 1.0f, 1.0f, { 25, 30, 35 }, { 25, 30, 35 }, false, std::nullopt, std::nullopt }
		}
	);

	unsigned char imageData[] = {
		0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,
		0x00, 0x00, 0x00,	0x03, 0x02, 0x01,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,
		0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,
	};

	auto ship = Physics::Ship::Create(
        mWorld.get(),
		imageData,
		4,
		3,
		materials);

	ASSERT_TRUE(!!ship);

	EXPECT_EQ(0u, ship->GetPoints().size());
}

///////////////////////////////////////////////////////////////////////
// Springs 
///////////////////////////////////////////////////////////////////////

TEST_F(ShipTests, BuildsSprings_OneSpring)
{
	auto materials = MakeMaterials(
		{
            { "Mat1", 1.0f, 1.0f, { 40, 45, 50 }, { 40, 45, 50 }, false, std::nullopt, std::nullopt },
            { "Mat2", 1.0f, 1.0f, { 25, 30, 35 }, { 25, 30, 35 }, false, std::nullopt, std::nullopt }
        }
	);

	unsigned char imageData[] = {
		0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,
		0x00, 0x00, 0x00,	25, 30, 35,			40, 45, 50,			0x00, 0x00, 0x00,
		0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,
		0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,
		0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,
	};

	auto ship = Physics::Ship::Create(
        mWorld.get(),
		imageData,
		4,
		5,
		materials);

	ASSERT_TRUE(!!ship);

	ASSERT_EQ(1u, ship->GetSprings().size());

	Physics::Spring const * sp1 = *(ship->GetSprings().begin());
	ASSERT_NE(nullptr, sp1->GetPointA());
	EXPECT_EQ(vec2f(-1.0f, 3.0f), sp1->GetPointA()->GetPosition());
	ASSERT_NE(nullptr, sp1->GetPointB());
	EXPECT_EQ(vec2f(0.0f, 3.0f), sp1->GetPointB()->GetPosition());
	EXPECT_EQ(materials[0].get(), sp1->GetMaterial());
}

///////////////////////////////////////////////////////////////////////
// Triangles 
///////////////////////////////////////////////////////////////////////

TEST_F(ShipTests, BuildsTriangles_OneTriangle)
{
	auto materials = MakeMaterials(
		{
            { "Mat1", 1.0f, 1.0f, { 40, 45, 50 }, { 40, 45, 50 }, false, std::nullopt, std::nullopt },
            { "Mat2", 1.0f, 1.0f, { 25, 30, 35 }, { 25, 30, 35 }, false, std::nullopt, std::nullopt }
        }
	);

	unsigned char imageData[] = {
		0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,
		0x00, 0x00, 0x00,	25, 30, 35,			40, 45, 50,			0x00, 0x00, 0x00,
		0x00, 0x00, 0x00,	25, 30, 35,			0x00, 0x00, 0x00,	0x00, 0x00, 0x00,
		0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,
		0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,
	};

	auto ship = Physics::Ship::Create(
        mWorld.get(),
		imageData,
		4,
		5,
		materials);

	ASSERT_TRUE(!!ship);

	ASSERT_EQ(1u, ship->GetTriangles().size());

	Physics::Triangle const * tr1 = *(ship->GetTriangles().begin());

	ASSERT_NE(nullptr, tr1->GetPointA());
	ASSERT_NE(nullptr, tr1->GetPointB());
	ASSERT_NE(nullptr, tr1->GetPointC());

    ASSERT_EQ(1U, tr1->GetPointA()->GetConnectedTriangles().size());
    EXPECT_EQ(tr1, tr1->GetPointA()->GetConnectedTriangles()[0]);
    ASSERT_EQ(1U, tr1->GetPointB()->GetConnectedTriangles().size());
    EXPECT_EQ(tr1, tr1->GetPointB()->GetConnectedTriangles()[0]);
    ASSERT_EQ(1U, tr1->GetPointC()->GetConnectedTriangles().size());
    EXPECT_EQ(tr1, tr1->GetPointC()->GetConnectedTriangles()[0]);

	std::set<Physics::Point const *> distinctPoints;
	distinctPoints.insert(tr1->GetPointA());
	distinctPoints.insert(tr1->GetPointB());
	distinctPoints.insert(tr1->GetPointC());

	ASSERT_EQ(3u, distinctPoints.size());
}

///////////////////////////////////////////////////////////////////////
// Destroy
///////////////////////////////////////////////////////////////////////

TEST_F(ShipTests, DestroyAt)
{
    GameParameters gameParameters;

	auto materials = MakeMaterials(
		{
            { "Mat1", 1.0f, 1.0f, { 40, 45, 50 }, { 40, 45, 50 }, false, std::nullopt, std::nullopt },
            { "Mat2", 1.0f, 1.0f, { 25, 30, 35 }, { 25, 30, 35 }, false, std::nullopt, std::nullopt },

            { "XXXX", 1.0f, 1.0f, { 77, 77, 77 }, { 77, 77, 77 }, false, std::nullopt, std::nullopt },
            { "YYYY", 1.0f, 1.0f, { 66, 66, 66 }, { 66, 66, 66 }, false, std::nullopt, std::nullopt }
		}
	);

	unsigned char imageData[] = {
		0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,
		0x00, 0x00, 0x00,	66, 66, 66,			40, 45, 50,			0x00, 0x00, 0x00,
		0x00, 0x00, 0x00,	77, 77, 77,			40, 45, 50,			0x00, 0x00, 0x00,
		0x00, 0x00, 0x00,	25, 30, 35,			40, 45, 50,			0x00, 0x00, 0x00,
		0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,
	};

	auto ship = Physics::Ship::Create(
        mWorld.get(),
		imageData,
		4,
		5,
		materials);

	ASSERT_TRUE(!!ship);

	EXPECT_EQ(6u, ship->GetPoints().size());
	EXPECT_EQ(11u, ship->GetSprings().size());
	EXPECT_EQ(8u, ship->GetTriangles().size());

    Physics::Point const * testPoint = nullptr;
    Physics::Point const * sentinelPoint = nullptr;
    for (auto const & pt : ship->GetPoints())
    {
        if (pt.GetMaterial()->Name == "XXXX")
            testPoint = &pt;
        if (pt.GetMaterial()->Name == "YYYY")
            sentinelPoint = &pt;
    }

    ASSERT_NE(testPoint, nullptr);
    ASSERT_NE(sentinelPoint, nullptr);

    EXPECT_EQ(5u, testPoint->GetConnectedSprings().size());
    EXPECT_EQ(6u, testPoint->GetConnectedTriangles().size());

    EXPECT_EQ(3u, sentinelPoint->GetConnectedSprings().size());
    EXPECT_EQ(3u, sentinelPoint->GetConnectedTriangles().size());


    //
    // Destroy test point now
    //

	ship->DestroyAt(
        vec2f(-1.0f, 2.0f), 
        0.1f,
        gameParameters);

    ship->GetSprings().shrink_to_fit();
    ship->GetTriangles().shrink_to_fit();

	EXPECT_EQ(6u, ship->GetPoints().size());
	EXPECT_EQ(6u, ship->GetSprings().size());
	EXPECT_EQ(0u, ship->GetTriangles().size()); 

    EXPECT_EQ(2u, sentinelPoint->GetConnectedSprings().size());
    // These days we nuke all triangles connected to each connected point
    EXPECT_EQ(0u, sentinelPoint->GetConnectedTriangles().size());
}

///////////////////////////////////////////////////////////////////////
// Lamps 
///////////////////////////////////////////////////////////////////////

TEST_F(ShipTests, BuildsLamps_OneLamp)
{
    auto materials = MakeMaterials(
        {
            { "Mat1", 1.0f, 1.0f, { 40, 45, 50 }, { 40, 45, 50 }, false, std::nullopt, std::nullopt },
            { "Mat2", 1.0f, 1.0f, { 25, 30, 35 }, { 25, 30, 35 }, false, {{Material::ElectricalProperties::ElectricalElementType::Lamp, 0.0f, 0.0f}}, std::nullopt }
        }
    );

    unsigned char imageData[] = {
        0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,
        0x00, 0x00, 0x00,	25, 30, 35,			40, 45, 50,			0x00, 0x00, 0x00,
        0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,
        0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,
        0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,
    };

    auto ship = Physics::Ship::Create(
        mWorld.get(),
        imageData,
        4,
        5,
        materials);

    ASSERT_TRUE(!!ship);

    ASSERT_EQ(1u, ship->GetElectricalElements().size());

    Physics::ElectricalElement const * el1 = *(ship->GetElectricalElements().begin());
    EXPECT_EQ(Physics::ElectricalElement::Type::Lamp, el1->GetType());
    EXPECT_EQ(vec2f(-1.0f, 3.0f), el1->GetPoint()->GetPosition());
}
