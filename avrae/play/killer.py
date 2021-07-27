!alias bk <drac2>
cvar='Play'
game_name='killer'
all_state=load_json(get(cvar,'{}'))
if player:=all_state.get('player'):
	all_state[player]=(state:=all_state.get(player,{}))
else:
	player=name
	state=all_state

game=state.get(game_name,[])

killer=1
turns=len(game_name)
syntax=f'{ctx.prefix}{ctx.alias} {game_name}'
args=&ARGS&

if len(game)>=turns or any(d==killer for d in game):
	game=[]
rounds=1
all_arg='all'
if all_arg in args:
	args.remove(all_arg)
	rounds=turns-len(game)

rollstr=args[0] if args else '1d6'

# roll
for _ in range(rounds):
	game.append(roll(rollstr))
	if game[-1]==killer:
		break


# persist
state[game_name]=game
character().set_cvar(cvar,dump_json(all_state))

turn=len(game)
if any(d==killer for d in game):
	result='Killed!'
else:
	result=f'Total {sum(game)}'
syntax='!play killer [die=d6] [all]'

desc="`K I L L E R`\n"
desc+="`" +" ".join(f"{d}" for i,d in enumerate(game))+"`"
desc +=f'\n**{result}**'
return f'embed -title "{player.title()} rolls the {rollstr}" -desc "{desc}" -thumb https://cdn.discordapp.com/attachments/751098635038621868/867699902578032670/Kill_dice.png -footer "{syntax}" -color {color}'
</drac2>
