tembed <drac2>
# retrieve data
gv = get_svar("training", "2605a178-bff2-4553-8548-6778ce0ba8e2")
data = {k.lower():v for (k,v) in load_json(get_gvar(gv)).items()}

# parse arguments into amount,options
arg = "&*&"
amount = 1
option=arg.lower()
split_arg=arg.split(maxsplit=1)
if len(split_arg)>1 and split_arg[0].isdigit():
	amount=int(split_arg[0])
	option=split_arg[1].lower()
if option=='level':
	option=f'level[{level+1}]'
training=data.get(option)

# verify arguments
if arg == '?' or arg.lower() == 'help' or arg == '' or not training:
	options=['level'] + [t for t in data.keys() if not t.startswith('level')]
	return f'-title "{name} doesn\'t know how to traing." -desc "`!train [<time spent>] <training>` where training is one of {", ".join(options)}."'

# check preconditions
ccn='Downtime'
if not cc_exists(ccn):
	return f'-title "{name} has no time to train" -desc "You need to have a Downtime counter to spend.\nYou can create it with `!cc create Downtime -max 4 -reset long`"'
downtime=get_cc(ccn)
if not downtime:
	return f'-title "{name} doesn\'t have time to train" -desc "You have no more downtime left." -f Downtime|"{cc_str(ccn)}"|inline'
amount = min(amount, downtime)
cvn='Training'
training_progress = load_json(get(cvn, '{}'))
progress =training_progress.get(option,0)

effort=training.get('effort')
amount = min(amount,effort-progress)
cost=training.get('cost',0)*amount/effort
# TODO: check cost precondition

# Progress reporting
desc=''
fields=''
progress+=amount
done = progress>=effort
cc = [c for c in character().consumables if c.name==ccn][0]
if done:
	fields+='-f Progress|Done|inline '
elif cc is not None and cc.reset_on=='long':
	fields+=f'-f Remaining|"{(effort-progress)//cc.max} days"|inline '
else:
	fields+=f'-f Progress|"{100*progress//effort}%"|inline '

grats=':tada: *Congratulations* :tada:'
# goal determination and completion
if target_level:=training.get('level'):
	title = f'{name} spends some downtime training for level {target_level}'
	if done:
		urls={
			"beyond": "https://ddb.ac/characters/",
			"dicecloud": "https://dicecloud.com/character/",
			"google": "https://docs.google.com/spreadsheets/d/"
		}
		ups = character().upstream.split('-',2)
		url=urls.get(ups[0],ups[0])+ups[1]
		desc=f'{grats}\nYou can now:'
		desc+=f'\n■ Update your sheet at {url}\n■ Import in Avrae using `!update`'
		if exists('subclass'):	# depends on !level produced cvar json {"<Class>Level":"SubClass"}
			level_options=[f'`!level {int(get(clslvl,0))+1} {clslvl[:-5]} {subcls}`' for (clslvl,subcls) in load_json(subclass).items()]
			desc+=f'\n■ update counters with e.g. {", ".join(level_options)}'
elif new_language:=training.get('language'):
	title = f'{name} spends some downtime studying {new_language}'
	if done:
		desc=training.get('diploma',f'{grats}. You can now proficiently speak {new_language}.')
		langcv='languages'
		langlist = [l for l in get(langcv,'').split(',') if l] +[new_language]
		set_cvar(langcv, ', '.join(langlist))
elif new_tool:=training.get('proficiency'):
	title = f'{name} spends some downtime practicing with {new_tool}'
	if done:
		desc=training.get('diploma',f'{grats}. You can now proficiently use {new_tool}.')
		toolcv='pTools'
		toollist = [t for t in get(toolcv,'').split(',') if t]+[new_tool]
		set_cvar(toolcv, ', '.join(toollist))
else:
	err(f"No goal defined for training {option}. Please fix it in `!gvar {gv}`")

# Apply costs
mod_cc(ccn,-amount,True)
fields+=f'-f "Downtime [{-amount}]"|"{cc_str(ccn)}"|inline '
training_progress[option] = progress
if done:
	training_progress.pop(option)
set_cvar(cvn, dump_json(training_progress))

return f'-title "{title}" -desc "{desc}" '+fields
</drac2>