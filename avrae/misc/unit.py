<drac2>
# first is default reverse
ft_per_m=3.28084
km_per_mile=1.60934
inch_per_m=39.3701
m_per_yard=0.9144

lbs_per_kg=2.20462
ounce_per_kg=35.274

ounce_per_liter=33.814
liter_per_galon=3.78541

minutes_per_hour=60
hours_per_day=24
days_per_week=7

table={
	# distance
	"ft":{"m":1.0/ft_per_m, "cm":100.0/ft_per_m},
	"foot": {"m": 1.0/ft_per_m, "cm":100.0/ft_per_m},
	"'": {"m": 1.0/ft_per_m, "cm":100.0/ft_per_m},
	"inch":{"cm":100/inch_per_m},
# avrae can't parse "\"": {"cm": 100/inch_per_m},
	"mile":{"km":km_per_mile},
	"km":{"mile":1.0/km_per_mile},
	"cm":{"foot":ft_per_m/100,"inch":inch_per_m/100},
	"m":{"foot":ft_per_m, "yard":1.0/m_per_yard},
	"yd":{"m":m_per_yard},
	"yard":{"m":m_per_yard},
	# weight
	"lb":{"kg":1.0/lbs_per_kg},
	"kg":{"lb":lbs_per_kg},
	"ounce":{"g":1000.0/ounce_per_kg},
	"g":{"oz":ounce_per_kg/1000},
	"oz":{"g":1000.0/ounce_per_kg},
	# volume
	"gal":{"l":liter_per_galon},
	"l":{"gal":1.0/liter_per_galon},
	"liter": {"gal": 1.0 / liter_per_galon},
	# time
	"hr":{"minute":minutes_per_hour, "day":1/hours_per_day},
	"hour":{"minute":minutes_per_hour, "day":1/hours_per_day},
	"day":{"hour":hours_per_day, "week":1/days_per_week},
	"week":{"day":days_per_week},
	"fortnight":{"day": 14, "week":2}
}

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
			conversions = [f"{v*f:.1f} {u if v*f<2 else plural.get(u,u)}" for u,f in conv.items()]
			return f'echo **{arg}**= {" or ".join(conversions)}'
return f'echo Unit of `{arg}` is not recognized. Use {syntax}.'
</drac2>