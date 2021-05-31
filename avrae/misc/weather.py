!alias weather <drac2>
args=&ARGS&
x=args[0] if args else get('biome','list')
if v:=(x not in "list"):
	set_uvar('biome',x)
n="\n"
gvar=get_gvar('bdd2389e-9277-4715-b42e-84e4cdef3e54').split(n)
h=get_gvar(gvar[0])
f=h.lower().find(x.lower()) if v else -1
f2=-1 if f<0 else 0 if f==0 else f if h[f]==n else 1+h.rfind(n,0,f) if f>0 else -1
dl=h[f2:-1] if f!=-1 else h
x2=dl.split(n)[0]

if f==-1:
	return f'embed -title "Supported Biomes" -desc "{n.join(h.split(n)[::5])}" -footer "{ctx.prefix}{ctx.alias} [biome] [season]"'
v=f!=-1
dv=f'Date_{ctx.guild.id}' if ctx.guild else 'Date'
z=int(get(dv,randint(366)))%365
y=1 if "%2%"=="%2"+"%" else 0
p='wi' if z>=355 or z<=79 and y else 'sp' if 80<=z<=171 and y else 'su' if 172<=z<=263 and y else 'fa' if 264<=z<= 354 and y else "%2%"
s=1 if p in "spring" else 2 if p in "summer" else 3 if p in "fallautumn" else 0
var=dl.split(n)[1].split()
r1=dl.split(n)[2].split()
w1=dl.split(n)[3].split()
t=dl.split(n)[4].split()
th=roll(str(int(t[s]))+"+"+var[0])
tl=roll(str(int(t[s]))+"-"+var[0])
r=roll("1d20+"+r1[s])
w=max(0,roll(var[1]+"-"+var[1]+"+"+w1[s]))
wd=roll("1d8")
dir=get_gvar(gvar[1]).split(n)
c=get_gvar(gvar[2]).split(n)
s=["Winter","Spring","Summmer","Fall"][s]
bft=[0,1,4,8,13,19,25,32,39,47,55,64,72]
bft=min(idx for idx,mpb in enumerate(bft) if mpb>=w)
t=f'-title "Today\'s Weather:  {x2}, {s}"'
[21]
if tl<33 and r>21:
	weather=0 if r<27 else 1
else:
	weather=max(idx for idx,rt in enumerate([99,99,0,16,17,19,20,21,23,28]) if r>=rt)

weather=f'-f "Weather|{c[weather]}"'
wind=f'-f "Wind|💨 {w} mph ({round(0.447*w)} m/s) ({bft} bft), {(dir[wd-1] if w!=0 else "Still")}"'
temp=f'-f "Temperature|🌡️ High: {round((th-32)/(9/5))}°C ({th}°F)  | Low: {round((tl-32)/(9/5))}°C ({tl}°F)"'
return f'embed {t} {weather} {wind} {temp}' # -f "Debug|x{x} f{f} f2{f2} {h}"'
</drac2>