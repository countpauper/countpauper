embed <drac2>
# TODO:

# load all gvars to data
sv = load_json(get_svar('downtime','{}'))
gv= sv.get('training','2605a178-bff2-4553-8548-6778ce0ba8e2')
if typeof(gv)=='str':
	gv=gv.split(',')
data={}
for tgv in gv:
	data.update({k.lower():v for (k,v) in load_json(get_gvar(tgv.strip())).items()})

# parse arguments into amount,options
arg = "&*&"
amount = 1
option=arg.lower()
split_arg=arg.split(maxsplit=1)
if len(split_arg)>1 and split_arg[0].isdigit():
	amount=int(split_arg[0])
	option=split_arg[1].lower()
if 'level'.startswith(option):
	new_level = level+1
	option=f'level[{new_level}]'
	training=data.get(option)
	if not training:
		return f'-title "{name} doesn\'t feel like training for level {new_level}." -desc "There is no training requirement for that level."'
else:
	matches=[k for k in data.keys() if k.startswith(option)]
	if matches:
		option=matches[0]
		training=data.get(option)
	else:
		training=None

# verify arguments
if arg == '?' or arg.lower() == 'help' or arg == '' or not training:
	options=['level'] + [t for t in data.keys() if not t.startswith('level')]
	return f'-title "{name} doesn\'t know how to train." -desc "`!train [<time spent>] <training>` where training is one of {", ".join(options)}."'

# recreate downtime consumable with current config, keeping value
char=character()
cc_max = sv.get('max',1)
cc_max = int(cc_max) if str(cc_max).isdigit() else None
cc_reset=sv.get('reset','long').lower()
ccn='Downtime'
if char.cc_exists(ccn):
	cc_val=char.get_cc(ccn)
	char.delete_cc(ccn)
else:
	cc_val=None
if cc_max>0:
	char.create_cc(ccn, maxVal=cc_max, reset=cc_reset)
	if cc_val is not None:
		char.set_cc(ccn,cc_val)

# check preconditions
if amount>0:
	downtime=get_cc(ccn)
	if not downtime:
		return f'-title "{name} doesn\'t have time to train" -desc "You have no more downtime left." -f Downtime|"{cc_str(ccn)}"|inline'
	amount = min(amount, downtime)

# effort is in days if downtime reset every day. else in downtime points, minimum is 1
effort=training.get('effort')
if effort:
	if cc_max and cc_reset=='long':
		effort*=cc_max
else:
	effort=1
effort = max(1, int(effort))

goal=option
if target_level:=training.get('level'):
	goal=f'level {target_level}'
elif new_language:=training.get('language'):
	goal=new_language
	if new_language in get('languages',''):
		return f'-title "{name} already knows how to speak {goal}" -desc "You are already proficient with that language."'
elif new_prof:=training.get('proficiency'):
	goal=new_prof
	if new_prof in get('pTools','') or new_prof in get('eTools',''):
		return f'-title "{name} already knows how to use {goal}" -desc "You are already proficient with that tool."'
else:
	err(f"No goal defined for training {option}. Please fix it in `!gvar {gv}`")

desc=''
fields=''
game_data=load_json(get_gvar('c2bd6046-90aa-4a2e-844e-ee736ccbc4ab'))

# get the current progress
cvn = 'Training'
training_progress = load_json(get(cvn, '{}'))
progress = training_progress.get(option)
# initial progress set by bonus
if not progress:
	if talents:=training.get('talent',None):
		id_str=talents.replace('-','|').replace('+','|').replace('*','|').replace('/','|')
		start_vars=[id for id in id_str.split("|") if id.isidentifier()]
		for start_var in start_vars:
			if start_var in game_data['cvar']:
				talents = talents.replace(start_var,f'{get(start_var,0)}[{game_data["cvar"][start_var]}]')
		for (skill_name, skill) in char.skills:
			if skill_name in start_var and not exists(skill_name):
				talents = talents.replace(skill_name,f'{int(skill.prof*proficiencyBonus)}[{game_data["skills"][skill_name]}]')
		progress_roll = vroll(talents)
		fields+=f'-f Talent|"{progress_roll}"|inline '
		progress = vroll(talents).total
		if cc_max and cc_reset=='long':
			progress*=cc_max
		progress=min(progress,effort-1)
	else:
		progress=0

# compute the cost and clip the amount to the maximum needed to complete
amount = min(amount,max(0,effort-progress))
cost=training.get('cost',0)*amount/effort

