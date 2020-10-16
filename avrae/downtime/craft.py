tembed <drac2>
#TODO
# money cost on start
# option to ignore/override money cost -cost 50gp (argparse(%*%))
# don't use recipe names, use item names, it's a sorted array
#	- by preference and if preconditions aren't met (tool, prof, ingredient), another one is tried
#	- (last precondition fail is reported0
# image thumb in data per recipe
# required recipe {Item} recipe (not used)



data = load_json(get_gvar(get_svar("recipes", "6498daf1-6d03-43ac-822a-2badfd533749")))
arg = "&*&"
toolprofs = {pstr.strip().lower(): 1 for pstr in get('pTools', '').split(',') if not pstr.isspace()}
toolprofs.update({estr.strip().lower(): 2 for estr in get('eTools', '').split(',') if not estr.isspace()})

if arg:
	matches = [n for n in data.keys() if n.startswith(arg.lower())]
	# not a recipe, if it's a category only list those recipes
	if not matches:
		idarg=arg.replace(' ','')
		recipes = [n for (n,r) in data.items() if r.get('skill','').lower().startswith(idarg.lower()) or r.get('tool','').lower().startswith(arg.lower())]
		if recipes:
			return f'-title "{name} wonders what you can do with {arg}." -desc "You can `!craft <recipe>` where recipe is one of {",".join(recipes)}." '

# invalid input or help: show how it works.
if arg == '?' or arg.lower() == 'help' or arg == '' or not matches:
	return f'-title "{name} doesn\'t know how to craft." -desc "`!craft <recipe>` where recipe is one of {", ".join(data.keys())}."'
# Check precondition: downtime
recipe_name = matches[0]
recipe = data[recipe_name]
item = recipe.get('item', recipe_name)
item_d = ('an  ' if item[0] in "aeiou" else 'a ') + item
plural=''
amount = recipe.get('amount',1)
if amount>1:
	item_d =f'{amount} {item}s'
ccn = 'Downtime'
if not cc_exists(ccn):
	return f'-title "{name} doesn\'t have time to craft {item_d}." -desc "You need to have a Downtime counter to spend.'+ \
		'\nYou can create it with `!cc create Downtime -max 4 -reset long`."'
downtime = get_cc(ccn)
ccfield =  f' -f Downtime|"{cc_str(ccn)}"|inline'
if not downtime:
	return f'-title "{name} doesn\'t have time to craft {item_d}." -desc "You have no more downtime left. Go do something useful."'+ccfield

# Check precondition: tool owned
bag = load_json(bags) if exists('bags') else None
tool = recipe.get('tool')

if bag is not None and tool is not None:
	if not any([(b[1].get(tool,0)>0) for b in bag]):
		return f'-title "{name} doesn\'t own a {tool} to craft {item_d}." -desc "You can purchase the tool using `!bag buy \\"{tool}\\"`"'

# Check precondition: proficiency
skill = recipe.get('skill')

proficient = 0
if skill:
	proficient = character().skills[skill.lower()].prof or 0
elif tool:
	proficient = toolprofs.get(tool.lower(), 0)
if character().levels.get('Bard') >= 2:
	proficient = max(proficient,0.5)

if recipe.get('proficient', False) and not proficient:
	return f'-title "{name} has no clue how to craft {item_d}." -desc "You are not proficient enough in {tool}.\nUse `!tool pro {tool}` to add your proficiency."'

# It's possible, initialize project
cvn='Projects'
project = load_json(get('Projects', '{}'))
if recipe_name in project:
	title = f'{name} spends their free time working on {item_d}.'
else:
	title = f'{name} starts working on {item_d}.'
	project[recipe_name] = {'progress': 0}

# roll for success
fields = ''
boni = []
boni += [f'{get(e)}[{e[:3]}]' if e.isidentifier() else e for e in recipe.get('bonus', '').split('+') if e]

if skill:
	skillBonus = character().skills[skill].value
	boni=[f'{skillBonus}[{skill}]'] + boni
	r = vroll('+'.join(['1d20']+boni))
	fields += f'-f "{skill}"|"{r.full}"|inline '
elif tool:
	boni=[f'{int(proficient*proficiencyBonus)}[{tool}]']+boni
	r = vroll('+'.join(['1d20'] + boni))
	fields += f'-f "{tool}"|"{r.full}"|inline '
elif boni:
	r = vroll('+'.join(['1d20'] + boni))
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
		set_cvar('bags',dump_json(bag))
	else:
		desc+=f' Please add {item_d} to your sheet.\nYou can manage your inventory with `!bag` to have it added automatically next time.'
else:
	pq = int(3.0 * progress / effort)
	pt = ['Just started', 'Halfway', 'Nearly done'][pq]
	fields += f'-f Progress|"{pt}"|inline '

# Apply the modifications to the character variables and counters
set_cvar(cvn, dump_json(project))
mod_cc(ccn, -1, True)
fields += f'-f Downtime|"{cc_str(ccn)}"|inline '

return f'-title "{title}" -desc "{desc}" ' + fields
</drac2>