tembed <drac2>
#TODO
# required ingredients in bag
# required recipe {Item} recipe (not used)
# don't use recipe names, use item names, it's a sorted array
#	- by preference and if preconditions aren't met (tool, prof, ingredient), another one is tried
#	- (last precondition fail is reported0

sv = load_json(get_svar('downtime','{}'))
gv= sv.get('recipes',['6498daf1-6d03-43ac-822a-2badfd533749'])
data={}
for rgv in gv:
	data.update(load_json(get_gvar(rgv)))

arg = "&*&"
arg_split=arg.split(" -",maxsplit=1)
choice=arg_split[0]
args=argparse('-'+arg_split[1] if len(arg_split)>1 else '')
toolprofs = {pstr.strip().lower(): 1 for pstr in get('pTools', '').split(',') if not pstr.isspace()}
toolprofs.update({estr.strip().lower(): 2 for estr in get('eTools', '').split(',') if not estr.isspace()})

if choice:
	matches = [n for n in data.keys() if n.startswith(choice)]
	# not a recipe, if it's a category only list those recipes
	if not matches:
		idchoise=choice.replace(' ','')
		recipes = [n for (n,r) in data.items() if r.get('skill','').lower().startswith(idchoise.lower()) or r.get('tool','').lower().startswith(choice)]
		if recipes:
			return f'-title "{name} wonders what you can do with {choice}." -desc "You can `!craft <recipe>` where recipe is one of {",".join(recipes)}." '

# invalid input or help: show how it works.
if choice == '?' or choice == 'help' or choice == '' or not matches:
	return f'-title "{name} doesn\'t know how to craft." -desc "`!craft <recipe>` where recipe is one of {", ".join(data.keys())}."'

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
char.create_cc(ccn, maxVal=cc_max, reset=cc_reset)
if cc_val is not None:
    char.set_cc(ccn,cc_val)

# Check precondition: downtime
recipe_name = matches[0]
recipe = data[recipe_name]
item = recipe.get('item', recipe_name)
item_d = ('an  ' if item[0] in "aeiou" else 'a ') + item
plural=''
amount = recipe.get('amount',1)
if amount>1:
	item_d =f'{amount} {item}s'

downtime = get_cc(ccn)
ccfield =  f' -f Downtime|"{cc_str(ccn)}"|inline'
if not downtime:
	return f'-title "{name} doesn\'t have time to craft {item_d}." -desc "You have no more downtime left. Go do something useful."'+ccfield

# Check precondition: tool owned
bag_var='bags'
bag = load_json(bags) if exists(bag_var) else None
tool = recipe.get('tool')

if bag is not None and tool is not None:
	if not any([(b[1].get(tool,0)>0) for b in bag]):
		return f'-title "{name} doesn\'t own a {tool} to craft {item_d}." -desc "You can purchase the tool using `!bag buy \\"{tool}\\"`"'

# Check precondition: proficiency
skill = recipe.get('skill')

proficient = 0
if skill:
	proficient = char.skills[skill.lower()].prof or 0
elif tool:
	proficient = toolprofs.get(tool.lower(), 0)
if char.levels.get('Bard') >= 2:
	proficient = max(proficient,0.5)

if recipe.get('proficient', False) and not proficient:
	return f'-title "{name} has no clue how to craft {item_d}." -desc "You are not proficient enough in {tool}.\nUse `!tool pro {tool}` to add your proficiency."'

dbg = ''
fields = ''
# It's possible, initialize project
cvn='Projects'
project = load_json(get('Projects', '{}'))
if recipe_name in project:
	title = f'{name} spends their free time working on {item_d}.'
