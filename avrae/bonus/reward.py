<drac2>
C=combat()
if not C:
	return f'echo The bonus can only be rewarded in combat'

mdb_key = 'bonus'
mdb = load_json(C.get_metadata(mdb_key,'[]'))
mdk_key = 'kills'
mdk = load_json(C.get_metadata(mdk_key,'[]'))
args=&ARGS&
sv='bonus'
config = load_json(get_svar(sv,'{}'))

# prepare by creating a list of all killers
killers={_ for _ in []}
for kill in mdk:
	killers.update({killer for killer in kill.k.keys()})

# add mass bonuses from the raw kill list
if mass:=config.get('mass',[]):
	for k in mdk:
		if b:=mass[min(len(k.t),len(mass))-1]:	# !bonus shouldn't put in empty kill lists
			cr=k.t.values()
			mdb.append(dict(c='mass', p=k.k, r=k.r, f=f'{b}*({"+".join(str(ccr) for ccr in cr)})', b=b*sum(cr)))

# Make a kill board, summing up the CR of all mass kills already
# it will be dict(killer.name=dict(round=[cr,cr,..],...),...)
# the crs of mass kills are already summed, to distinguish combos
kill_board=dict()
for killer in killers:
	kill_count=dict()
	for kill in mdk:
		if killer in kill.k:
			kill_count[kill.r]=kill_count.get(kill.r,[])+[sum(kill.t.values())]
	kill_board[killer]=kill_count

# add bonuses for all combos
combo = config.get('combo',[])
for killer, board in kill_board.items():
	for rnd, round_kills in board.items():
		if combo and (combonus:=combo[min(len(combo), len(round_kills))-1]):
			mdb.append(dict(c='combo', p=killer, r=rnd, f=f'{combonus}*({"+".join(str(cr) for cr in round_kills)})', b=combonus*sum(round_kills)))

# collect all streaks [(killer,round,[cr,...])]
kill_streaks=[]
for killer, board in kill_board.items():
	current_streak=None
	for rnd, round_kills in board.items():
		if current_streak and current_streak[1]+len(current_streak[2])==rnd:
			current_streak[2].append(sum(round_kills))
			continue
		kill_streaks.append((killer,rnd, [sum(round_kills)]))
		current_streak=kill_streaks[-1]

# add bonuses for all streaks
if streak:=config.get('streak',[]):
	for killer,rnd,killstreak in kill_streaks:
		if streak_bonus:=streak[min(len(streak), len(killstreak))-1]:
			mdb.append(dict(c='streak',r=rnd, p=killer, f=f'{streak_bonus}*({"+".join(str(cr) for cr in killstreak)})', b=streak_bonus*sum(killstreak)))


total_reward=sum(b.b for b in mdb)
if 'verbose' in args or 'v' in args:
	# TODO: could group per command
	verbonus = [f'''{b.get('c','???')} @ {b.get('r','?')} : {b.get('b')}''' for b in mdb]
	nl='\n'
	return f'embed -title "Bonus Reward" -desc "{nl.join(verbonus)}" -f "Total|`{total_reward}`"'
else:
	return f'echo Total bonus: `{total_reward}`'
</drac2>
