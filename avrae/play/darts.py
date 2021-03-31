<drac2>
cvar='Play'
game_name='darts'
state=load_json(get(cvar,'{}'))
game=state.get(game_name,{'turn':[],'score':501,'in':True})	# todo start options -double in,out -straight in,out 301/501/701
base_cmd=f'{ctx.prefix}{ctx.alias}'

syntax=f'{base_cmd} {game_name} <target #> [2|3] [adv][dis] [-b <bonus>]'

board=[20,1,18,4,13,6,10,15,2,17,3,19,7,16,8,11,14,9,12,5]
# inner bullseye size 0.2"^2 [=1], outer=x3 outer segment =x18 inner=x12,tripple = 2x, double = 3x
rings=[
	dict(name='bullseye', mul=1, ac=18,dbl=True),
	dict(name='bullseye', mul=1,ac=16,dbl=False),
	dict(name='',mul=1,ac=10,dbl=False),
	dict(name='tripple',mul=3,ac=15,dbl=False),
	dict(name='',mul=1,ac=8,dbl=False),
	dict(name='double',mul=2,ac=14,dbl=True),
	dict(name='out',mul=0,ac=0,dbl=False)]

args=&ARGS&
if not args or not args[0].isdigit():
	# todo: default if not in: double (arg 0 or 20) else if score>60: tripple 20
	# if score <40  and even double (number=score/2)
	# else if uneven: single 1
	target_nr=20
else:
	target_nr=int(args[0])

if target_nr==50:
	target=dict(nr=50,ring=0)
elif target_nr==25:
	target=dict(nr=25,ring=1)
elif len(args)>1 and args[1].isdigit():
	target=dict(nr=target_nr,ring=[0,4,5,3][int(args[1])])
else:
	target=dict(nr=target_nr,ring=4)

args=argparse(args)
if reroll:=character().csettings.get('reroll'):
	die=f'd20ro{reroll}'
else:
	die='d20'
die=[f'1{die}',f'2{die}kh1',f'3{die}kh1',f'2{die}kl1'][args.adv(ea=True)]
dice='+'.join([die,str(proficiencyBonus),str(dexterityMod)]+args.get('b'))
r=vroll(dice)
ac=rings[target.ring].ac
off = ac - r.total
if r.result.crit==2:	# critical miss
	off=max(off,1+randint(5))

if off<=0 or r.result.crit==1:
	hit=target
else:	# miss

	# ring	1			>1				>4				>10
	# 0 	1 25		random #		(off random)	off the board
	# 1 	1 50 		random #		(off random)	off the board
	# x3	x3 neighbour same number	neighbour (off-3) (even/odd)/2	off the board
	# x2	x2 neighbour same number	Off the board	off the board
	# x1	x3			neighbour off/3					off the board

	# hit ring for target ring,off
	hit_rings=[
		[0,1,2,2,2,3,4,4,4,5,6],	# 50
		[1,0,2,2,2,3,4,4,4,5,6], 	# 25
		[2,1,2,2,3,4,4,4,4,5,6], 	# inner
		[3,3,2,2,2,4,4,4,4,5,6], 	# x3
		[4,4,4,4,4,3,2,2,2,5,6], 	# outer
		[5,5,4,4,4,6,6,6,6,6,6], 	# x2
		[6,5,4,6,6,6,6,6,6,6,6] 	# out
	][target.ring]
	if off>=len(hit_rings):
		hit_ring=6
	else:
		hit_ring=hit_rings[off]

	if hit_ring==0:
		hit=dict(nr=50,ring=hit_ring)
	elif hit_ring==1:
		hit=dict(nr=25,ring=hit_ring)
	elif hit_ring==6:
		hit=dict(nr=target.nr,ring=hit_ring)
	else:
		if target.nr>=25:	# random board number if missed bullseye
			missed_number=board[randint(len(board))]
		else:
			missed_number=target.nr
		divergence = [
			[0, 0, 1, 1, 0, 1, 2, 2, 2, 0, 0],	# 50
			[0, 0, 1, 1, 0, 1, 2, 2, 2, 0, 0],	# 25
			[0, 0, 1, 1, 0, 1, 2, 2, 2, 0, 0],  # inner
			[0, 1, 0, 0, 1, 0, 1, 2, 2, 0, 0],  # x3
			[0, 1, 1, 1, 2, 0, 2, 2, 3, 0, 0],  # outer
			[0, 1, 0, 0, 1, 0, 1, 1, 2, 2, 0],  # x2
			[0, 0, 0, 0, 1, 1, 1, 2, 2, 3, 3]
		][target.ring][off]
		if randint(2):	# random direction
			divergence=-divergence
		t=board.index(missed_number)+divergence
		if t>=len(board):
			t-=len(board)
		hit=dict(nr=board[t],ring=hit_ring)

score=hit.nr * rings[hit.ring].mul
if not game['in']:	 # double in
	game['in']=rings[hit[1]].dbl

if not game['in']:	# not in, not scoring
	score=0

game['turn']=game.turn+[score]

win=False
turn_over=False
turn=game.turn
if sum(game.turn)==game.score and rings[hit.ring].dbl:
	win=True
	turn_over=True
elif sum(game.turn)+1>=game['score']:
	game['turn']=[]
	turn_over=True
elif sum(game_turn)+1
if len(game.turn)>=3:
	game['score']-=sum(game.turn)
	game['turn']=[]
	turn_over=True

# persist
if win:
	state.pop(game_name)
else:
	state[game_name]=game
character().set_cvar(cvar,dump_json(state))

return f'echo Aim at {rings[target.ring].name} {target.nr} {r} vs AC {ac}: off by {off}, hit {rings[hit.ring].name} {hit.nr} = `{score}` Turn: {turn} over: {turn_over} Score={game.score if game["in"] else "Not in"}'
</drac2>
