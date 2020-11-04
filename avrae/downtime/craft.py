embed <drac2>
#TODO
# (partially done, just not all checked for prereqs) don't use recipe names, use item names, it's a sorted array
#	- by preference and if preconditions aren't met (tool, prof, ingredient), another one is tried
#	- (last precondition fail is reported

sv = load_json(get_svar('downtime','{}'))
gv= sv.get('recipes',['6498daf1-6d03-43ac-822a-2badfd533749'])
if typeof(gv)=='str':
	gv=gv.split(',')
data={}
for rgv in gv:
	data.update(load_json(get_gvar(rgv.strip())))

arg = "&*&"
choice=arg.split(" -",maxsplit=1)[0]
lowchoice = choice.lower()
args=argparse(arg)

game_data=load_json(get_gvar('c2bd6046-90aa-4a2e-844e-ee736ccbc4ab'))
if choice:
	matches = [n for (n,r) in data.items() if n.lower() == lowchoice or r.get('item','').lower() == lowchoice or r.get('plural','')== lowchoice]
	# partial match as backup
	if not matches:
		matches = [n for (n,r) in data.items() if n.lower().startswith(lowchoice) or r.get('item','').lower().startswith(lowchoice) or r.get('plural','').startswith(lowchoice)]
	# not a recipe, if it's a category only list those recipes
	if not matches:
		smatches = [s for (s, d) in game_data.get('skills', {}).items() if s.lower().startswith(lowchoice) or d.lower().startswith(lowchoice)]
		tmatches = [t for t in game_data.get('tools',[]) if t.lower().startswith(lowchoice)]
		if smatches or tmatches:
			if recipes := [n for (n, r) in data.items() if r.get('skill', '') in smatches or r.get('tool', '') in tmatches]:
				desc = f'You can `!craft <recipe>` where recipe is one of {", ".join(recipes)}.'
			else:
				desc = f'There are no recipes for that. To learn how to create your own use `!help {ctx.alias}`.'
			return f'-title "{name} wonders what you can do with {", ".join(smatches+tmatches)}." -desc "{desc}" '

# invalid input or help: show how it works.
if choice == '?' or lowchoice == 'help' or choice == '' or not matches:
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
if cc_max>0:
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
	item_d =f'{amount} {recipe.get("plural",item+"s")} '

effort = recipe.get('effort',1)
if effort>0:
	downtime = get_cc(ccn)
	if not downtime:
		return f'-title "{name} doesn\'t have time to craft {item_d}." -desc "You have no more downtime left. Go do something useful." -f Downtime|"{cc_str(ccn)}"|inline'

override = args.last('with')
override = override.lower().split('&') if override else []

# Check precondition: tool owned
bag_var='bags'
bag = load_json(bags) if exists(bag_var) else None
bag_update=False
tool = recipe.get('tool')
for o in override:
	tool_override = [t for t in game_data.get('tools',[]) if t.replace('\'',' ').split()[0].lower() == o or t.lower()==o]
	if tool_override:
		tool = tool_override[0]

if bag is not None and tool is not None:
	if not any([(b[1].get(tool,0)>0) for b in bag]):
		return f'-title "{name} doesn\'t own a {tool} to craft {item_d}." -desc "You can purchase the tool using `!bag buy \\"{tool}\\"`"'

# Check precondition: proficiency
skill = recipe.get('skill')
for o in override:
	skill_override = [s for (s,d) in game_data.get('skills',{}).items() if s.lower().startswith(o) or d.lower().startswith(o)]
	if skill_override:
		skill = skill_override[0]
if skill:
	s = char.skills[skill.lower()]
else:
	s = None
if skill and recipe.get('proficient', False) and s.prof<1:
	return f'-title "{name} has no clue how to craft {item_d}." -desc "You are not proficient enough in {game_data["skills"][skill]}."'

prof = ''
if exists('pTools') or exists('eTools'):
	toolprofs = {pstr.strip().lower(): 1 for pstr in get('pTools', '').split(',') if not pstr.isspace()}
	toolprofs.update({estr.strip().lower(): 2 for estr in get('eTools', '').split(',') if not estr.isspace()})
	if tool and tool.lower() in toolprofs:
		prof = f'{toolprofs[tool.lower()]*proficiencyBonus}[{tool}]'
	if tool and recipe.get('proficient', False) and not prof:
		return f'-title "{name} has no clue how to craft {item_d}." -desc "You are not proficient enough in {tool}.\nYou can use `!tool pro {tool}` to add your proficiency."'
elif tool and recipe.get('proficient', False):
	prof = f'{proficiencyBonus}[{tool}]'	# assume proficient if not using !tool, but required to be

