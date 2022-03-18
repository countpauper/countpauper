<drac2>
# fix extra sack problem (ie !qb dungeon folk will add a sack because it doesn't have a default bag before the set. Move bag creation to actual adding items?)
# irrgular plural: torches (and such) in separate database
# if argument ends in plural (rations) and quantity>1 then remove plural on partial match
# compact coins for coin pouch display
# $ prefix buys (automatically remove coins), but change? and -$ to sell?
# Weight delta (with support for custom weight configuration from bag)
# Add capped removal items back to show in failed
# [x] delta before bag selection doesn't add multiple but select an indexed copy [1 is first -1 is last], add to help
# add recognized class weapons to a worn/equipped bag instead
# add ammo to ammo bags automatically
# support using coins not in pouch
# support extra_packs variable used by !bag

args=&ARGS&

if not args:
	return f'echo `{ctx.prefix}{ctx.alias} [<bag>] [+/-][amount] <item> [<background>] [<pack>] ...`'

# typo protection
if len(args)==1 and (('undo' in args[0] and len(args[0])<7) or
					 (all(c in "undo" for c in args[0].lower()) and len(args[0]) in [3,4])):
	return f"echo :confused: `{ctx.prefix}{ctx.alias} {args[0]}`? Maybe you mean `{ctx.prefix}{ctx.alias} undo`?"

bv='bags'
backup=get(bv, '[["Bag", {}]]')	# !bags can't handle []
bags=load_json(get(bv,'[]'))
item_table=load_json(get_gvar('19753bce-e2c1-42af-8c4f-baf56e2f6749'))	 # original !bag items for interop

# use bagSettings
bag_settings=load_json(get('bagSettings','{}'))
item_table.update(bag_settings.get('customWeights'))
show_bag=bag_settings.get('openMode','').lower()!='none'
show_weight=bag_settings.get('weightTracking','').lower()=='on'

# load configuration, prioroty: server, char, user, global
var_name='quickbag'
config=load_json(get_gvar(get_svar(var_name,'71ff40cc-4c5f-4ae5-bdb6-32130f869090')))

# TODO add to quickbag gvar and add svar override
purse_names=config.get('purse_bags',[])
purse_name=config.get('purse','Coinpurse')
equipped_names=config.get('equipment',[])
ammo_containers=config.get('ammo',[])
containers=config.get('containers',[]) + equipped_names + ammo_containers + purse_names
special_bags=purse_names + ammo_containers
sets=config.get('packs',{})
sets.update(config.get('backgrounds',{}))
sets={n.lower():c for n,c in sets.items()}
coins=config.get('coinRates',{})
short_words=config.get('forbidden',[])
removed_items=["exploder's pack"]
item_list=[i for i in item_table.keys() if i not in sets.keys() and i not in removed_items]

iterations=int((10000-2000)/len(item_list))	# limit iterations through the item sets when computing weight especially
# add the purse bag at the end
purse_idx=len(bags)
purse=character().coinpurse.get_coins()
if 'total' in purse:
	purse.pop('total')
bags.append([purse_name,purse])

# find the purse bag
coin_bag_idx = [idx for idx,b in enumerate(bags) if b[0].lower() in purse_names]
coin_bag_idx.append(purse_idx)

# optionally enable debug mode
debug_break=None
if args[0].startswith('dbg='):
	dbg = 0
	debug_break=args.pop(0)[4:]
	debug_break=int(debug_break) if debug_break else 999

