<drac2>
# left 7 Aarakorca, Dragonborn, Tabaxi, Tiefling Genasi, Goliath, Aasimar, Bugbear, Firbolg, Goblin, Hobgoblin, Kenku, Kobold, Lizardfolk, Orc, Tabaxi, Triton, Yuan-Ti, Fey, but names ...
races={"Human":65,"Dwarf":10,"Half Orc":5,"Elf":4,"Halfling":4,"Gnome":2,"Half Elf":1}
race_names={"Half Elf":["Human","Elf"],"Half Orc":["Human","Orc"]}	# TODO more race data like ages
# TODO: half elves and half orcs, use combination tables of human/elf and human/orc
genders={'Male':50,'Female':50}	# TODO: non binary, Race specific
ages={'Child':10,'Young':15,'Adult':20,'Senior':20,'Elderly':20}	# TODO: per race

race,gender,min_age=None,None,0
for arg in &ARGS&:
	arg=arg.title()
	if arg in races:
		race=arg
	elif arg in genders:
		gender=arg
	elif arg.isdecimal():	# TODO range or key, also relative to race age
		min_age=int(arg)
	else:
		return f'echo Unrecognized race: `{arg}`.'

# TODO: subraces for each or total
if not race:
	race_rnd=randint(sum(races.values()))
	race,gender=None,None
	for r,v in races.items():
		if race_rnd<v:
			race=r
			break
		else:
			race_rnd-=v

if not gender:
	gender_rnd=randint(sum(genders.values()))
	for g,v in genders.items():
		if gender_rnd<v:
			gender=g
			break
		else:
			gender_rnd-=v

ages_rnd=max(min_age,randint(sum(ages.values())))
for a,v in ages.items():
	if ages_rnd<v:
		age=a
		break
	else:
		ages_rnd-=v

names=load_json(get_gvar('b0d2932d-7408-4998-ad7d-226d83eacee1'))
name_races=race_names.get(race,[race])
name_keys=['Male','Female','Neutral','Surname']
race_names={k:[] for k in name_keys}
for rn in name_races:
	if more_names:=names.get(rn,{}):
		for k in name_keys:
			race_names[k]+=more_names.get(k,[])

gender_names=race_names.get(gender,{})+race_names.get('Neutral',[])
npc_name=f'{gender_names[randint(len(gender_names))]}'

if sur_names:=race_names.get('Surname'):
	npc_name=f'{npc_name} {sur_names[randint(len(sur_names))]}'
return f'echo {npc_name} {age} {gender} {race}'
</drac2>
