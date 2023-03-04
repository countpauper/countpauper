<drac2>
args=&ARGS&
if not args:
	return f'echo `{ctx.prefix}{ctx.alias} transfer <bag> [<target>] ["<filter item>"|-"<exclude item>"][...]`'

# bag state
bv='bags'
backup=get(bv)
bags=load_json(get(bv,'[]'))

# config
var_name='quickbag'
sv=get_svar(var_name,'')
config=load_json(get_gvar(get_svar(var_name,'71ff40cc-4c5f-4ae5-bdb6-32130f869090')))
purse_name=config.get('purse','Coinpurse')
purse_names=config.get('purse_bags',[])
ammo_containers=config.get('ammo',[])
special_bags=purse_names + ammo_containers
item_table=load_json(get_gvar('19753bce-e2c1-42af-8c4f-baf56e2f6749'))	 # original !bag items for interop
coins=config.get('coinRates',{})

backup_purse=False
purse=character().coinpurse.get_coins()
if 'total' in purse:
	purse.pop('total')

source_name=args[0].lower()
if source_name == purse_name.lower():
	source_bag=[purse_name, purse]
	backup_purse=True
else:
	source_bag=([b for b in bags if b[0].lower()==source_name] +
				[b for b in bags if b[0].lower().startswith(source_name)] +
				[b for b in bags if ' '+source_name in b[0].lower()] +
				[None])[0]
if not source_bag:
	return f'echo No bag named `{source_name}` found to transfer from.'

target_name=args[1].lower() if len(args)>=2 else None
if not target_name:
	target_bag=([b for b in bags if b[0].lower() not in purse_names] + [None])[0]
	if not target_bag:
		return f'echo No default bag found to transfer to.'
	new_bag = False
elif target_name==purse_name.lower():
	target_bag=[purse_name, purse]
	backup_purse=True
	new_bag = False
else:
	target_bag=([b for b in bags if b[0].lower()==target_name] +
				[b for b in bags if b[0].lower().startswith(target_name)] +
				[b for b in bags if ' '+target_name in b[0].lower()] +
				[None])
	target_bag=[b for b in target_bag if b is not source_bag]
	target_bag=target_bag[0]
	if not target_bag:	# create a new bag
		target_name=target_name.title().replace("'S","'s'")
		bags.append([target_name,{}])
		target_bag=bags[-1]
		new_bag=True
	else:
		target_name=target_bag[0]
		new_bag=False

# remaining arguments are to filter items
include_items=[a.lower() for a in args[2:] if not a.startswith('-')]
exclude_items=[a[1:].lower() for a in args[2:] if a.startswith('-')]

# if purse is affected, add it to the backup json
if backup_purse:
	backup_list=load_json(backup) if backup else dict()
	backup_list.append([purse_name,purse])
	backup=dump_json(backup_list)
	unmovable_items=[]
else:
	purse=None
	unmovable_items=list(coins.keys())

# move all items
dbg=[]
diff={}
for item_name, amount in source_bag[1].items():
	# filter prioritized
	if item_name.lower() in unmovable_items:
		dbg.append(f'Unmove {item_name}')
		continue
	elif any(include==item_name.lower() for include in include_items):
		dbg.append(f'Include {item_name}')
		pass	# exact (case insensitive match) to include
	elif any(exclude==item_name.lower() for exclude in exclude_items):
		dbg.append(f'Exclude {item_name}')
		continue	# exact (case insensitive match) to exclude
	elif any(any(item_part.startswith(include) for item_part in item_name.lower().split()) for include in include_items):
		dbg.append(f'include {item_name}')
		pass 	# partial to include
	elif any(any(item_part.startswith(exclude) for item_part in item_name.lower().split()) for exclude in exclude_items):
		dbg.append(f'exclude {item_name}')
		continue	# partial to exclude
	elif include_items:
		dbg.append(f'skip {item_name}')
		continue	# if include is specified but it doesn't match, then don't assume
	old_amount=target_bag[1].get(item_name,0)
	new_amount = old_amount + amount
	target_bag[1][item_name]= new_amount
	source_bag[1][item_name]=0
	# remember changes for report
	diff[item_name]=(old_amount,new_amount)

# clean up source bag of 0 count items

# clear the source bag, but only remove it if it is free(beer)
nl='\n'
plural_es = ['s', 'sh', 'ch', 'x', 'z']
coinIcons = config.get('coinIcons', {})
source_desc = "*Empty*"
source_name=source_bag[0].lower()
if source_name==purse_name.lower():
	source_desc=nl.join(f'{coinIcons.get(coin,coin)} ~~{character().coinpurse[coin]}~~ {coin}' for coin in purse.keys())
else:
	source_bag[1] = {i: q for i, q in source_bag[1].items() if q > 0 or i in unmovable_items}
	if not source_bag[1]:
		if source_name in purse_names or not source_name in item_table or not item_table[source_name].get('cost'):
			bags.remove(source_bag)
			source_desc="*Removed*"
	else:
		source_desc=nl.join(f'{q} x {i}{"es" if any(i.endswith(end) for end in plural_es) else "s"}' if q>1 else i for i,q in source_bag[1].items())

source_name = source_name.title().replace("'S","'s")

# backup and persist
if backup:
	character().set_cvar('bag_backup', backup)
if purse is not None:
	character().coinpurse.set_coins(purse.get('pp',0), purse.get('gp',0), purse.get('ep',0), purse.get('sp',0), purse.get('cp',0))
character().set_cvar(bv,dump_json(bags))

if target_name==purse_name.lower():
	desc=[]
	for coin in purse.keys():
		if change:=diff.get(coin): # TODO or showbag, then default to (q,q)
			if change[0]!=change[1]:
				desc.append(f'{coinIcons.get(coin,coin)} {change[1]:,} {coin} ({change[1]-change[0]:+,})')
			else:
				desc.append(f'{coinIcons.get(coin,coin)} {change[0]:,} {coin} ')
else:
	# pluralize
	plural_diff={(f'{new:,}x {i}{"es" if any(i.endswith(end) for end in plural_es) else "s"}' if new>1 else i):old for i,(old,new) in diff.items()}
	# report changes
	desc=[f'~~{old:,}~~ {item}' if old else item for item,old in plural_diff.items()]

if not desc:
	desc=["*No items transferred*"]

def format_field(header, items):
	item_desc=nl.join(items)
	limit = 1024
	if len(item_desc) + 10 + len(header) >= limit:
		return item_desc[:limit-len(header)-10]+'…'
	else:
		return item_desc

desc=format_field(target_bag[0], desc)

possessive=f'{name}\'' if name[-1]=='s' else f'{name}\'s'
dbg=f'-f "Debug|{nl.join(dbg)}"' if dbg and ctx.alias=='bqb' else ''
return f'embed -title "{possessive} bags" -f "{"+" if new_bag else ""}{target_bag[0]}|{desc}|inline" -f "{source_name}|{source_desc}|inline" {dbg} -color {color} -thumb https://images2.imgbox.com/69/c2/Fe3klotA_o.png'
</drac2>