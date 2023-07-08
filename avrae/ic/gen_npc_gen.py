import json

trait_txt="""Profession/traits,Generic ,Spiritual,Criminal,Law,Martial,Marine,Mercantile,Artisanal,Scholarly,Outlandish,Performer,Tribal
Soldier,20,5,10,10,30,10,,,,5,,10
Officer,20,10,5,10,25,20,,,5,5,,
Watchman/Sherriff,25,5,10,30,20,,,,5,5,,
Fisherman/Sailor/Mariner,45,,,5,,30,,10,,10,,
Pirate,30,,20,,20,30,,,,,,
Shipwright,40,,,,,15,10,30,5,,,
Miner,100,,,,,,,,,,,
Prospector,70,,,,,,,,,30,,
Guide,55,,,5,,,,,,30,,10
Scout/Ranger/Hunter,40,,,10,10,,,,,30,,10
Druid,30,10,,,,,,,20,30,,10
Herbalist,50,,,,,,,10,20,20,,
Witch,30,10,,,,,10,10,10,20,,10
Highwayman,30,,40,,10,,,,,20,,
brewer/jeweler/baker/carpenter/cooper/grocer/fletcher/butcher/glassblower/mason/potter/tanner/weaver/cobbler/stablemaster/blacksmith,40,2,4,,,,30,20,2,2,,
Innkeeper,10,10,5,,10,10,10,10,10,10,10,5
Dungsweeper,60,10,10,,,,,,10,,10,
Carter/Coachman,60,,10,,,,10,,,20,,
Apothecary/Physician,40,10,,,,,20,10,20,,,
Jester/Actor/Minstrel,50,,5,,,,,5,10,,30,
Magister,40,5,5,20,,,,10,20,,,
Beggar ,40,10,10,,,,10,,10,10,10,
Pickpocket,70,,30,,,,,,,,,
Scribe/Historian/Wizard,40,5,,10,,,,5,40,,,
Engineer,70,,,,,,,20,10,,,
Priest/Monk,35,40,,5,,,,,20,,,
Farmer/Shepherd,80,,,,,,,,,20,,
Miller,60,,,,,,20,20,,,,
Raider,30,5,10,,15,,,,,10,,30"""

goal_txt="""Profession/goals,Generic ,Spiritual,Criminal,Law,Martial,Marine,Mercantile,Artisanal,Scholarly,Outlandish,Performer,Tribal
Soldier,70,,,,30,,,,,,,
Officer,50,,,10,40,,,,,,,
Watchman/Sherriff,50,,,30,20,,,,,,,
Fisherman/Sailor/Mariner,90,,,,,10,,,,,,
Pirate,40,,20,,,40,,,,,,
Shipwright,85,,,,,5,,10,,,,
Miner,100,,,,,,,,,,,
Prospector,90,,,,,,,,,10,,
Guide,80,,,,,,,,,20,,
Scout/Ranger/Hunter,70,,,10,,,,,,10,,10
Druid,60,10,,,,,,,,,,30
Herbalist,70,,,,,,,,,10,,20
Witch,80,,,,,,,,10,,,10
Highwayman,40,,50,,,,,,,10,,
brewer/jeweler/baker/carpenter/cooper/grocer/fletcher/butcher/glassblower/mason/potter/tanner/weaver/cobbler/stablemaster/blacksmith,60,,,,,,30,10,,,,
Innkeeper,80,,,,,,20,,,,,
Dungsweeper,100,,,,,,,,,,,
Carter/Coachman,90,,,,,,10,,,,,
Apothecary/Physician,70,,,,,,20,10,,,,
Jester/Actor/Minstrel,50,,,,,,,,,,50,
Magister,90,,,10,,,,,,,,
Beggar ,100,,,,,,,,,,,
Pickpocket,60,,40,,,,,,,,,
Scribe/Historian/Wizard,70,,,,,,,,30,,,
Engineer,80,,,,,,,,20,,,
Priest/Monk,30,50,,,,,,,20,,,
Farmer/Shepherd,100,,,,,,,,,,,
Miller,80,,,,,,20,,,,,
Raider,30,,,,,,,,,20,,50"""

