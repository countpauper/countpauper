import json

trait_txt="""Profession/traits	Generic 	Spiritual	Criminal	Martial	Marine	Artisinal	Scholarly	Outlandish	Performer	Tribal
Soldier/Officer	30	5	10	30	10			5		10
Officer	20	10	5	30	20		10	5		
Fisherman/Sailor/Mariner	50				30	10		10		
Pirate	30		20	20	30					
Shipwright	50				15	30	5			
Miner	100									
Prospector	70							30		
Guide	60							30		10
Scout/Ranger	50			10				30		10
Druid	30	10					20	30		10
Herbalist	50					10	20	20		
Witch	30	20				10	10	20		10
Highwayman	30		40	10				20		
Baker/Carpenter/Cooper/Fletcher/Butcher etc	70	2	4			20	2	2		
Dungsweeper	60	10	10				10		10	
Carter, coachman	70		10					20		
Apothecary / Physician	50	10				10	30			
Jester / Actor / Minstrel	50		5			5	10		30	
Magister	60	5	5			10	20			
Beggar 	50	10	10				10	10	10	
Pickpocket	70		30							
Scribe/Historian/Wizard	50	5				5	40			
Engineer	70					20	10			
Priest/Monk	40	40					20			
Farmer/Shepherd	80							20		
Savage	30	5	10	15				10		30"""

trait_table_txt=[line.split('\t') for line in trait_txt.splitlines()]
trait_header=trait_table_txt.pop(0)
def trait_percentage(s):
    s=s.strip()
    if not s:
        return 0
    else:
        return int(s)

trait_table={trait_table_row[0]:[trait_percentage(s) for s in trait_table_row[1:]] for trait_table_row in trait_table_txt}
prof_table=dict()
for k, percentages in trait_table.items():
    if (p:=sum(percentages))!=100:
        print(f"Row {k} is not normalized ({p}).")
    for profession in k.split('/'):
        prof_table[profession] = {trait.lower():percentage for trait, percentage in zip(trait_header[1:], percentages)}

final=dict(prof=[
    dict(profession=prof,
         trait={f"@trait.{trait}": percentage for trait,percentage in prof_row.items() if percentage>0},
         goal=dict())
    for prof, prof_row in prof_table.items()])
print(json.dumps(final))