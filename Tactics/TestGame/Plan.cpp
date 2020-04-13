#include "pch.h"
#include <numeric>
#include "Game/Plan.h"
#include "Data.h"

namespace Game
{
namespace Test
{

template<typename _KT>
double Sum(const std::map<_KT,double>& histogram)
{
	return std::accumulate(histogram.begin(), histogram.end(), 0.0, [](double value, const std::pair<_KT,double>& bin)
	{
		return value + bin.second;
	});
}

TEST(PlanTest, CenterMassDefault)
{
	auto cm = Plan::CenterMass(0);
	EXPECT_NEAR(cm[-1], cm[0],0.001);
	EXPECT_GT(cm[0], cm[1]);
	EXPECT_GT(cm.begin()->second, 0.001);
	EXPECT_GT(cm.rbegin()->second, 0.001);
	EXPECT_NEAR(cm[0] + cm[-1], 0.6827, 0.01);
	EXPECT_NEAR(cm[-2] + cm[-1] + cm[0] + cm[1], 0.9545, 0.01);
	EXPECT_NEAR(Sum(cm), 1.0, 0.001);
}

TEST(PlanTest, CenterMassOffset)
{
	auto cm = Plan::CenterMass(2);
	EXPECT_NEAR(cm[1], cm[2], 0.001);
	EXPECT_NEAR(cm[0] + cm[1] + cm[2] + cm[3], 0.9545, 0.01);
	EXPECT_NEAR(Sum(cm), 1.0, 0.001);
}

TEST(PlanTest, CenterMassOffsetHalf)
{
	auto cm = Plan::CenterMass(0.5, 0.5);
	EXPECT_NEAR(cm[0], 0.6827, 0.01);
	EXPECT_NEAR(Sum(cm), 1.0, 0.001);
}

TEST(PlanTest, CenterMassSigma2)
{
	auto cm = Plan::CenterMass(0,2);
	EXPECT_NEAR(cm[-2] + cm[-1] + cm[0] + cm[1], 0.6827, 0.01);
	EXPECT_NEAR(Sum(cm), 1.0, 0.001);
}

TEST(PlanTest, CenterMassTreshold)
{
	auto cm = Plan::CenterMass(0,1,0.1);

	EXPECT_GT(cm.begin()->second, 0.1);
	EXPECT_GT(cm.rbegin()->second, 0.1);
	EXPECT_NEAR(Sum(cm), 1.0, 0.001);
}

TEST(PlanTest, SwingLocation)
{
	Data::Knight attacker;
	Data::Mage defender;
	auto hls = Plan::HitLocations(attacker, attacker.melee, defender, Direction::down);
	EXPECT_EQ(hls.size(), 1);
	EXPECT_NEAR(Sum(hls), 1.0, 0.001);
	const auto& hl = hls.begin()->first;
	EXPECT_EQ(hl.plane, Plane({ Direction::up }));
	EXPECT_EQ(hl.size, 3);
	EXPECT_EQ(hl.position, 4);
}

TEST(PlanTest, CenterLocation)
{
	Data::Mage attacker;
	Data::Knight defender;
	auto hls = Plan::HitLocations(attacker, attacker.zap, defender, Direction::forward);
	EXPECT_NEAR(Sum(hls), 1.0, 0.001);
	EXPECT_NEAR(Sum(hls), 1.0, 0.001);
	ASSERT_EQ(defender.body.Anatomical().Length(), 5);
	EXPECT_GE(hls.size(), defender.body.Anatomical().Length());
}

TEST(PlanTest, AimSwing)
{
	Data::Knight attacker;
	ASSERT_TRUE(attacker.melee.HasTargeting(::Game::Targeting::Swing));
	Data::Mage defender;
	auto[hits, direction] = Plan::Aim(attacker, defender, attacker.melee);
	EXPECT_TRUE(attacker.melee.trajectory.count(direction));
	EXPECT_EQ(hits.size(), 1);
	EXPECT_NEAR(Sum(hits), 1.0, 0.001);
	const auto& hit = *hits.begin();
	EXPECT_EQ(hit.second, 1.0);
	EXPECT_EQ(hit.first, &defender.body.Anatomical()[L"Head"]);
}

TEST(PlanTest, AimCenter)
{
	Data::Mage attacker;
    ASSERT_TRUE(attacker.zap.HasTargeting(::Game::Targeting::Center));
	Data::Knight defender;
	auto[hits, direction] = Plan::Aim(attacker, defender, attacker.zap);
    EXPECT_TRUE(attacker.zap.trajectory.count(direction));
	EXPECT_EQ(hits.size(), 5);
	EXPECT_NEAR(Sum(hits), 1.0, 0.001);
	EXPECT_TRUE(hits.count(&defender.body.Anatomical()[L"Legs"]));
	EXPECT_TRUE(hits.count(&defender.body.Anatomical()[L"Belly"]));
	EXPECT_TRUE(hits.count(&defender.body.Anatomical()[L"Chest"]));
	EXPECT_TRUE(hits.count(&defender.body.Anatomical()[L"Head"]));
	EXPECT_TRUE(hits.count(nullptr));
}


}
}  // Test