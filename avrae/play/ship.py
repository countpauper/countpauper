<drac2>
cvar='Play'
game_name='ship'
state=load_json(get(cvar,'{}'))
game=state.get(game_name,{})
args="&*&"
turns=3
syntax=f'{ctx.prefix}{ctx.alias} {game_name}'

turn=game.get('turn',0)
bank=game.get('bank',[])
if turn>=turns:
	turn=0
	bank=[]

# roll
rolls=[roll('1d6') for _ in range(5-len(bank))]
target=[6,5,4]
for t in target:
	if t not in bank:
		if t in rolls:
			rolls.remove(t)
			bank.append(t)
		else:
			break

# persist
if done:= all(t in bank for t in target):
	state[game_name]={}
else:
	turn+=1
	state[game_name]={'turn':turn, 'bank':bank}
character().set_cvar(cvar,dump_json(state))

# display game
ship=f'**{", ".join(str(r) for r in bank)}**, ' if bank else ''
cargo=", ".join(str(r) for r in rolls)
# output
if done:
	desc = f':sailboat: {ship} {cargo} = {sum(rolls)}'
else:
	desc= f'**Round:** {turn}/{turns} **Dice:** {ship} {cargo}'
	if turn==turns:
		desc+=' = 0'
return f'embed -title "Ship, captain and crew" -desc "{desc}" -thumb https://upload.wikimedia.org/wikipedia/commons/9/99/Dice_-_1-2-4-5-6.jpg -footer "{syntax}"'
</drac2>
