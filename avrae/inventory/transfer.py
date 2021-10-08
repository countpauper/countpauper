<drac2>
args=&ARGS&
if not args:
	return f'echo `{ctx.prefix}{ctx.alias} transfer <bag> [<target>]`'

# bag state
bv='bags'
backup=get(bv)
bags=load_json(get(bv,'[]'))

# config
var_name='quickbag'
sv=get_svar(var_name,'')
config=load_json(get_gvar(get_svar(var_name,'71ff40cc-4c5f-4ae5-bdb6-32130f869090')))
purse_names=config.get('purses',[])
ammo_containers=config.get('ammo',[])
special_bags=purse_names + ammo_containers
item_table=load_json(get_gvar('19753bce-e2c1-42af-8c4f-baf56e2f6749'))	 # original !bag items for interop
coins=config.get('coinRates',{})
unmovable_items=list(coins.keys())

source_name=args[0].lower()
source_bag=([b for b in bags if b[0].lower()==source_name] +
			[b for b in bags if b[0].lower().startswith(source_name)] +
			[b for b in bags if ' '+source_name in b[0].lower()] +
			[None])[0]
if not source_bag:
	return f'echo No bag named `{source_name}` found to transfer from.'

target_name=args[1].lower() if len(args)>=2 else None
if not target_name:
	target_bag=([b for b in bags if b[0].lower() not in special_bags] + [None])[0]
	if not target_bag:
		return f'echo No default bag found to transfer to.'
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
# move all items
diff={}
for item_name, amount in source_bag[1].items():
	if item_name.lower() not in unmovable_items:
		old_amount=target_bag[1].get(item_name,0)
		new_amount = old_amount + amount
		target_bag[1][item_name]= new_amount
		source_bag[1][item_name]=0
		# remember changes for report
		plural_name=(item_name if item_name[-1]=='s' else item_name+'s') if new_amount>1 else item_name
		diff[plural_name]=(old_amount,new_amount)

# clean up source bag
source_bag[1]={i:q for i,q in source_bag[1].items() if q>0 or i in unmovable_items}

# clear the source bag, but only remove it if it is free(beer)
source_desc="*Empty*"
nl='\n'
source_name=source_bag[0].lower()
if not source_bag[1]:
	if not source_name in item_table or not item_table[source_name].get('cost'):
		bags.remove(source_bag)
		source_desc="*Removed*"
else:
	source_desc=nl.join(f'{q} x {i}' if q>1 else i for i,q in source_bag[1].items())

source_name = source_name.title().replace("'S","'s")

# backup and persist
if backup:
	character().set_cvar('bag_backup', backup)
character().set_cvar(bv,dump_json(bags))

# report changes

desc=[f'~~{old}~~ {new}x {item}' if old else f'+{item}' for item,(old,new) in diff.items()]
if not desc:
	desc=["*No items transferred*"]
possessive=f'{name}\'' if name[-1]=='s' else f'{name}\'s'
return f'embed -title "{possessive} bags" -f "{"+" if new_bag else ""}{target_bag[0]}|{nl.join(desc)}|inline" -f "{source_name}|{source_desc}|inline" -color {color} -thumb https://images2.imgbox.com/69/c2/Fe3klotA_o.png'
</drac2>