<drac2>
# first is default reverse
ft_per_m=3.28084
km_per_mile=1.60934
inch_per_m=39.3701
lbs_per_kg=2.20462
liter_per_galon=3.78541
ounce_per_kg=35.274
ounce_per_liter=33.814

table={
	"ft":{"m":1.0/ft_per_m},
	"feet": {"m": 1.0/ft_per_m},
	"'": {"m": 1.0/ft_per_m},
	"inch":{"cm":100/inch_per_m},
# avrae can't parse "\"": {"cm": 100/inch_per_m},
	"mile":{"km":km_per_mile},
	"km":{"mile":1.0/km_per_mile},
	"cm":{"feet":ft_per_m/100,"inch":inch_per_m/100},
	"m":{"feet":ft_per_m},
	"lb":{"kg":1.0/lbs_per_kg},
	"lbs": {"kg": 1.0/lbs_per_kg},
	"kg":{"lbs":lbs_per_kg},
	"ounce":{"g":1000.0/ounce_per_kg},
	"g":{"oz":ounce_per_kg/1000},
	"oz":{"g":1000.0/ounce_per_kg},
	"gal":{"l":liter_per_galon},
	"l":{"gal":1.0/liter_per_galon},
	"liter": {"gal": 1.0 / liter_per_galon},
	"fortnight":{"days": 14, "weeks":2}
}

arg="""&*&""".lower()
sep=', '
for u,conv in table.items():
	if arg.endswith(u):
		v=arg[:-len(u)].strip().replace(',','')
		if v=='':
			v='1'
		if v.replace('.','').isdecimal():
			v=float(v)

			return f'echo **{arg}**= {sep.join(f"{v*f:.1f} {u}" for u,f in conv.items())}'
return f'echo Unit of `{arg}` if not recognized. Use `!{ctx.prefix}{ctx.alias} <value> <unit>` where unit is one of {sep.join(table.keys())}. '
</drac2>