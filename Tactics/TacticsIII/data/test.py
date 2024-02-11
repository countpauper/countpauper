import sys
import json


##################### Load data
primary_stats=json.load(open("primary_stats.json"))
secondary_stats=json.load(open("secondary_stats.json"))
archetypes=json.load(open("archetypes.json"))
builds=json.load(open("builds.json"))
armor_stats=json.load(open("armor_stats.json"))
race_stats=json.load(open("race_stats.json"))


################ Test data

def test_primary_table_length(stats, req, key="table"):
        return [f"length of {key} in {name} is not {req}" for name, stat in stats.items() if len(stat.get(key,[])) != req]


def test_secondaries_exist(primary, secondary):
        return [f"boost {boost} in {name} does not exist"
                for name, stat in primary.items() for boost in stat.get("boost") if boost not in secondary]

def compute_range(boost, base, primary_stats):
        return [base, base+sum(
                stat["boost"].count(boost) for stat in primary_stats
        )]

def test_secondary_range(primary_stats, seconadry_stats):
        return [f"range of {secondary} is {computed_range} but should be {range}"
                for secondary, stats in seconadry_stats.items()
                        if (computed_range:=compute_range(secondary, stats.get("base", 0), primary_stats.values())) != (range:=stats.get("range"))]
def test_stats():
        return (test_primary_table_length(primary_stats, 15, "boost") +
                test_secondaries_exist(primary_stats, secondary_stats.keys()) +
                test_secondary_range(primary_stats, secondary_stats))




nl="\n"
if __name__ == "__main__":
        if test_result := test_stats():
                print(f"tests failed: {nl.join(test_result)}")
                exit()
        print("tests passed")
