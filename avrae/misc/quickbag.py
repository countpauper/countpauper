<drac2>
# TODO: regonize packs as argument and use the official gvar to add them
#* create a backup and subcommand undo
# help sub command
# Add capped removal items back to show in failed
# Split and reorder all exact matches for priority: exact bag, set, item then partial bag set item
# with default bag, remove from any bag ? (split up removal from addition?)
# delta before bag selection doesn't add multiple but select an indexed copy [1 is first -1 is last]
# $ prefix buys (automatically remove coins), but change?
# add recognized class weapons to a worn/equipped bag instead
# add ammo to ammo bags instead
# support extra_packs variable like bag

args=&ARGS&

if not args:
	return f'help {ctx.alias} -here'

bv='bags'
backup=get(bv)
bags=load_json(get(bv,'[]'))
item_table=load_json(get_gvar('19753bce-e2c1-42af-8c4f-baf56e2f6749'))

# load configuration, prioroty: server, char, user, global
var_name='quickbag'
sv=get_svar(var_name,'')
config=load_json(sv if sv else get(var_name,get_gvar('71ff40cc-4c5f-4ae5-bdb6-32130f869090'))) # get_svar(var_name,get(var_name,get_gvar('71ff40cc-4c5f-4ae5-bdb6-32130f869090'))))

# TODO add to quickbag gvar and add svar override
purse_names=config.get('purses',[])
equipped_names=config.get('equipment',[])
ammo_containers=config.get('ammo',[])  # TODO: these make it impossible to add or remove with partial matching crossbow bolts,['case, crossbow bolt','crossbow bolt case']
containers=config.get('containers',[]) + equipped_names + ammo_containers + purse_names
specific_bags=purse_names + ammo_containers
sets=config.get('packs',{})
sets.update(config.get('backgrounds',{}))
sets={n.lower():c for n,c in sets.items()}
coins=config.get('coins',[])

# find the purse bag
coin_bags = [idx for idx,b in enumerate(bags) if b[0].lower() in purse_names]
coin_idx = coin_bags[0] if coin_bags else None

# convert arguments to quantified changes
delta=1
buy=False
bag_idx=None	# track selection using index for report
debug=[]
report={}	# list of modified bags: {bagidx:{item_name:[amount deltas]}], could be bag index to resolve reuse
fail='fail'