else:
	title = f'{name} starts working on {item_d}.'
	project[recipe_name] = {'progress': 0}
	# check and subtract costs in 1) args 2) recipe 3) default free
	cost = recipe.get('cost',0)
	rates = load_json(get_svar('coins', '{}')).get('rates', {"cp": 100, "sp": 10, "ep": 2, "gp": 1, "pp": 0.1})
	cost_str=args.get('cost',[''])[0].lower()
	if cost_str:
		cost=sum([float(cost_str[:-len(coin_name)])/coin_rate for (coin_name,coin_rate) in rates.items() if cost_str.endswith(coin_name)])

	inv_rates = {(1 / r): c for (c, r) in rates.items()}
	if cost > 0:
		# make a list of coins needed for if there are no bags or not enough coins
		total_needed = []
		cost_remaining = cost
		rev_coin_values = list(inv_rates.keys())
		rev_coin_values.reverse()
		for coin_value in rev_coin_values:
			coins_needed = int((cost_remaining + (0.1 * coin_value)) // coin_value)
			if coins_needed:
				total_needed += [f'{coins_needed} {inv_rates[coin_value]}']
				cost_remaining -= coins_needed * coin_value
		if bag:
			total_taken=[]
			spare_coins = []
			for coin_value in rev_coin_values:
				coin_name=inv_rates[coin_value]
				coins_needed=int((cost+(0.1*coin_value))//coin_value)	# add 0.1*coin for numeric imprecision ie 0.999 cp
				for b in bag:
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
								dbg+=f'{bag_coins}/{coins_needed} {coin_name} + conv {converted_coins}/{spare_amount}{spare_coin} at {conversion_rate}='
								bag_coins+=int(converted_coins*conversion_rate)
								spare_bag[spare_coin] = spare_amount-converted_coins
								#dbg+=f'{bag_coins}{coin_name}, '
						if coins_taken:=min(coins_needed, bag_coins):
							cost-=coins_taken*coin_value
							b[1][coin_name]=bag_coins-coins_taken
							total_taken+= [f'{coins_taken} {coin_name}']
							coins_needed-=coins_taken
							#dbg+=f'take {coins_taken}/{coins_needed}{coin_name} at {coin_value}, '
						elif bag_coins:
							#dbg+=f'spare found {bag_coins}{coin_name} '
							spare_coins=[(b[1],coin_name)]+spare_coins
			if cost>0:
				if not total_taken:
					total_taken=['nothing']
				return f'-title "{name} can\'t afford the ingredients for {item_d}." -desc "You need {" ".join(total_needed)}, but have {" ".join(total_taken)} in your bags." '
			else:
				fields+=f'-f Cost|"{" ".join(total_taken)}"|inline '
				#fields+=f'-f Debug|"{dbg}" '
				set_cvar(bag_var,dump_json(bag))
		else:
			fields += f'-f Cost|"{" ".join(total_needed)}"|inline '

# roll for success
boni = []
boni += [f'{get(e)}[{e[:3]}]' if e.isidentifier() else e for e in recipe.get('bonus', '').split('+') if e]

rollstr = ['1d20', '2d20kh1', '2d20kl1'][args.adv()]
if skill:
	skilld20 = char.skills[skill].d20(args.adv(False,True))
	r = vroll('+'.join([skilld20] + boni))
	fields += f'-f "{skill}"|"{r.full}"|inline '
elif tool:
	boni=[f'{int(proficient*proficiencyBonus)}[{tool}]']+boni
	r = vroll('+'.join([rollstr] + boni))
	fields += f'-f "{tool}"|"{r.full}"|inline '
elif boni:
	r = vroll('+'.join([rollstr] + boni))
	fields += f'-f Roll|"{r.full}"|inline '
else:
	r = None

# Make progress based on roll
if not r:
	advance = 1
elif r.result.crit == 1:
	advance = 1
elif r.total >= recipe.get('dc',1):
	advance = 1
else:
	advance = 0

if advance:
	project[recipe_name]['progress'] += advance
	desc = 'You make some progress.'
else:
	desc = 'You make no progress.'

# Report the progress
progress = project[recipe_name]['progress']
effort = recipe.get('effort',1)
if progress >= effort:
	project.pop(recipe_name)
	desc = f'You have finished crafting.'
	fields += '-f Progress|Done|inline '
	# automatically award the item if bags are set up
	if bag is not None:
		if not bag:
			bag+=['Held',[]]
		pref_bags=[]
		# if a bag is specified in the recipe, prefer that first
		if pref_bag:=recipe.get('bag'):
			pref_bags+=[b for b in  range(0,len(bag)) if pref_bag.lower() in bag[b][0].lower()]
		# else prefer all bags that are not special purpose
		special_bags=['quiver','worn','case, map','case, scroll','case, crossbow bolt','coin pouch']
		pref_bags+=[b for b in range(0,len(bag)) if bag[b][0].lower() not in special_bags]
		# add the item to the preferred bag
		target_bag = pref_bags[0] if pref_bags else 0
		bag[target_bag][1][item] = bag[target_bag][1].get(item,0)+amount
		desc+=f'\n{item_d} {"was" if amount==1 else "were"} added to your {bag[target_bag][0].lower()}.'
		set_cvar('bag_var',dump_json(bag))
	else:
		desc+=f' Please add {item_d} to your sheet.\nYou can manage your inventory with `!bag` to have it added automatically next time.'
else:
	pq = int(3.0 * progress / effort)
	pt = ['Just started', 'Halfway', 'Nearly done'][pq]
	fields += f'-f Progress|"{pt}"|inline '

if img_url:=recipe.get('img'):
	fields+=f'-thumb "{img_url}" '

# Apply the modifications to the character variables and counters
char.set_cvar(cvn, dump_json(project))
char.mod_cc(ccn, -1, True)
fields += f'-f Downtime|"{cc_str(ccn)}"|inline '

return f'-title "{title}" -desc "{desc}" ' + fields
</drac2>