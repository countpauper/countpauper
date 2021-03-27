<drac2>
cvar='Play'
game_name='drink'
state=load_json(get(cvar,'{}'))
game=state.get(game_name,{})
args=argparse("&*&")
syntax=f'{ctx.prefix}{ctx.alias} {game_name} [-b <bonus>] [adv] [dis] [-mi <minimum>]'

drinks=game.get('drinks',0)
drinks+=1
dc=9+drinks
ch=character()
rolls=[ch.saves.get('con').d20(args.adv(boolwise=True),reroll=ch.csettings.get('reroll'), min_val=int(args.last('mi',0)))]+args.get('b')
save=vroll("+".join(rolls))

intoxication=game.get('intoxication',0)
if save.total<dc:
	intoxication+=1

flavour=[
	["You're fine.","You feel good.","No problem."],
	["You're tipsy.","You're woozy.","You feel merry.","You're buzzed."],
	["You're intoxi. ... intoximicated.*","You're drunk.","Yes, you're definetly soused.","You may have had one too many, or two."],
	["You're blind drunk.","You're smashed.","Wasted!","You're sloshed.","You're drunk as a lord.","You're hammered."],
	["You're out!","You're black out drunk.","You're under the table.","You drank yourself into a stupor.","You pass out."]
]

result=flavour[intoxication]
result=result[randint(len(result))]
done=intoxication>=len(flavour)-1
if done:
	result=f'**{result}**'
elif intoxication>=2:
	result=f'*{result}*'

# persist
if done:
	state[game_name]={}
else:
	state[game_name]={'drinks':drinks,'intoxication':intoxication}
character().set_cvar(cvar,dump_json(state))

return f'embed -title "{name} drinks #{drinks}" -f "{save} VS DC {dc}|{result}" -footer "{syntax}" -thumb https://cdn.discordapp.com/attachments/751098635038621868/825287768040931349/Chasing-the-green-fairy-absinthe-fact-and-fiction-730x410.png'
</drac2>