# convert arguments to quantified changes
delta=None
buy=False
bag_idx=None	# track selection using index for report
force_bag=False	# explicitly selected bags have preference for the next item over coin pouch and removal
removed_bags=[]
debug=[]
report={}	# list of modified bags: {bagidx:{item_name:[amount deltas]}], could be bag index to resolve reuse
fail='fail'
partial=False
while args:
	# debug break
	if debug_break:
		dbg+=1
		if dbg>=debug_break:
			break
	# debug.append(str(args))
	arg=args.pop(0)

	# fix characters
	arg=arg.replace("’","'")

	## Prefixes
	if arg[0]=='$':	# unimplemented
		buy=True
		arg=arg[1:]
		delta=1
	elif arg[0]=='-':
		delta=-1
		arg=arg[1:]
	elif arg[0] == '+':
		arg = arg[1:]
		delta=1

	# split number from item_name, especially for coins 2gp => 2 gp
	for n_pos in range(len(arg)):
		if not arg[n_pos].isdecimal():
			if n_pos:
				args.insert(0, arg[n_pos:])
				arg = arg[:n_pos]
			break

	# amounts
	if (delta==1 or delta is None) and arg.isdecimal():
		delta=int(arg)
		continue
	elif delta==-1 and arg.isdecimal():
		delta=-int(arg)
		continue

	if arg and arg[0]=='~':
		partial=True
		arg=arg[1:]
	else:
		partial=False

	if not arg:	# spaces after prefixes are allowed, just continue parsing next argument
		continue

	# heuristic to detect missing quotes
	if arg in short_words:
		return f'echo `{arg}` is not a recognizable item. Use "quotes" for items that consist of more than one word.'

	# TODO: could maybe pop next arg here maybe and remove all the delta=1, buy=False, continue
	if delta==0:
		continue

	item_name=arg.lower()
	# jank optimization:
	#  - just using item_name in string overfits,
	#  - using item_name in string.split() underfits (ration, clothes, traveler's)
	#  - extra list comprehension any([a.startsWith(item_name) for a in string.split()]) hits loop limit
	#  so: string.startsWith(item_name) or space_name in string
	space_name = f' {item_name}'

	# select to existing bags if the delta is 1 and arg, partial and
	if delta is None or delta==-1:
		# partial and exact matching at once to avoid creating a default sack when partial matching
		existing_bags = [idx for idx,b in enumerate(bags) if idx not in removed_bags and (item_name.lower()==b[0].lower())]
		if not existing_bags:
			existing_bags = [idx for idx,b in enumerate(bags) if idx not in removed_bags and (b[0].lower().startswith(item_name.lower()) or space_name in b[0].lower())]
		# Delete exiting bag
		if delta==-1 and existing_bags:
			removed_bag_idx=existing_bags[-1]	# remove last bag first
			if removed_bag_idx==purse_idx:
				# instead of removing the coinpurse
				force_bag=True
				bag_idx=purse_idx
				money=bags[bag_idx][1]
				for coin,q in money.items():
					if q:
						args=[str(-q), coin] + args
				debug.append(f'Empty purse')
				delta=None
			else:
				removed_bag=bags[removed_bag_idx]
				removed_bags.append(removed_bag_idx)	# don't remove until the end
				if removed_bag_idx in coin_bag_idx:
					coin_bag_idx.remove(removed_bag_idx)
				diff = report.get(removed_bag_idx,{})
				for i,q in removed_bag[1].items():
					removed_bag[1][i]=0
					diff[i]=diff.get(i,[q])+[0]

				if removed_bag_idx==bag_idx:
					bag_idx=None
				report[removed_bag_idx]=diff
				debug.append(f'Remove Bag {removed_bag[0]}')
				delta+=1
				if delta:	# repeat remove next backpack
					args.insert(0,item_name)
				else:
					delta=None
			continue
		# select existing bag
		elif delta is None and existing_bags:
			bag_idx=existing_bags[0]
			force_bag=True
			bag=bags[bag_idx]
			# TODO: what if it's the same bag? dict? what is it's not? index?
			# TODO: report
			debug.append(f'Select {bag[0]}')

			continue
		delta = 1 if delta is None else delta

	## Coins : if an item is in the coin pouch, change the coins (don't remove the whole entry)
	if item_name in coins:
		if force_bag:
			used_purses=[idx for idx in coin_bag_idx if idx==bag_idx]
		else:
			used_purses=coin_bag_idx
		if used_purses:
			for coin_idx in used_purses:
				money=bags[coin_idx][1]
				current=money.get(item_name)
				diff = report.get(coin_idx, {})
				if current+delta>=0:
					# enough coins, take them or add them
					money[item_name] = current + delta
					debug.append(f'${delta} {item_name}')
					# update report
					diff[item_name] = diff.get(item_name, [current]) + [delta]
					report[coin_idx] = diff
					delta = None
					break
				elif current:
					# coins, but not enough, take them and continue with the remaining coins
					money[item_name] = 0
					debug.append(f'$= {current}/{-delta} {item_name}')

					diff[item_name] = diff.get(item_name, [current]) + [-current]
					report[coin_idx] = diff

					delta = delta + current
			else:
				# none of the purses have these coins, make change
				bigger_coins={c:r for c,r in coins.items() if r<coins[item_name]}
				if not bigger_coins:
					err(f'You don\'t have {-delta} {item_name}.')
				else:
					next_coin=list(bigger_coins.keys())[0]
					exchange_rate=int(round(coins[item_name]/coins[next_coin]))
					next_needed=int(ceil((-delta-current)/exchange_rate))
					change=next_needed*exchange_rate + delta
					debug.append(f'$$ {next_needed} {next_coin} +{change} {item_name}')
					if change:
						args=[next_coin, str(change), item_name]+args
					else:
						args=[next_coin]+args
					delta=-next_needed
					continue
			if delta is None:
				continue

	## Add: new bags
	if delta>=1:
		if partial:
			new_bags=[nb for nb in containers if nb.startswith(item_name) or space_name in nb]
		else:
			new_bags=[nb for nb in containers if item_name==nb]
		if new_bags:
			new_bag=new_bags[0].title()
			bags+=[[new_bag,{}]]*delta
			bag_idx=len(bags)-delta

			# add the bag's name in its report to remember that it's new
			report[bag_idx]={new_bag:[delta]}
			debug.append(f'Bag {delta}x{new_bag}')
			delta=None
			continue


	# Remove items from any bag
	if delta<0:
		if force_bag:
			remove_bags={bag_idx:bags[bag_idx][1]}
		else:
			remove_bags={i:b[1] for i,b in enumerate(bags) if not i in removed_bags}
		for bi,remove_bag in remove_bags.items():
			if partial:
				remove_items = {n:q for n,q in remove_bag.items() if n.lower().startswith(item_name) or space_name in n.lower()}
			else:
				remove_items = {n:q for n,q in remove_bag.items() if n.lower()==item_name or # exact match
								(delta<1 and item_name[-1]=='s' and n.lower()==item_name[:-1])}	# plural name exact singular match
			for remove_item,current in remove_items.items():
				if current>-delta:
					remove_bag[remove_item]+=delta
					removed=-delta
					delta=None
				else:
					removed=current
					remove_bag.pop(remove_item)
					delta+=removed
				debug.append(f'RemoveAny {current}-{removed} x {remove_item}')
				diff = report.get(bi, {})
				diff[remove_item] = diff.get(remove_item, [current]) + [-removed]
				report[bi] = diff
				if not delta:
					break
			if not delta:
				break

		if not delta:
			delta=None
			continue

	# Default bag: about to add items to  the current bag, if there is no current bag (or its the purse), select one
	if bag_idx is None or bag_idx==purse_idx:
		default_bags = [(i,b[0]) for i, b in enumerate(bags) if
						i not in removed_bags and (b[0].lower() not in special_bags) and i!=purse_idx]
		# queue switching to the default bag explicitly and adding the current item. create arg basic one if none found
		if default_bags:
			bag_idx,default_bag = default_bags[0]
			debug.append(f'Default {default_bag}')
			bag=bags[bag_idx]
		elif containers and delta > 0:
			default_bag = containers[0]
			debug.append(f'Default* {default_bag}')
			bags.append([default_bag,{}])
			bag_idx=len(bags)-1
			bag=bags[bag_idx]
			report[bag_idx]={default_bag:[1]}
		else:
			pass

	###  ITEMS
	bag = bags[bag_idx]
	# Modify: find items already in the current bag, always use partial match first
	if partial:
		mod_items=[]
	else:
		mod_items = [n for n in bag[1].keys() if n.lower().startswith(item_name) or space_name in n.lower()]
	if mod_items:
		mod_item=mod_items[0]
		current=bag[1].get(mod_item)
		amount=max(0,current+delta)
		if amount:
			bag[1][mod_item]=amount
			debug.append(f'Adjust {current}+{delta}={amount} x {mod_item}')
		else:
			bag[1].pop(mod_item)
			debug.append(f'Remove {current} x {mod_item}')

		# update the item's diff in the report
		diff=report.get(bag_idx,{})
		change=current-amount
		diff[mod_item]=diff.get(mod_item,[current])+[change]
		report[bag_idx]=diff

		# next arg, unless still items to remove
		delta+=change
		if delta>=0:
			delta=None
			continue

	# Add NEW known items to the current bag
	if delta>0:
		if len(item_name)==1:
			return f'echo `{item_name}` is not a recognizable item. Use "quotes" for items that consist of more than one word.'
		if partial:
			iterations-=1
			new_items += [n for n in item_list if n.startswith(item_name) or space_name in n.lower()]
		else:
			iterations-=1
			new_items = [n for n in item_list if item_name == n]
		if new_items:
			new_item=new_items[0].title().replace("'S","'s")
			bag[1][new_item]=delta
			debug.append(f'Add {delta} x {new_item}')
			# update the item's diff in the report
			diff=report.get(bag_idx,{})
			diff[new_item]=[0,delta]
			report[bag_idx]=diff

			delta=None
			continue

		# Sets: add all of a set's contents to the arguments and parse as normal
		if partial:
			item_set += [n for n in sets.keys() if n.startswith(item_name) or space_name in n.lower()]
		else:
			item_set = [n for n in sets.keys() if item_name==n]
		if item_set:
			item_set=item_set[0]
			set_items=sets[item_set]
			added_args=[]
			for item_name,q in set_items.items():
				added_args+=[str(q),item_name]
			args=added_args * delta + args
			debug.append(f'Set {item_set}')
			delta=None
			continue

	# try again with partial prefix
	if not partial:
		args.insert(0,f'~{arg}')
		continue

	# Lowest priority: unrecognized item
	if delta>0:
		new_item=item_name.title().replace("'S","'s")
		bag[1][new_item]=delta
		debug.append(f'Unknown {delta} x {new_item}')
		diff=report.get(bag_idx,{})
		diff[new_item]=[0,delta]
		report[bag_idx]=diff
		delta=None
		continue

	# missing items
	if delta<0:
		debug.append(f'Missing {-delta} {item_name}')
		diff=report.get('fail',{})
		diff[item_name]=diff.get(item_name,[])+[delta]
		report['fail']=diff
		continue

	delta=1

