<drac2>
cvar='Play'
game_name='darts'
base_cmd=f'{ctx.prefix}{ctx.alias}'
syntax=f'{base_cmd} {game_name} [<target #>[x2|x3]|yield] [adv] [dis] [ea] [-b <bonus>]'

### parse arguments
args=&ARGS&

target_arg = None
if args:
	first_arg=args[0].lower()
	if first_arg[0].isdecimal() or first_arg in 'outer bullseye':
		target_arg=first_arg

args=argparse(args)
game_type=args.last('game',type_=int)
double_in=('in' in args.get('double')) and not ('in' in args.get('straight'))  	# default False
double_out=('out' in args.get('double')) or not ('out' in args.get('straight'))	# default True

### get game state
state=load_json(get(cvar,'{}'))
game={'turn':[],'score':game_type or 501,'in':not double_in,'out':not double_out}
if not game_type:	# continue
	game=state.get(game_name,game)

### Board definition
board=[20,1,18,4,13,6,10,15,2,17,3,19,7,16,8,11,14,9,12,5]
# inner bullseye size 0.2"^2 [=1], outer=x3 outer segment =x18 inner=x12,tripple = 2x, double = 3x
rings=[
	dict(name='bullseye', mul=1, ac=18,dbl=True),
	dict(name='outer bullseye', mul=1,ac=16,dbl=False),
	dict(name='',mul=1,ac=10,dbl=False),
	dict(name='treble',mul=3,ac=15,dbl=False),
	dict(name='',mul=1,ac=8,dbl=False),
	dict(name='double',mul=2,ac=14,dbl=True),
	dict(name='out',mul=0,ac=0,dbl=False)]

### Yield
if args.last('yield'):
	if game_name in state:
		state.pop(game_name)
	turn_result='*You lose.* :disappointed: '
	turn=game.turn
	turn_desc=''
	title=f'{name} yields.'
	gameover=True
else:	### Turn
	# default target function, finds double out and double in
	remaining_score=game.score-sum(game.turn)
	if not game['in']:	# out double in
		default_ring=5
		default_nr=20
	elif remaining_score<=62:
		if game['out']:
			if remaining_score==50:
				default_nr=remaining_score
				default_ring=0
			elif remaining_score==25:
				default_nr=remaining_score
				default_ring=1
			elif remaining_score<=20:
				default_nr=remaining_score
				default_ring=4
			elif remaining_score % 3 == 0:
				default_ring=3
				default_nr=remaining_score / 3
			elif remaining_score % 2 == 0 and remaining_score<=40:
				default_ring=5
				default_nr=remaining_score / 2
			else:
				default_ring=4
				default_nr=20
		else:
			if remaining_score==50:	# double out on bullseye
				default_nr=remaining_score
				default_ring=0
			elif remaining_score>40:	# get to doubling out
				default_nr=remaining_score-40
				default_ring=4
			elif remaining_score % 2:	 # uneven, make even
				default_nr = 1
				default_ring = 4
			elif not game.out: 	# try to double out for doubles
				default_nr=remaining_score//2
				default_ring=5
			elif game.score>20:	# try for doubles
				default_nr=remaining_score//2
				default_ring=5
			else:
				default_nr=remaining_score
	else:	# maximize score
		default_ring=3
		default_nr=20

	# parse target argument with default
	if not target_arg:
		# todo: default if not in: double (arg 0 or 20) else if score>60: tripple 20
		# if score <40  and even double (number=score/2)
		# else if uneven: single 1
		target=dict(nr=default_nr,ring=default_ring)
	else:
		if 'out' in target_arg.lower():
			target_nr=25
		elif 'bull' in target_arg.lower() or 'eye' in target_arg.lower():
			target_nr=50
		else:
			target_args = target_arg.split('x', maxsplit=1)
			target_nr=int(target_args[0])
			if len(target_args)==1:	# allow for space between number and mul
				if args.last('x3',False):
					target_args+=[3]
				elif args.last('x2',False):
					target_args+=[2]
				elif args.last('x1',False):
					target_args+=[1]

		if target_nr not in board and target_nr!=50 and target_nr!=25:
			return f'echo You can only aim for 1-20,25 or 50. Use `{syntax}`'
		if target_nr == 50:
			target = dict(nr=50, ring=0)
		elif target_nr == 25:
			target = dict(nr=25, ring=1)
		elif len(target_args)==1:
			target=dict(nr=int(target_arg),ring=default_ring)
		else:
			matching_rings= [default_ring] + [idx for idx,r in enumerate(rings) if r.mul==int(target_args[1])]
			target=dict(nr=target_nr,ring=matching_rings[-1])

	aim=f'**{rings[target.ring].name} {target.nr if target.nr<=20 else ""}**'

	if reroll:=character().csettings.get('reroll'):
		die=f'd20ro{reroll}'
	else:
		die='d20'
	die=[f'1{die}',f'2{die}kh1',f'3{die}kh1',f'2{die}kl1'][args.adv(eadv=True)]
	dice='+'.join([die,str(proficiencyBonus),str(dexterityMod)]+args.get('b'))
	r=vroll(dice)
	ac=rings[target.ring].ac
	off = ac - r.total
	if r.result.crit==2:	# critical miss
		off=max(off,1+randint(5))

	if off<=0 or r.result.crit==1:
		hit=target
		hits = 'hits!'
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
		hits = f'misses and hits {rings[hit.ring].name} {hit.nr if hit.nr<=20 else ""}'
	score=hit.nr * rings[hit.ring].mul
	if not game['in']:	 # double in
		game['in']=rings[hit.ring].dbl

	if not game['in']:	# not in, not scoring
		score=0

	game['turn']=game.turn+[score]
	turn=game.turn
	gameover=False
	turn_result=f'Score={game.score-sum(turn)}'
	if sum(game.turn)==game.score and (game['out'] or rings[hit.ring].dbl):	 # win
		gameover=True
		turn_result='**You win!** :tada:'
	elif sum(game.turn)+1>=game['score']:	# bust
		game['turn']=[]
		turn_result='*You bust and your turn is over.* :confounded:'
	elif len(game.turn)>=3:			# end of turn
		game['score']-=sum(game.turn)
		game['turn']=[]
		turn_result='*Your turn is over.* :track_next:'
	title = f'{name} throws dart #{len(turn)}'
	turn_desc=f'**Ranged Attack:** {r} vs {aim} - AC `{ac}` {hits} (`{score}`)'
# persist
if gameover:
	if game_name in state:
		state.pop(game_name)
else:
	state[game_name]=game

character().set_cvar(cvar,dump_json(state))

dart_desc=[f':dart: `{turn[i] if i<len(turn) else "..."}`' for i in range(3)]
sep=' | '
game_desc=f'{sep.join(dart_desc)}\n{turn_result} ({game.score if game["in"] else "Not in."})'
image='' if gameover else '-image https://cdn.discordapp.com/attachments/751098635038621868/826779973271420948/Darts.png'
return f'embed -title "{title}" -desc "{turn_desc}\n{game_desc}" {image} -footer "{syntax}" -color {color}'
</drac2>
