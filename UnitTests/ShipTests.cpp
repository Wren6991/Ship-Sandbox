#include <GameLib/Physics.h>

#include <initializer_list>

#include "gtest/gtest.h"

class ShipTests : public ::testing::Test
{
protected:

	static std::vector<std::unique_ptr<Material const>> MakeMaterials(std::vector<Material> && materials)	
	{
		std::vector<std::unique_ptr<Material const>> res;
		for (Material const & m : materials)
			res.emplace_back(new Material(m));

		return res;
	}
};


///////////////////////////////////////////////////////////////////////
// Points
///////////////////////////////////////////////////////////////////////

TEST_F(ShipTests, BuildsPoints_OnePoint)
{
	Physics::World wld;

	auto materials = MakeMaterials(
		{
			{ "Mat1", 1.0f, 1.0f, { 0.1f, 0.1f, 0.1f }, false, std::nullopt },
			{ "Mat2", 1.0f, 1.0f, { 25.f/255.f, 30.f/255.f, 35.f/255.f }, false, std::nullopt }
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
		&wld,
		imageData,
		4,
		5,
		materials);

	ASSERT_TRUE(!!ship);

	ASSERT_EQ(1u, ship->GetPoints().size());

	Physics::Point const * pt = *(ship->GetPoints().begin());
	EXPECT_EQ(vec2f(-1.0f, 3.0f), pt->GetPosition());
	EXPECT_EQ(materials[1].get(), pt->GetMaterial());
	EXPECT_EQ(0u, pt->GetTriangles().size());
	EXPECT_TRUE(pt->GetIsLeaking());

	EXPECT_EQ(0u, ship->GetPointAdjencyMap().size());
}

TEST_F(ShipTests, BuildsPoints_TwoPoints)
{
	Physics::World wld;

	auto materials = MakeMaterials(
		{
			{ "Mat1", 1.0f, 1.0f,{ 40.f / 255.f, 45.f / 255.f, 50.f / 255.f }, true, std::nullopt },
			{ "Mat2", 1.0f, 1.0f,{ 25.f / 255.f, 30.f / 255.f, 35.f / 255.f }, false, std::nullopt }
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
		&wld,
		imageData,
		4,
		5,
		materials);

	ASSERT_TRUE(!!ship);

	ASSERT_EQ(2u, ship->GetPoints().size());

	Physics::Point const * pt1 = *(ship->GetPoints().begin());
	EXPECT_EQ(vec2f(-1.0f, 3.0f), pt1->GetPosition());
	EXPECT_EQ(materials[1].get(), pt1->GetMaterial());
	EXPECT_EQ(0u, pt1->GetTriangles().size());
	EXPECT_TRUE(pt1->GetIsLeaking());

	Physics::Point const * pt2 = *std::next((ship->GetPoints().begin()));
	EXPECT_EQ(vec2f(0.0f, 3.0f), pt2->GetPosition());
	EXPECT_EQ(materials[0].get(), pt2->GetMaterial());
	EXPECT_EQ(0u, pt2->GetTriangles().size());
	EXPECT_FALSE(pt2->GetIsLeaking());

	// Adjacent nodes

	EXPECT_EQ(2u, ship->GetPointAdjencyMap().size());

	ASSERT_EQ(1u, ship->GetPointAdjencyMap().count(const_cast<Physics::Point *>(pt1)));
	ASSERT_EQ(1u, ship->GetPointAdjencyMap().at(const_cast<Physics::Point *>(pt1)).size());
	EXPECT_EQ(pt2, *(ship->GetPointAdjencyMap().at(const_cast<Physics::Point *>(pt1)).begin()));

	ASSERT_EQ(1u, ship->GetPointAdjencyMap().count(const_cast<Physics::Point *>(pt2)));
	ASSERT_EQ(1u, ship->GetPointAdjencyMap().at(const_cast<Physics::Point *>(pt2)).size());
	EXPECT_EQ(pt1, *(ship->GetPointAdjencyMap().at(const_cast<Physics::Point *>(pt2)).begin()));
}

TEST_F(ShipTests, BuildsPoints_EmptyShip)
{
	Physics::World wld;

	auto materials = MakeMaterials(
		{
			{ "Mat1", 1.0f, 1.0f,{ 0.1f, 0.1f, 0.1f }, false, std::nullopt },
			{ "Mat2", 1.0f, 1.0f,{ 25.f / 255.f, 30.f / 255.f, 35.f / 255.f }, false, std::nullopt }
		}
	);

	unsigned char imageData[] = {
		0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,
		0x00, 0x00, 0x00,	0x03, 0x02, 0x01,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,
		0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,
	};

	auto ship = Physics::Ship::Create(
		&wld,
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
	Physics::World wld;

	auto materials = MakeMaterials(
		{
			{ "Mat1", 1.0f, 1.0f,{ 40.f / 255.f, 45.f / 255.f, 50.f / 255.f }, false, std::nullopt },
			{ "Mat2", 1.0f, 1.0f,{ 25.f / 255.f, 30.f / 255.f, 35.f / 255.f }, false, std::nullopt }
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
		&wld,
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
	Physics::World wld;

	auto materials = MakeMaterials(
		{
			{ "Mat1", 1.0f, 1.0f,{ 40.f / 255.f, 45.f / 255.f, 50.f / 255.f }, false, std::nullopt },
			{ "Mat2", 1.0f, 1.0f,{ 25.f / 255.f, 30.f / 255.f, 35.f / 255.f }, false, std::nullopt }
		}
	);

	unsigned char imageData[] = {
		0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,
		0x00, 0x00, 0x00,	25, 30, 35,			40, 45, 50,			0x00, 0x00, 0x00,
		0x00, 0x00, 0x00,	25, 30, 35,			0x00, 0x00, 0x00,	0x00, 0x00, 0x00,
		0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,
		0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,
	};

	// TODOTEST
	auto shipTODO = Physics::Ship::Create(
		&wld,
		imageData,
		4,
		5,
		materials);

	ASSERT_TRUE(!!shipTODO);

	// TODOTEST
	Physics::Ship * ship = shipTODO.get();
	wld.AddShip(std::move(shipTODO));

	ASSERT_EQ(1u, ship->GetTriangles().size());

	Physics::Triangle const * tr1 = *(ship->GetTriangles().begin());

	ASSERT_NE(nullptr, tr1->GetPointA());
	ASSERT_NE(nullptr, tr1->GetPointB());
	ASSERT_NE(nullptr, tr1->GetPointC());

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
	Physics::World wld;

	auto materials = MakeMaterials(
		{
			{ "Mat1", 1.0f, 1.0f,{ 40.f / 255.f, 45.f / 255.f, 50.f / 255.f }, true, std::nullopt },
			{ "Mat2", 1.0f, 1.0f,{ 25.f / 255.f, 30.f / 255.f, 35.f / 255.f }, false, std::nullopt }
		}
	);

	unsigned char imageData[] = {
		0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,
		0x00, 0x00, 0x00,	25, 30, 35,			40, 45, 50,			0x00, 0x00, 0x00,
		0x00, 0x00, 0x00,	25, 30, 35,			40, 45, 50,			0x00, 0x00, 0x00,
		0x00, 0x00, 0x00,	25, 30, 35,			40, 45, 50,			0x00, 0x00, 0x00,
		0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,	0x00, 0x00, 0x00,
	};

	// TODOTEST
	auto shipTODO = Physics::Ship::Create(
		&wld,
		imageData,
		4,
		5,
		materials);

	ASSERT_TRUE(!!shipTODO);

	// TODOTEST
	Physics::Ship * ship = shipTODO.get();
	wld.AddShip(std::move(shipTODO));

	EXPECT_EQ(6u, ship->GetPoints().size());
	EXPECT_EQ(11u, ship->GetSprings().size());
	EXPECT_EQ(8u, ship->GetTriangles().size());
	EXPECT_EQ(6u, ship->GetPointAdjencyMap().size());

	// TODO: verify all relationships

	wld.DestroyAt(vec2f(-1.0f, 2.0f), 0.1f);

	EXPECT_EQ(5u, ship->GetPoints().size());
	EXPECT_EQ(11u, ship->GetSprings().size());
	EXPECT_EQ(2u, ship->GetTriangles().size());
	EXPECT_EQ(5u, ship->GetPointAdjencyMap().size());

	// TODO: verify all new relationships
}