belief_txt = """Profession/beliefs,Generic ,Spiritual,Criminal,Law,Martial,Marine,Mercantile,Artisanal,Scholarly,Outlandish,Performer,Tribal
Soldier,10,5,10,10,20,10,5,5,,10,5,10
Officer,15,5,5,10,30,5,5,5,5,5,5,5
Watchman/Sherriff,10,5,5,25,20,5,5,5,5,5,5,5
Fisherman/Sailor/Mariner,25,5,5,5,5,30,10,5,,5,,5
Pirate,15,5,20,,20,5,5,5,5,10,5,5
Shipwright,25,5,5,5,5,10,10,20,5,5,,5
Miner,55,5,5,5,5,,5,5,,5,5,5
Prospector,15,5,5,5,5,5,10,5,5,30,5,5
Guide,5,5,5,5,5,10,5,,5,30,5,20
Scout/Ranger/Hunter,15,5,5,10,5,5,5,5,5,30,,10
Druid,10,15,,5,5,,5,5,10,30,5,10
Herbalist,5,10,5,5,,5,15,20,10,10,,15
Witch,20,20,10,,,,10,5,10,10,5,10
Highwayman,10,5,30,,10,,5,,5,20,5,10
brewer/jeweler/baker/carpenter/cooper/grocer/fletcher/butcher/glassblower/mason/potter/tanner/weaver/cobbler/stablemaster/blacksmith,25,5,,5,5,,25,25,10,,,
Innkeeper,20,10,5,5,10,10,15,5,5,5,10,
Dungsweeper,40,10,5,10,5,5,,5,10,5,5,
Carter/Coachman,10,5,5,10,5,10,20,,5,20,5,5
Apothecary/Physician,25,5,5,5,,5,15,20,5,5,,10
Jester/Actor/Minstrel,20,5,5,5,,10,15,10,,,30,
Magister,25,10,5,15,,,15,10,20,,,
Beggar ,45,10,10,5,,,10,,5,5,5,5
Pickpocket,50,,20,,10,,5,,,5,5,5
Scribe/Historian/Wizard,30,10,,10,,,10,10,25,,5,
Engineer,35,,,10,,5,10,30,10,,,
Priest/Monk,15,30,5,10,,,5,5,20,,,10
Farmer/Shepherd,40,5,5,10,,,5,,,20,,15
Miller,35,5,5,10,,,15,20,,5,,5
Raider,10,5,5,,15,,5,5,,20,5,30"""
prof_categories=dict(
    military=dict(soldier=80, officer=20),
    coastal=dict(fisherman=40,sailor=25,pirate=15,mariner=10,shipwright=10),
    mountain=dict(miner=60, prospector=25, guide=15),
    forest=dict(hunter=20,scout=15,ranger=10,druid=10, herbalist=15,witch=10,highwayman=20),
    town=dict(baker=7,  carpenter=8,  cooper=4,  grocer=4,  dungsweeper=4,  carter=4,  coachman=3,  fletcher=3,  innkeeper=5,
         butcher=4,  glassblower=3,  mason=7,  potter=5,  jeweler=2,  tanner=5,  weaver=8,  cobbler=4,  stablemaster=3,
         brewer=5,  blacksmith=5, watchman=7),
    city=dict(apothecary=10,  physician=5, sherriff=5, jester=10,   actor=10,  minstrel=10,  magister=20,  beggar=20,  pickpocket=10),
    scholar=dict(scribe=20,  historian=20,  wizard=10,  engineer=20,  priest=20,  monk=10),
    rural=dict(farmer=50, miller=10, shepherd=20, raider=20)
)

def threshold_table(distribution):
    threshold=0
    result = dict()
    for key, percentage in distribution.items():
        threshold += percentage
        result[threshold] = key
    return result

categorized_profs = {cat:threshold_table(profs) for cat, profs in prof_categories.items()}

def parse_table(name, txt):
    rows=[line.split(',') for line in txt.splitlines()]
    header=rows.pop(0)
    def percentage(s):
        s=s.strip()
        if not s:
            return 0
        else:
            return int(s)

    parsed={row[0].strip():[percentage(s) for s in row[1:]] for row in rows}
    table=dict()
    for k, percentages in parsed.items():
        if (p:=sum(percentages)) != 100:
            print(f"Row {k} of {name} is not normalized ({p}).")
        for profession in k.split('/'):
            table[profession.lower()] = {f"@{name}.{group.lower().strip()}":percentage for group, percentage in zip(header[1:], percentages) if percentage>0}
    return table
trait_table=parse_table('traits', trait_txt)
goal_table=parse_table('goals', goal_txt)
belief_table=parse_table('beliefs', belief_txt)
merged={cat : {threshold:dict(profession=prof,
                              trait=trait_table.get(prof, []),
                              goal=goal_table.get(prof, []),
                              belief=belief_table.get(prof, []))
               for threshold, prof in profs.items() }
        for cat, profs in categorized_profs.items() }
final=merged
def check(prop, professions):
    return [prof['profession'] for cat, table in professions.items() for prof in table.values() if not prof[prop]]

if missing := check('trait', merged):
    print(f"Missing traits for {missing}")
if missing := check('goal', merged):
    print(f"Missing goals for {missing}")
if missing := check('belief', merged):
    print(f"Missing beliefs for {missing}")
print(json.dumps(final))