!alias hbtable <drac2>
# load treasure tables
treasure_guid=get('treasure_table',get_svar('treasure_table','1009a4b8-20b8-4892-8e7f-8259233128bf'))
tables=load_json(get_gvar(treasure_guid))

# parse arguments
args=&ARGS&
req=dict()
for idx,a in enumerate(args):
	A=a.upper()
	if A in tables:
		amt='1'
		if idx+1<len(args) and not args[idx+1].upper() in tables:
			amt=args[idx+1]
		else:
			amt = '1'
		req[A]=req.get(A,[])+[amt]
# roll all amounts
req={t:vroll('+'.join(amt for amt in amount)) for t,amount in req.items()}

# roll, treasure will be a {"table title",["item description",..]}
treasure=dict()
for t,r in req.items():
	r_title=str(r.total) if typeof(r.raw.set[0])=="Literal" else r
	title=f'{r_title} item{"s" if r.total>1 else ""} from table {t}'
	table_items=[]
	for idx in range(r.total):
		table=tables.get(t,[])
		table=[dict(min=int(item.min),max=int(item.get('max',item.min)),name=item.name) for item in table]
		if table:
			die_size=max(item.max for item in table)
			item_roll=vroll(f'1d{die_size}')
			item_idx = f'**{idx + 1}.**' if r.total > 1 else ''
			if match:=[item.name for item in table if item_roll.total>=item.min and item_roll.total<=item.max]:
				item=match[0]
			else:
				item='???'
			table_items.append(f'{item_idx} `{item_roll.total}` - {item}')
		treasure[title]=table_items

# format output
nl='\n'
fields=' '.join(f' -f "{table}|{nl.join(items)[:1024]}"' for table,items in treasure.items())
return f'embed -title "Rolling for treasure" {fields} -footer "{ctx.prefix}{ctx.alias} [A-I] [roll=1] ..."'
</drac2>