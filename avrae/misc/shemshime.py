embed -title "Evil Influence"
<drac2>
args=argparse(&ARGS&)
dc_sing=args.last('sing',5,int)
dc_hum=args.last('hum',10,int)
influence_threshold=args.last('influence',4, int)
infected=load_json(get('shemshime','{}'))
infected_saves={i:vroll(s) for i,s in infected.items()}


nl='\n'
fields=[]
if singing:=[i for i,r in infected_saves.items() if r.total<dc_sing]:
	fields.append(f'-f "Singing [DC {dc_sing}]|{nl.join(f"{i} - {r}" for i,r in infected_saves.items() if i in singing)}"')
if humming:=[i for i,r in infected_saves.items() if r.total>=dc_sing and r.total<dc_hum]:
	fields.append(f'-f "Humming [DC {dc_hum}]|{nl.join(f"{i} - {r}" for i, r in infected_saves.items() if i in humming)}"')
if saved:=[i for i in infected_saves.keys() if i not in singing and i not in humming]:
	fields.append(f'-f "Save|{nl.join(f"{i} - {r}" for i,r in infected_saves.items() if i in saved)}"')

# return f'-desc "{singing}\n{humming}\n{saved}"'  #  {singing} {humming} {saved}'

influence_score=len(singing)+len(humming)//2
if singing and influence_score>=influence_threshold:
	influence=[
		"Lamps flicker.",
		"Random books fly off the shelves.",
		"The library becomes much colder.",
		"Three shadows appear in the same area as the characters.",
		"Nightmare for lowest roll.",
		"Strife."
	]
	influence_roll=vroll('1d6')
	fields.append(f'-f "Influence [{influence_score}/{influence_threshold}]|{influence_roll}\n||{influence[influence_roll.total-1]}|| (see book for details)"')
return " ".join(fields)
</drac2>