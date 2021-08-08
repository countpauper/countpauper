!alias weather <drac2>
args=argparse([a.lower() for a in &ARGS&])
cv,n='climate',"\n"
g=load_json(get_gvar('16b912b6-817a-4923-9f66-f765bd69f346'))
C=g.climate
if args.get('help'):
	return f'echo `{ctx.prefix}{ctx.alias} [<climate>] [<season>] [-h <humidity bonus>] [-w <wind speed bonus>] [-t <temperature bonus>]`'
if args.get('list'):
	return f'embed -title "Supported Climates" -desc "{n.join(c.title() for c in C.keys())}" -footer "{ctx.prefix}{ctx.alias} [climate] [season]"'
x2=([k for k in C.keys() if args.last(k)]+[get(cv)])[0]
if not x2:
	return f'echo No default climate set'
set_uvar(cv,x2)

sns=g.season
dv = f'Date_{ctx.guild.id}' if ctx.guild else 'Date'
z = int(get(dv, randint(366))) % 365
ss = [(n, sn.idx) for n, sn in sns.items() if
	  (sn.start < sn.end and z >= sn.start and z <= sn.end) or (sn.end < sn.start and (z <= sn.end or z >= sn.start))]
p,s=ss[0]
p=([k for k in sns.keys() if args.get(k)]+[p])[0]
s=sns[p].idx

c=C[x2]
th=roll('+'.join([c.th[s]]+args.get('t')))
tl=roll('+'.join([c.tl[s]]+args.get('t')))
r=max(1,roll('+'.join([c.humid[s]]+args.get('h'))))
w=max(0,roll('+'.join([c.wind[s]]+args.get('w'))))
wd=roll("1d8")
dir=g.wind
c=g.weather

s=p.title()
bft=[0,1,4,8,13,19,25,32,39,47,55,64,73]
bft=min(idx for idx,mpb in enumerate(bft) if mpb>=w) if w<bft[-1] else 12
t=f'-title "Today\'s Weather:  {x2.title()}, {s}"'
if tl>0:
	weather=[w for v,w in g.weather.items() if r>int(v)][-1]
else:
	weather=[w for v,w in g.freezing.items() if r>int(v)][-1]

weather=f'-f "Weather|{weather}"'
wind=f'-f "Wind|💨 {bft} bft ({w} mph, {round(0.447*w)} m/s), {(dir[wd-1] if w!=0 else "Still")}"'
c0f,cf=32,9/5
temp=f'-f "Temperature|🌡️ High: {th}°C ({round(cf*th+c0f)}°F)  | Low: {tl}°C ({round(cf*tl+c0f)}°F)"'
return f'embed {t} {weather} {wind} {temp}' # -f "Debug|{r}"'
</drac2>