# format the bags
nl='\n'
fields=''
if  report:
	for idx,changes in report.items():
		if idx==fail:
			bag_name="Failed"
			bag=None
		else:
			bag=bags[idx]
			bag_name=bag[0]
		if bag_name in changes:
			changes.pop(bag_name)
			bag_name+='+'
		if idx in removed_bags:
			bag_name+='-'

		contents=bag[1] if bag else {}
		items=[]
		done_items=[]
		if idx in coin_bag_idx:
			# format as coins: no plural, no removal, no new items
			coinIcons = config.get('coinIcons',{})
			done_items+=list(coinIcons.keys())
			for coin, icon in coinIcons.items():
				q=contents.get(coin,0)
				if coin in changes:
					items.append(f'{icon} ~~{changes[coin][0]}~~ {q} {coin}')
				else:
					items.append(f'{icon} {q} {coin}')
		for item_name,q in contents.items():
			if item_name in done_items:
				continue
			done_items.append(item_name)
			plural_name=item_name if item_name[-1]=='s' else item_name+'s'
			item_desc= f'{q} x {plural_name}' if q>1 else item_name	# TODO: better plural
			if item_name in changes:
				original_amount=changes[item_name][0]
				if original_amount==0:
					items.append('+' + item_desc)
				else:
					items.append(f'~~{original_amount}~~ '+ item_desc)
			elif show_bag:	# unchanged item
				items.append(item_desc)
		for item_name,diff in changes.items():
			if item_name in done_items:
				continue
			plural_name=item_name if item_name[-1]=='s' else item_name+'s'
			original_amount=diff[0]
			items.append(f'~~{original_amount} x {plural_name}~~' if original_amount!=1 else f'~~{item_name}~~')
		if not items:
			items=['Empty']
		fields+=f' -f "{bag_name}|{nl.join(items)}|inline"'

