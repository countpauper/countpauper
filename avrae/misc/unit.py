<drac2>
table=load_yaml(get_gvar('7b9457fc-3dfa-44f6-a0a2-794388ca9a28'))
plural=dict(foot='feet',inch='inches',mile='miles',yard='yards',lb='lbs', ounce='ounces', liter='liters',minute='minutes', hour='hours', day='days',week='weeks',fortnight='fortnights')

arg="""&*&""".lower()
sep=', '
syntax = f'`{ctx.prefix}{ctx.alias} [<value>]<unit>`, where units is one of {sep.join(table.keys())}'
if not arg:
	return f"echo You didn't specify a value or unit. Use {syntax}."

for singular, multiple in plural.items():
	arg = arg.replace(multiple, singular)

# TODO: parse all input units and if they all have an overlapping output unit add them together uscase: 5ft 4 inch = meter/cm
# TODO: for the output unit if total<1 and another output unit has total>1 then use that use case: 3cm = 1.2 inch, don't mention feet (?) but mostly:
# TODO: if there's a fraction left after whole units and another conversion is available use that? use case 166cm = 5ft 5 inch
for u, conv in table.items():
	if arg.endswith(u):
		v=arg[:-len(u)].strip().replace(',','')
		if v=='':
			v='1'
		if v.replace('.','').isdecimal():
			v=float(v)
			conversions = [f"{v*f:.1f} {u if v*f<2 else plural.get(u,u)}" for u,f in conv.items() if f!=1 and v*f>=0.1]
			return f'echo **{arg}**= {" or ".join(conversions)}'
return f'echo Unit of `{arg}` is not recognized. Use {syntax}.'
</drac2>