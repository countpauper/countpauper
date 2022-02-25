<drac2>
#cmd is an integer: target is actor for shenanigans(), cowardice(50), danger(50) multiplied by the level. Store as custom, value*level gpin metadata
#!bonus [<player>] <key> if key is any of those (listed in an svar) predefs the svar has the gp/level Store as key, value*level gp in metadatata
#!bonus [<player>] kill <target> [...] store targets in metadata with [name:(hp,cr), ...] , to compute later for: streak, combo, mass, overkill
#!bonus [<player>] ohko <target> [....] same as kill but add bonus for ohko
#!bonus [<player>] crit <target> [....] same as kill but crit IF DEAD else just store as crit value
#!bonus ]<player>] perform[ance] <score> store as performance:gp with value from svar performance tresholds
#!bonus [<npc>] ko <target> store as really dead


# TODO: add a way to add a kill in a round with a CR if missed, it's a pain to add it by hand in the uvar/metadata.
#   Sub command would have a weird name and still have to do the undo thing
#   - CR<x> becomes a general CR creature added to kills with duck typing. May need to flatten SimpleCombatants into ducks as well
#   - : all -options val are allowed and argparsed. Only before first -string option is parsed as bonus data
#   - round <x> overrides the current combat round to store
#   - (and later -hp -max and maybe even -cr then too? )
# TODO: [done] store backup in single bonus_backup uvar per channel.
#   - [done] reward without combat still finds the reward of the current channel in the back
#   - [done] reward without channel shows the first backup
#   - [done] reward with channel id argument shows that channel's backup regardless of combat or channel
#  	- [done] Separate restore command to restore
#   - [done] undo goes back to combat metadata
# TODO: Allow not just decimals but all formula operators as well for flat bonuses, store that string, roll it. (+-*/.())
#       make sure this - doesn't trigger the -argument. First character after the - must be there and it must be alphabetical for options -3 is flat -d is options just - is nothing

C=combat()
if not C:
	return f'echo Can only track the bonus in combat'

# retrieve the current bonus data
mdb_key = 'bonus'
mdb = load_json(C.get_metadata(mdb_key,'[]'))
mdk_key = 'kills'
mdk = load_json(C.get_metadata(mdk_key,'[]'))

# remember this data for potential undo update
original_data = dump_json(dict(kills=mdk, bonus=mdb))

args=&ARGS&
# boni is {"cmd":{"reward":"formula","kill":bool}} or just forumula string instead of a dict or just a boolean to track ?
# formula will be parsed and can contain variables:
#       cr = cr of target (for each)
#       lvl = player level of all targets (same as)
#       hp = hp of target
#       max = max hp of target
#       all current (u)vars with get (cr and lvl) will be set ?

# Load and clean up commands
sv='bonus'
config = load_json(get_svar(sv,'{}'))
commands = dict()
for cmd,b in config.get('commands',{}).items():
	if typeof(b)=='str':
		commands[cmd]=dict(reward=b)
	elif typeof(b)=='bool':
		commands[cmd]=dict(kill=b)
	else:
		commands[cmd]=b

# early check all arguments to prevent missed registration due to typos
if unknown_args:=[a for a in args if not (C.get_combatant(a) or a in commands or a.isdecimal())]:
	return f'techo 5 "Unrecognized arguments `{",".join(unknown_args)}`'

# gather all player and monster from all arguments
players=[t for a in args if (t:=C.get_combatant(a))  and t.levels and not t.levels.get('Monster')]
monsters=[t for a in args if (t:=C.get_combatant(a)) and t.levels.get('Monster')]
kills=[t for t in monsters if t.hp<=0]

# create a dictionary of usable variables from the combat state
variables=dict(
	lvl=sum(t.levels.total_level for t in players) if players else None,
	cr=sum(t.levels.get('Monster') for t in monsters) if monsters else None,
	kills=len(kills) if kills else None,
	hp=sum(t.hp for t in monsters)  if monsters else None,
	max=sum(t.max_hp for t in monsters) if monsters else None,
	dead=(1 if any(t.hp<=0 for t in monsters) else 0) if monsters else None,
	died=(1 if any(t.hp==0 for t in players) else 0) if players else None
)

# prepare for adding bonuses
kill=False
total_bonus = 0
player_names=[p.name for p in players]

# Add all number arguments as flat bonuses
for a in args:
	if a.isdecimal():
		b = int(a)
		total_bonus+=b
		mdb.append(dict(p=player_names, c='flat',f=a, b=b, r=C.round_num))

# loop over all configured commands present in the arguments and apply their formula
for cmd,b in commands.items():
	# could do case insensitive or partial matching here, but can also just alias in the config
	if not cmd in args:
		continue
	kill = kill or b.get('kill',False)
	if formula:=b.get('reward'):
		for var, val in variables.items():
			if val is None:
				if var in formula:
					break	 # unavailable varliable, formula invalid, skip
				else:
					continue # unavailable but unneeded
			formula=formula.replace(var,str(val))
		else:	# all variables replaced, valid formula, apply the bonus
			if bonus_value := roll(formula):
				mdb.append(dict(p=player_names, c=cmd, f=formula, b=bonus_value,r=C.round_num))
				total_bonus+=bonus_value
if kill and kills:
	mdk.append(dict(k={p.name:p.levels.total_level for p in players},
					t={k.name:k.levels.get('Monster') for k in kills},
					r=C.round_num))

# debug only on the debug server
debug = ctx.guild.id == 751060661290795069

if kills or total_bonus:
	C.set_metadata(mdb_key, dump_json(mdb))
	C.set_metadata(mdk_key, dump_json(mdk))
	C.set_metadata('bonus_undo', original_data)
	# TODO: store bonus undo per channel id and fix undo

# store the state as backup per channel
backup_var='bonus_backup'
backup=load_json(get(backup_var,"{}"))
backup[str(ctx.channel.id)]=dict(kills=mdk, bonus=mdb)
set_uvar(backup_var, dump_json(backup))

if debug:
	return f'echo Bonus `{total_bonus}` kills `{", ".join(k.name for k in kills) or "none"}`'
elif kills:
	if total_bonus:
		return f'techo 3 Bonus and kill registered.'
	else:
		return f'techo 3 Kill registered.'
elif total_bonus:
	return f'techo 3 Bonus registered.'
else:
	return f'techo 3 Nothing registered.'
</drac2>