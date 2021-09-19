<drac2>
cvar='Play'
game_name='drink'
state=load_json(get(cvar,'{}'))
game=state.get(game_name,{})
syntax=f'{ctx.prefix}{ctx.alias} {game_name} [reset]|[-b <bonus>] [adv] [dis] [-mi <minimum>]'

# initialize game state
drinks=game.get('drinks',0)
args=&ARGS&
if 'reset' in args:
	state[game_name]={}
	character().set_cvar(cvar, dump_json(state))
	return f'''embed -title "{name} sleeps it off." -desc "{get("their","their").capitalize()} intoxication level is back to normal."'''

drinks+=1
dc=9+drinks
ch=character()

# parse arguments for roll string
args=argparse(args)
rolls=[ch.saves.get('con').d20(args.adv(boolwise=True),reroll=ch.csettings.get('reroll'), min_val=int(args.last('mi',0)))]+args.get('b')
save=vroll("+".join(rolls))

# increase intoxication on failed save
intoxication=game.get('intoxication',0)
if save.total<dc:
	intoxication+=1

# disply result
flavour=[
	["You're fine.","You feel good.","No problem."],
	["You are tipsy.","You're woozy.","You feel merry.","You're buzzed."],
	["You're intoxi. ... intoximicated.","You're drunk.","Yes, you're definitely soused.","You may have had one too many, or two."],
	["You're blind drunk.","You're smashed.","Wasted!","You're sloshed.","Drunk as a lord.","You're hammered."],
	["You're out!","You're black out drunk.","You slide under the table.","You drank yourself into a stupor.","You pass out.","Zzzzz"]
]

result=flavour[intoxication]
result=result[randint(len(result))]
done=intoxication>=len(flavour)-1
if done:
	result=f'**{result}**'
elif intoxication>=2:
	result=f'*{result}*'

result=f'*{"Success!" if save.total>=dc else "Failed."}*\n{result} (`{intoxication+1}`)'

# persist game state
if done:
	state[game_name]={}
else:
	state[game_name]={'drinks':drinks,'intoxication':intoxication}
character().set_cvar(cvar,dump_json(state))

return f'embed -title "{name} drinks #{drinks}" -f "{save} VS DC {dc}|{result}" -footer "{syntax}" -color {color} -thumb https://cdn.discordapp.com/attachments/751098635038621868/825287768040931349/Chasing-the-green-fairy-absinthe-fact-and-fiction-730x410.png'
</drac2>