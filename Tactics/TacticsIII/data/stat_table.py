import json

##################### Load data
primary_stats=json.load(open("primary_stats.json"))
secondary_stats=json.load(open("secondary_stats.json"))

################### BUILD stats

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
