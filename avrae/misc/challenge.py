<drac2>
args=&ARGS&
if not args:
	return f'help {ctx.alias} -here'
# Split the arguments in challengers and options
first_option=min([i for i, a in enumerate(args) if a.startswith('-')] + [len(args)])
challenger_arg=args[:first_option]
args=argparse(args[first_option:])

# parse the challengers to a list of CRs
challengers=[]
for ca in challenger_arg:
	if 'x' in ca:
		amount,cr=ca.split('x',2)
		challengers+=[cr] * int(amount)
	else:
		challengers.append(ca)
CR=[float(cr) for cr in challengers if cr.isdigit() or '.' in cr]
CR+=[roll("1000*"+cr)/1000 for cr in challengers if '/' in cr]

# parse the players to a list of levels
timeout = None
players = None
if C:=combat():	# from current initiative, add a timeout
	monsters=[c for c in C.combatants if c.controller==ctx.author.id]
	CR += [cr for c in monsters if bool(tuple(c.levels)) and (cr:=(c.levels.get('Monster',default=-1)))>=0]
	players=[c for c in C.combatants if c.controller!=ctx.author.id and c.levels.get('Monster')==0]
	players=[p.levels.total_level for p in players]
	if players:
		timeout = 10
# if none found in combat, get the default from a variable
if not players:
	var_name = 'challenge_players'
	players = [int(p) for p in load_yaml(get_svar(var_name, get(var_name, '[]')))]
# get the override from the optional arguments -p
if player_options:=args.get('p'):
	players=[int(p) for parg in player_options for p in parg.split(',')]

# compute the adjusted XP table
XPtable={0:10, 0.125:25, 0.25:50, 0.5:100, 1:200, 2:450, 3:700, 4:1100, 5:1800, 6:2300, 7:2900, 8:3900, 9:5000, 10:5900, 11:7200, 12:8400, 13:10000, 14:11500, 15:13000, 16:15000 }
xp=sum(XPtable.get(cr,0) for cr in CR)
adjusted_table={0: 1, 1:1,
			 2: 1.5,
			 3: 2, 4: 2, 5: 2, 6: 2,
			 7: 2.5, 8: 2.5, 9: 2.5, 10: 2.5,
			 11: 3, 12: 3, 13: 3, 14: 3,
			 15: 4, 16: 4, 17: 4, 18: 4, 19: 4, 20: 4}
mul=adjusted_table.get(len(CR), 1)
adjusted_xp = round(mul*xp)

# compute the party's threshold
threshold_table = [
	(10, 20, 30, 50), # HB level 0 dummy
	(25, 50, 75, 100),
	(50, 100, 150, 200),
	(75, 150, 225, 400),
	(125, 250, 375, 500),
	(250, 500, 750, 1100),
	(300, 600, 900, 1400),
	(350, 750, 1100, 1700),
	(450, 900, 1400, 2100),
	(550, 1100, 1600, 2400),
	(600, 1200, 1900, 2800),
	(800, 1600, 2400, 3600),
	(1000, 2000, 3000, 4500),
	(1100, 2200, 3400, 5100),
	(1250, 2500, 3800, 5700),
	(1400, 2800, 4300, 6400),
	(1600, 3200, 4800, 7200),
	(2000, 3900, 5900, 8800),
	(2100, 4200, 6300, 9500),
	(2400, 4900, 7300, 10900),
	(2800, 5700, 8500, 12700)]
# HB extend threshold range
threshold_labels=["trivial","easy","medium","hard","deadly","more than deadly","deadly and a half","almost double deadly", "double deadly", "more than double deadly", "double deadly and a half", "almost triple deadly", "triple deadly"]
threshold_table=[(0, e, m, h, d, d+e, d+m, d+h, 2*d, 2*d+e, 2*d+m, 2*d+h, 3*d) for (e, m, h, d) in threshold_table]

thresholds={lbl:sum(threshold_table[lvl][i] for lvl in players) for i,lbl in enumerate(threshold_labels)}
challenge_rating=(["Impossible"]+[lbl for lbl,t in thresholds.items() if adjusted_xp>=t])[-1]

# construct the output
table, challenge='',''

# Optionally The table
if args.get('table'):
	table= '\n```        Challenge Rating | XP Threshold\n'+'-'*40 + '\n'
	table+= '\n'.join(f'{r.capitalize():>25}| {xp}' for r,xp in thresholds.items())
	table+= '```'

def format_cr(cr):
	if cr>=1:
		return str(int(cr))
	return ['0', '1/8', '1/4', '3/8', '1/2', '5/8', '3/4', '7/8'][int(cr*8)]

if CR:
	monsters = ', '.join(format_cr(f) for f in CR)
	player_lvls = ', '.join(str(p) for p in players)
	challenge = f'`{len(CR)}` Monsters, CR: `{monsters}` === VS === `{len(players)}` Players, level: `{player_lvls}`'
	challenge += f'\n{xp} XP x {mul*100}% = {adjusted_xp} adjusted XP: **{challenge_rating}**'

if timeout and challenge:
	cmd=f'techo {timeout}'
else:
	cmd='echo'

if challenge or table:
	return f'{cmd} {challenge}{table}'
else:
	return f'echo Provide at least one CR to compute the challenge.'
</drac2>