while args:
	# debug.append(str(args))
	arg=args.pop(0)

	## Prefixes
	if arg[0]=='$':	# unimplemented
		buy=True
		arg=arg[1:]
	elif arg[0]=='-':
		delta=-delta
		arg=arg[1:]
	elif arg[0]=='+':
		arg=arg[1:]

	if delta==1 and arg.isnumeric():
		delta=int(arg)
		continue
	elif delta==-1 and arg.isnumeric():
		delta=-int(arg)
		continue

	if not arg:	# spaces after prefixes are allowed, just continue parsing next argument
		continue

	# TODO: could maybe pop next arg here maybe and remove all the delta=1, buy=False, continue

	# TODO: if 1 is +1 and arg is the name of an existing bag, then select that bag. If it's preceded by $ then add the bag to bags first
	item_name=arg.lower()
	if delta==0:
		continue

	# select to existing bags if the delta is 1 and arg
	existing_bags = [idx for idx,b in enumerate(bags) if item_name.lower()==b[0].lower()]
	existing_bags += [idx for idx,b in enumerate(bags) if item_name.lower() in b[0].lower()]
	if delta==1 and existing_bags:
		bag_idx=existing_bags[0]
		bag=bags[bag_idx]
		# TODO: what if it's the same bag? dict? what is it's not? index?
		# TODO: report
		debug.append(f'Select {bag[0]}')
		if not bag_idx in report:
			report[bag_idx]={ }
		continue

	# TODO: if delta=-1 could delete existing bag and everything in it

	## Coins : if an item is in the coin pouch, change the coins (don't remove)
	if coin_idx is None:
		if item_name in coins:
			err(f'Before adding or removing `{item_name}`, first set your coins using `{ctx.prefix}coins`')
	else:
		coins=bags[coin_idx][1]
		if item_name in coins.keys():
			current=coins.get(item_name,0)
			if -delta>current:
				err(f'You don\'t have {-delta} {item_name}.')
			coins[item_name]=current+delta

			# update report
			diff = report[coin_idx]=report.get(coin_idx,{})
			diff[item_name]=diff.get(item_name,[])+[current, delta]
			report[coin_idx]=diff

			# TODO report[pouch_name].get(coin,[amount])
			debug.append(f'$ {delta} {item_name}')
			delta=1
			continue

	## Add: new bags
	if delta==1:
		new_bags=[nb for nb in containers if item_name in nb]
		if new_bags:
			new_bag=new_bags[0].title()
			bags.append([new_bag,{}])
			args=[new_bag]+args	# queue switching to this bag

			# add the bag's name in its report to remember that it's new
			report[len(bags)-1]={new_bag:1}
			debug.append(f'Bag {new_bag}')
			delta=1
			continue

	# Default bag: about to add/remove items from the current bag, if there is no current bag, select one
	if bag_idx is None:
		default_bags=[b[0] for b in bags if b[0].lower() not in specific_bags]
		# queue switching to the default bag explicitly and adding the current item. create arg basic one if none found
		default_bag=default_bags[0] if default_bags else containers[0]
		args=[default_bag,str(delta),arg]+args
		debug.append(f'Default {default_bag}')
		delta=1
		continue

	###  ITEMS
	bag = bags[bag_idx]

	# Modify: find items already in the current bag
	mod_items=[n for n in bag[1].keys() if item_name==n.lower()]
	mod_items+=[n for n in bag[1].keys() if item_name in n.lower()]
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
		diff[mod_item]=diff.get(mod_item,[current])+[current-amount]
		report[bag_idx]=diff

		delta=1
		continue

	# NEW known items
	new_items = [n for n in item_table.keys() if item_name == n]
	new_items += [n for n in item_table.keys() if item_name in n]
	# filter out exact sets because Priest's pack and such is both
	new_items=[n for n in new_items if n not in sets.keys()]
	if delta>0 and new_items:
		new_item=new_items[0].title().replace("'S","'s")
		bag[1][new_item]=delta
		debug.append(f'Add {delta} x {new_item}')
		# update the item's diff in the report
		diff=report.get(bag_idx,{})
		diff[new_item]=[0,delta]
		report[bag_idx]=diff

		delta=1
		continue

	# Sets: add all of a set's contents to the arguments and parse as normal
	item_set = [n for n in sets.keys() if item_name==n]
	item_set += [n for n in sets.keys() if item_name in n]
	if delta>0 and item_set:
		new_items=[]
		item_set=item_set[0]
		set_items=sets[item_set]
		for item_name,q in set_items.items():
			new_items+=[str(q),item_name]
		args=new_items * delta + args
		debug.append(f'Set {item_set}')
		delta=1
		continue

	# Lowest priority: unrecognized item
	if delta>0:
		bag[1][item_name]=delta
		debug.append(f'Unknown {delta} x {item_name}')
		diff=report.get(bag_idx,{})
		diff[item_name]=[0,delta]
		report[bag_idx]=diff
		delta=1
		continue

	debug.append('f Missing {-delta} {item_name}')
	diff=report.get('fail',{})
	diff[item_name]=diff.get(item_name,[])+[delta]
	report['fail']=diff

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

		contents=bag[1] if bag else {}
		items=[]
		if idx==coin_idx:
			# format as coins: no plural, no removal, no new items
			for coin,q in contents.items():
				if coin in changes:
					items.append(f'~~{changes[coin][0]}~~ {q} {coin}')
				else:
					items.append(f'{q} {coin}')
		else:
			for item_name,q in contents.items():
				plural_name=item_name if item_name[-1]=='s' else item_name+'s'

				item_desc= f'{q} x {plural_name}' if q>1 else item_name	# TODO: better plural
				if item_name in changes:
					original_amount=changes[item_name][0]
					if original_amount==0:
						items.append('+' + item_desc)
					else:
						items.append(f'~~{original_amount}~~ '+ item_desc)
				else:	# unchanged item
					items.append(item_desc)
			for item_name,diff in changes.items():
				plural_name=item_name if item_name[-1]=='s' else item_name+'s'
				if item_name not in contents.keys():
					original_amount=diff[0]
					items.append(f'~~{original_amount} x {plural_name}~~' if original_amount!=1 else f'~~{item_name}~~')
		if not items:
			items=['Empty']
		fields+=f' -f "{bag_name}|{nl.join(items)}|inline"'


#if debug:
#	fields+=f' -f "Debug|{", ".join(debug)}"'

#backup
if backup:
	character().set_cvar('bag_backup', backup)

#update the variable
character().set_cvar(bv,dump_json(bags))
possessive=f'{name}\'' if name[-1]=='s' else f'{name}\'s'

return f'embed -title "{possessive} bags" -thumb https://i.imgur.com/GT9YkF2.png {fields}'
</drac2>