# update the purse if changed
purse = bags.pop(purse_idx)[1]
if report.get(purse_idx):
	# inject old coinpurse into backup
	backup_bags=load_json(backup)
	backup_purse=character().coinpurse.get_coins()
	if 'total' in backup_purse:
		backup_purse.pop('total')
	backup_bags.append([purse_name, backup_purse])
	backup=dump_json(backup_bags)
	# apply purse update
	character().coinpurse.set_coins(purse.get('pp',0), purse.get('gp',0), purse.get('ep',0), purse.get('sp',0), purse.get('cp',0))

# remove bags after indices are no longer referenced
bag_idx=None
bags=[b for i,b in enumerate(bags) if i not in removed_bags]

#backup
if backup:
	character().set_cvar('bag_backup', backup)

#update the variable
final_bags=bags
character().set_cvar(bv,dump_json(bags))

# Compute the weight
footer=''
if show_weight:
	item_table.update({coin: dict(weight=0.02, cost=1 / rate) for coin, rate in coins.items()})
	weightless = bag_settings.get('weightlessBags',[])
	weight=0
	unmatched=[]
	items=len(final_bags)
	for bag,content in final_bags:
		bag=bag.lower()
		match=item_table.get(bag) # ,([i for k,i in item_table.items() if bag.lower() in k]+[None])[0])
		if not match and iterations>0:
			iterations-=1
			match=([i for n,i in item_table.items() if n in bag]+[None])[0]
		if match:
			weight+=match.get('weight',0)
		else:
			unmatched.append(bag)
		if bag.lower() in weightless:
			continue
		items+=len(content)
		for item,quantity in content.items():
			item=item.lower()
			match = item_table.get(item) # , ([i for k, i in item_table.items() if item.lower() in k] + [None])[0])
			if not match and iterations>0:
				iterations-=1
				# partial match,
				match = ([None]+[i for n, i in item_table.items() if n in item])[-1]
			if match:
				weight += quantity * match.get('weight', 0)
			else:
				unmatched.append(item)
	max_weight=character().stats.strength *15
	powerful_races = ['Firbolg','Goliath','Orcs']+load_json(get('powerfulBuildRaces','[]'))
	if any(r.lower() in character().race.lower() for r in powerful_races):
		max_weight*=2
	footer=f'-footer "Weight carried {weight:.2f} / {max_weight} lbs, with {len(unmatched) if unmatched else "no" } unrecognized items."'

if debug_break:
	fields+=f' -f "Debug [{dbg}/{debug_break}]|{nl.join(debug)}|inline" -f "Iterations|{iterations} remaining|inline"'
	if args:
		fields+=f' -f "arguments remaining|`{args}`"'

# Format the output
possessive=f'{name}\'' if name[-1]=='s' else f'{name}\'s'
return f'embed -title "{possessive} bags" -thumb https://images2.imgbox.com/69/c2/Fe3klotA_o.png {fields} {footer} -color {color}'
</drac2>