# check the cost if bags are set up. coins will be taken at the same time, but bag is only updated on succeess
# get server rates or default phb, but invert from normal definition in price per coin and sort by value
rates = load_json(get_svar('coins', '{}')).get('rates',{"cp":100,"sp":10,"ep":2,"gp":1,"pp":0.1})
inv_rates = {(1/r):c for (c,r) in rates.items() }
if cost>0:
	# make a list of coins needed for if there are no bags or not enoug coins
	total_needed = []
	cost_remaining=cost
	rev_coin_values = list(inv_rates.keys())
	rev_coin_values.reverse()
	for coin_value in rev_coin_values:
		coins_needed = int((cost_remaining+(0.1*coin_value)) // coin_value)
		if coins_needed:
			total_needed += [f'{coins_needed} {inv_rates[coin_value]}']
			cost_remaining -= coins_needed * coin_value
	bag_var='bags'
	if exists(bag_var):
		baglist=load_json(get(bag_var))
		total_taken=[]
		spare_coins = []
		for coin_value in rev_coin_values:
			coin_name=inv_rates[coin_value]
			coins_needed=int((cost+(0.1*coin_value))//coin_value)	# add 0.1*coin for numeric imprecision ie 0.999 cp
			for b in baglist:
				if b[0].lower()=='coin pouch':
					# coins in this bag
					bag_coins=b[1].get(coin_name,0)
					# convert spare coins if needed
					for spare_idx in range(0,len(spare_coins)):
						if coins_needed>bag_coins:
							spare_bag,spare_coin=spare_coins[spare_idx]
							spare_amount=spare_bag[spare_coin]
							conversion_rate=rates[coin_name]/rates[spare_coin]
							conversion_needed=int(ceil((coins_needed-bag_coins)/conversion_rate))
							converted_coins=min(conversion_needed, spare_amount)
							bag_coins+=int(converted_coins*conversion_rate)
							spare_bag[spare_coin] = spare_amount-converted_coins
					if coins_taken:=min(coins_needed, bag_coins):
						cost-=coins_taken*coin_value
						b[1][coin_name]=bag_coins-coins_taken
						total_taken+= [f'{coins_taken} {coin_name}']
						coins_needed-=coins_taken
					elif bag_coins:
						spare_coins=[(b[1],coin_name)]+spare_coins
		treshold=list(inv_rates.keys())[0]
		if cost>treshold:
			if not total_taken:
				total_taken=['nothing']
			return f'-title "{name} can\'t afford to train {goal} today." -desc "You need {" ".join(total_needed)}, but have {" ".join(total_taken)} in your bags." '
		else:
			fields+=f'-f Cost|"{" ".join(total_taken)}"|inline '
			set_cvar(bag_var,dump_json(baglist))
	else:
		fields += f'-f Cost|"{" ".join(total_needed)}"|inline '

# Progress reporting
progress+=amount
done = progress>=effort

if done:
	fields+='-f Progress|Done|inline '
elif cc_max and cc_reset=='long':
	fields+=f'-f Remaining|"{(effort-progress)//cc_max} days"|inline '
else:
	fields+=f'-f Progress|"{100*progress//effort}%"|inline '

grats=':tada: *Congratulations* :tada:'
# goal determination and completion
spends='spends some downtime' if amount else 'checks their progress in'
if target_level:
	title = f'{name} {spends} training for {goal}'
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
elif new_language:
	title = f'{name} {spends} studying {new_language}'
	if done:
		desc=training.get('diploma',f'{grats}. You can now proficiently speak {new_language}.')
		langcv='languages'
		langlist = [l for l in get(langcv,'').split(',') if l] +[new_language]
		set_cvar(langcv, ', '.join(langlist))
elif new_prof:
	title = f'{name} {spends} practicing with {new_prof}'
	if done:
		desc=training.get('diploma',f'{grats}. You can now proficiently use {new_prof}.')
		toolcv='pTools'
		toollist = [t for t in get(toolcv,'').split(',') if t]+[new_prof]
		set_cvar(toolcv, ', '.join(toollist))

# Apply costs
if amount>0:
	char.mod_cc(ccn,-amount,True)
	fields+=f'-f "Downtime [{-amount}]"|"{cc_str(ccn)}"|inline '

if img_url:=training.get('img'):
	fields+=f'-thumb "{img_url}" '

training_progress[option] = progress
if done:
	training_progress.pop(option)
set_cvar(cvn, dump_json(training_progress))

return f'-title "{title}" -desc "{desc}" '+fields
</drac2>