if not prof and char.levels.get('Bard') >= 2:
	prof = f'{proficiencyBonus//2}[jack]'

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
	# cost override
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
				bag_update=True
		else:
			fields += f'-f Cost|"{" ".join(total_needed)}"|inline '
	# Take ingredients on start
	ingredients=recipe.get('ingredients',{})
	if typeof(ingredients) == 'str':
		ingredients=[i.strip() for i in ingredients.split(',')]
	if typeof(ingredients)=='SafeList':
		ingredients={ ii:ingredients.count(ii) for ii in ingredients }

	if ingredients:
		ingredient_desc=','.join((f'{q}x{i}' if (q>1) else i) for (i,q) in ingredients.items())
		if bag:
			items_missing = ingredients
			for b in bag:
				contents=b[1]
				for i in list(items_missing.keys()):
					if i in contents:
						bag_items=contents.get(i,0)
						missing_items=items_missing[i]
						items_taken=min(missing_items,bag_items)
						# NB: this will also check for and remove items for which 0 are required from the missing,
						#  but not from the bag, this intentionally allows for prerequirements that are not consumed
						if still_missing:=missing_items-items_taken:
							items_missing[i]=still_missing
						else:
							items_missing.pop(i)
						if still_left:=bag_items-items_taken:
							contents[i]=still_left
						else:
							contents.pop(i)
			if len(items_missing):
				return f'-title "{name} doesn\'t have the ingredients for {item_d}." -desc "You need {", ".join(items_missing)} in your bags." '
			else:
				bag_update=True
		fields+=f'-f Ingredients|"{ingredient_desc}"|inline '

# decide how to name the field for the roll
rolldesc='Roll'
if tool:
	rolldesc = tool
elif skill:
	rolldesc = game_data["skills"][skill]

# check crit or fail override without rolling
if 'crit' in override:
	advance=1
	fields += f'-f "{rolldesc}"|crit|inline '
elif 'fail' in override:
	advance=0
	fields += f'-f "{rolldesc}"|fail|inline '
else:
	# roll for success
	rolladv = 0
	rollstr = None
	boni = []
	s = None

	# add recipe bonuses
	if skill:
		s = char.skills[skill]
		sname = game_data['skills'][skill]
		if s.prof * proficiencyBonus >= roll(prof):
			boni += [f'{s.value}[{sname}]']
			prof=''	# use skill proficiency over lesser tool proficiency
		else:  # use tool prof, deduct skill prof (eg jack on ability check)
			boni += [f'{s.value - int(s.prof * proficiencyBonus)}[{sname}]']
		rolladv += 1 if s.adv is True else -1 if s.adv is False else 0
		rollstr = '1d20'
	if tool and prof:
		boni += [prof]
		rollstr = '1d20'
	if bonusstr := recipe.get('bonus'):
		ids = [v for v in bonusstr.replace('//',' ').translate("".maketrans("+-*/", "    ", " \t\n")).split() if v.isidentifier()]
		for id in ids:
			bonusstr = bonusstr.replace(id, f'{get(id, 0)}[{id[:3]}]')
		boni += [bonusstr]
		rollstr='1d20'

	# add argument override bonuses
	for o in override:
		if o[0] == '+':
			boni += [o[1:]]
		elif o[0] == '-':
			boni += [o]
		elif o.lower() == 'adv':
			rolladv += 1
		elif o.lower() == 'dis':
			rolladv -= 1
		elif o == 'fail':
			rollstr = '1'
		elif o == 'crit':
			rollstr = '20'
		elif o.isdigit():
			rollstr = o

	if rollstr:
		# Reliable talent
		if rollstr[1:4] == 'd20' and (roll(prof)>=proficiencyBonus or (s and s.prof >= 1)) and char.csettings.get('talent', False):
			rollstr = rollstr[:4] + 'mi10' + rollstr[4:]
		# halfing luck
		elif rollstr[1:4] == 'd20' and (reroll := char.csettings.get('reroll', 0)):
			rollstr = rollstr[:4] + f'ro{reroll}' + rollstr[4:]
		# apply the counted advantage of all overrides and skill
		if rollstr.startswith('1d'):
			if rolladv > 0:
				rollstr = '2d' + rollstr[2:] + 'kh1'
			elif rolladv < 0:
				rollstr = '2d' + rollstr[2:] + 'kl1'

		r = vroll('+'.join([rollstr] + boni))
		fields += f'-f "{rolldesc}"|"{r.full}"|inline '
	else:	# no tool, bonus, skill or override
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
			if pref_bag is True:
				pref_bags=bag
			else:
				pref_bags+=[b for b in  range(0,len(bag)) if pref_bag.lower() in bag[b][0].lower()]
		if pref_bags is bag:
			bag+=[[item,{}]]*amount
			desc+=f'\n{item_d} {"was" if amount==1 else "were"} added to your bags.'
		else:
			# else prefer all bags that are not special purpose
			special_bags=game_data.get('special_bags',[])
			pref_bags+=[b for b in range(0,len(bag)) if bag[b][0].lower() not in special_bags]
			# add the item to the preferred bag
			target_bag = pref_bags[0] if pref_bags else 0
			bag[target_bag][1][item] = bag[target_bag][1].get(item,0)+amount
			desc+=f'\n{item_d} {"was" if amount==1 else "were"} added to your {bag[target_bag][0].lower()}.'
		bag_update=True
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
if effort>0:
	char.mod_cc(ccn, -1, True)
	fields += f'-f Downtime|"{cc_str(ccn)}"|inline '
if bag_update:	# update bag at end to avoid item loss
	set_cvar(bag_var, dump_json(bag))

return f'-title "{title}" -desc "{desc}" ' + fields
</drac2>