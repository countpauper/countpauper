import sys
import json
from format import format_table


##################### Load data
primary_stats=json.load(open("primary_stats.json"))
secondary_stats=json.load(open("secondary_stats.json"))
# armor_stats=json.load(open("armor_stats.json"))
# race_stats=json.load(open("race_stats.json"))
tertiary_stats=json.load(open("tertiary_stats.json"))
item_stats=json.load(open("item_stats.json"))

################### BUILD characters

def build_stat_tables():
        result = dict()
        for name, stat in primary_stats.items():
                boosts = {s: secondary_stats.get(s, dict()) for s in set(stat.get("boost")) }
                secondary_stat = {s:stat.get("base",0) * stat.get("multiplier", 1) for s, stat in boosts.items()}
                table = [dict(secondary_stat)]
                for boost in stat.get("boost"):
                        secondary_stat[boost] = secondary_stat.get(boost, 0) + boosts.get(boost).get("multiplier", 1)
                        table.append(dict(secondary_stat))
                result[name] = table
        return result

def transpose_tables(stat_table):
        result = dict()
        for lvl, row in enumerate(stat_table):
                for stat, value in row.items():
                        result[stat] = result.get(stat, [])+[value]
                        assert len(result[stat]) == lvl+1
        return result

nl="\n"
if __name__ == "__main__":
        stat_tables = build_stat_tables()

        for stat,table in stat_tables.items():
                print(f"{stat}:")
                print(format_table(table))

        stats = dict()
        for stat, props  in primary_stats.items():
                stats[stat] = {prop:value for prop,value in props.items() if prop in ['range']}
                for secondary, table in transpose_tables(stat_tables[stat]).items():
                        props=dict(depends=stat)
                        secondary_stat = secondary_stats[secondary]
                        if mul := secondary_stat.get("multiplier"):
                                table=[int(v/mul) for v in table]
                        props["table"] = table;
                        props.update(secondary_stats[secondary])
                        stats[secondary] = props
        stats.update(tertiary_stats)
        # stats.update(item_stats)
        with open('creature.json', 'w') as f:
                json.dump(stats, f)

