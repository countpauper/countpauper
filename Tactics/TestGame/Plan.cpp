#include <boost/test/unit_test.hpp>
#include <numeric>
#include "Game/Plan.h"
#include "Data.h"

namespace Game
{
namespace Test
{
BOOST_AUTO_TEST_SUITE(PlanTest);


template<typename _KT>
double Sum(const std::map<_KT,double>& histogram)
{
	return std::accumulate(histogram.begin(), histogram.end(), 0.0, [](double value, const std::pair<_KT,double>& bin)
	{
		return value + bin.second;
	});
}

BOOST_AUTO_TEST_CASE(CenterMassDefault)
{
	auto cm = Plan::CenterMass(0);
	BOOST_CHECK_CLOSE(cm[-1], cm[0],0.001);
	BOOST_CHECK_GT(cm[0], cm[1]);
	BOOST_CHECK_GT(cm.begin()->second, 0.001);
	BOOST_CHECK_GT(cm.rbegin()->second, 0.001);
	BOOST_CHECK_CLOSE(cm[0] + cm[-1], 0.6827, 0.01);
	BOOST_CHECK_CLOSE(cm[-2] + cm[-1] + cm[0] + cm[1], 0.9545, 0.01);
	BOOST_CHECK_CLOSE(Sum(cm), 1.0, 0.001);
}

BOOST_AUTO_TEST_CASE(CenterMassOffset)
{
	auto cm = Plan::CenterMass(2);
	BOOST_CHECK_CLOSE(cm[1], cm[2], 0.001);
	BOOST_CHECK_CLOSE(cm[0] + cm[1] + cm[2] + cm[3], 0.9545, 0.01);
	BOOST_CHECK_CLOSE(Sum(cm), 1.0, 0.001);
}

BOOST_AUTO_TEST_CASE(CenterMassOffsetHalf)
{
	auto cm = Plan::CenterMass(0.5, 0.5);
	BOOST_CHECK_CLOSE(cm[0], 0.6827, 0.01);
	BOOST_CHECK_CLOSE(Sum(cm), 1.0, 0.001);
}

BOOST_AUTO_TEST_CASE(CenterMassSigma2)
{
	auto cm = Plan::CenterMass(0,2);
	BOOST_CHECK_CLOSE(cm[-2] + cm[-1] + cm[0] + cm[1], 0.6827, 0.01);
	BOOST_CHECK_CLOSE(Sum(cm), 1.0, 0.001);
}

BOOST_AUTO_TEST_CASE(CenterMassTreshold)
{
	auto cm = Plan::CenterMass(0,1,0.1);

	BOOST_CHECK_GT(cm.begin()->second, 0.1);
	BOOST_CHECK_GT(cm.rbegin()->second, 0.1);
	BOOST_CHECK_CLOSE(Sum(cm), 1.0, 0.001);
}

BOOST_AUTO_TEST_CASE(SwingLocation)
{
	Data::Knight attacker;
	Data::Mage defender;
	auto hls = Plan::HitLocations(attacker, attacker.melee, defender, Direction::down);
	BOOST_CHECK_EQUAL(hls.size(), 1);
	BOOST_CHECK_CLOSE(Sum(hls), 1.0, 0.001);
	const auto& hl = hls.begin()->first;
	BOOST_CHECK_EQUAL(hl.plane, Plane({ Direction::up }));
	BOOST_CHECK_EQUAL(hl.size, 3);
	BOOST_CHECK_EQUAL(hl.position, 4);
}

BOOST_AUTO_TEST_CASE(CenterLocation)
{
	Data::Mage attacker;
	Data::Knight defender;
	auto hls = Plan::HitLocations(attacker, attacker.zap, defender, Direction::forward);
	BOOST_CHECK_CLOSE(Sum(hls), 1.0, 0.001);
	BOOST_CHECK_CLOSE(Sum(hls), 1.0, 0.001);
	BOOST_REQUIRE_EQUAL(defender.body.Anatomical().Length(), 5);
	BOOST_CHECK_GE(hls.size(), defender.body.Anatomical().Length());
}

BOOST_AUTO_TEST_CASE(AimSwing)
{
	Data::Knight attacker;
	BOOST_REQUIRE(attacker.melee.HasTargeting(::Game::Targeting::Swing));
	Data::Mage defender;
	auto[hits, direction] = Plan::Aim(attacker, defender, attacker.melee);
	BOOST_CHECK(attacker.melee.trajectory.count(direction));
	BOOST_CHECK_EQUAL(hits.size(), 1);
	BOOST_CHECK_CLOSE(Sum(hits), 1.0, 0.001);
	const auto& hit = *hits.begin();
	BOOST_CHECK_EQUAL(hit.second, 1.0);
	BOOST_CHECK_EQUAL(hit.first, &defender.body.Anatomical()[L"Head"]);
}

BOOST_AUTO_TEST_CASE(AimCenter)
{
	Data::Mage attacker;
	BOOST_REQUIRE(attacker.zap.HasTargeting(::Game::Targeting::Center));
	Data::Knight defender;
	auto[hits, direction] = Plan::Aim(attacker, defender, attacker.zap);
	BOOST_CHECK(attacker.zap.trajectory.count(direction));
	BOOST_CHECK_EQUAL(hits.size(), 5);
	BOOST_CHECK_CLOSE(Sum(hits), 1.0, 0.001);
	BOOST_CHECK(hits.count(&defender.body.Anatomical()[L"Legs"]));
	BOOST_CHECK(hits.count(&defender.body.Anatomical()[L"Belly"]));
	BOOST_CHECK(hits.count(&defender.body.Anatomical()[L"Chest"]));
	BOOST_CHECK(hits.count(&defender.body.Anatomical()[L"Head"]));
	BOOST_CHECK(hits.count(nullptr));
}


BOOST_AUTO_TEST_SUITE_END()
}
}  // Test