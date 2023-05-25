!alias pr <drac2>
mundane_gvar='f0eebf2a-8ed2-4a34-9520-a5e7bc8a5b92'
def load_prices():
	gvars=["79f8db00-99f2-4e0a-a1c3-6b85ead56afa", "f0eebf2a-8ed2-4a34-9520-a5e7bc8a5b92", "0528df3b-eafa-4613-b9a0-bf272382fbe8"]
    # = load_yaml(get_svar('pricelist',f'["79f8db00-99f2-4e0a-a1c3-6b85ead56afa","{mundane_gvar}"]'))
	price_list=dict()
	for gv in gvars:
		price_list.update(load_yaml(get_gvar(gv)))
	return {i.lower():p for i,p in price_list.items()}

prices = load_prices()
def simplify(s):
	return s.lower().replace('(each)','')

prices={simplify(n):p for n,p in prices.items()}
tables=load_json(get_gvar('1009a4b8-20b8-4892-8e7f-8259233128bf'))
table_name=&ARGS&[0].upper() # only first
table=tables.get(table_name)
if table is None:
    return f'echo Not a known table `{table_name}` in {tables.keys()}'

def shorten(s):
	if (sIdx:=s.find('('))>0:
		return s[:sIdx].strip()
	return s.strip()

def price(item_name):
	item_name = item_name.lower().strip()
	if p:=prices.get(item_name):
		return p
	item_name = shorten(item_name)
	if p:=prices.get(item_name):
		return p
	return None

table_prices=[dict(name=item.name, price=price(item.name), range=(1+int(item.max)-int(item.min) if item.get('max') else 1)) for item in table]
# return f'embed -desc "`{table_prices}`"'
known_prices = [p.price for p in table_prices if p.price is not None]
avg_price=sum(p.price * p.range for p in table_prices if p.price is not None)/sum(p.range for p in table_prices if p.price is not None)
price_list='\n'.join(f'{i.name}:{i.price}' for i in table_prices if i.price is not None)
unknown_list='\n'.join(i.name for i in table_prices if i.price is None)
txt = f'{min(known_prices)} to {max(known_prices)} Average: {avg_price}\n`{unknown_list}\n{price_list} '
return f'echo {txt[:1900]}'
</drac2>