<drac2>
cvar='Play'
game_name='hoard'
state=load_json(get(cvar,'{}'))
game=state.get(game_name,{})
args="&*&"
base_cmd=f'{ctx.prefix}{ctx.alias}'
base_syntax=f'{base_cmd} {game_name}'
syntax=f'{base_syntax} [<dice>|<number eaten>]'

hoard=game.get('hoard')

if not args:
	if not hoard:
		args='3d6'
	else:	# reveal
		desc=f""":dragon: Hoard : {", ".join(f"*{d}*" if d>0 else f"~~{-d}~~" for d in hoard)} = **{sum(d for d in hoard if d>0)}**
		The player with the biggest hoard wins. Split the pot on ties."""
		hoard=None
if args:
	if args.isdigit():
		args=int(args)
		hoard=[-args if h==args else h for h in hoard]
		desc=f""":dragon: Hoard plundered [{args}] ||{", ".join(f"*{d}*" if d>0 else f"~~{-d}~~" for d in hoard)}||. **Only the {name}  can peek!**
		Raise your bet with `{base_cmd} raise <amount>` or drop out with `{base_cmd} drop`. Then reveal with `{base_syntax}`"""
	else:
		dice=args.split('d',maxsplit=1)
		if not dice[0]:
			dice[0]=1
		elif dice[0].isdigit():
			dice[0]=int(dice[0])
		else:
			return f'echo `{syntax}`'
		if len(dice)<2:
			return f'echo `{syntax}`'

		roll_str='1d'+dice[1]
		if dice[0]==1:
			v=vroll(roll_str)
			desc=f""":dragon: Hoard Plundered {v}
			All dragon players run `{base_syntax} {v.total}`"""
		else:	# roll for hoard
			hoard=[roll(roll_str) for _ in range(dice[0])]
			desc=f"""{name} :dragon: hoard. ||{", ".join(f"*{d}*" for d in hoard)}||. **Only {name} can peek!**
			Place your bet with `{base_cmd} bet <amount>`. 
			Then the dealer can plunder with `{base_syntax} d{dice[1]}`"""
# persist
if hoard:
	state[game_name]={'hoard':hoard}
else:
	state[game_name]={}
character().set_cvar(cvar,dump_json(state))

# output
return f'embed -title "Dragon Hoard" -footer "{syntax}" -desc "{desc}" -thumb https://cdn.discordapp.com/attachments/751098635038621868/825619306037248040/3041-DZ.png'
</drac2>
