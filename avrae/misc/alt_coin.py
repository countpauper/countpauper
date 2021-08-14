!servalias coins <drac2>
if args:='''&*&''':
	return f'echo Use `{ctx.prefix}qb {args}`'
if bags:=get('bags'):
	bags=load_json(bags)
	coins=dict()
	purses=['coin pouch']
	for p in [b for n,b in bags if n.lower() in purses]:
		for i,q in p.items():
			coins[i]=coins.get(i,0)+q
	desc='\n'.join(f'{q} {c}' for c,q in coins.items())
	return f'''embed -title "{name}'s coins" -desc "{desc}" '''
else:
	return f'echo You have nothing.'
</drac2>