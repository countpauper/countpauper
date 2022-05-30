<drac2>
def load_prices():
	gvars=load_yaml(get_svar('pricelist','["79f8db00-99f2-4e0a-a1c3-6b85ead56afa","f0eebf2a-8ed2-4a34-9520-a5e7bc8a5b92"]'))
	price_list=dict()
	for gv in gvars:
		price_list.update(load_yaml(get_gvar(gv)))
	return {i.lower():p for i,p in price_list.items()}

def match_query(items, query):
	matches=[m for m in items.keys() if query.lower() == m]
	matches +=[m for m in items.keys() if m.startswith(query.lower()) or ' '+query.lower() in m or '('+query.lower() in m]
	return matches

def format_price(price):
	if typeof(price)=='str':
		return price
	if price<0.1:
		return f"<:DDBCopper:948681049217597480>{int(price*100)} cp"
	if price<1:
		return f"<:DDBSilver:948681049288867930> {int(price*10)} sp"
	return f"<:DDBGold:948681049221775370> {int(price):,} gp"

def format_name(item):
	return item.split(',')[0].strip().title().replace('Of','of').replace("'S","'s")

arg = '''&*&'''.strip('"').strip("'")
# TODO: allow price ranges and price range only queries. Format maybe !bprice <=2000 >=100.
# TODO: add categories (weapons, armor, consumable/potion/scroll)?
if not arg:
	return f'echo Use `{ctx.prefix}{ctx.alias} <item name>`'

prices = load_prices()
items = match_query(prices, arg)
if not items:
	return f'echo `{arg}` has no listed price.'
else:
	prices={format_name(i) : format_price(prices.get(i)) for i in items}
	nl='\n'
	# TODO: for many matches split into headerless fields after desc of 2000 desc[0] will be desc, desc[1:] fields
	desc=nl.join(f'{i} : *{p}*' for i,p in prices.items())
	return f'''embed -title "Price list" -desc "{desc}" '''
</drac2>