!alias date embed <drac2>
G=get('DateGvar',"1665221a-908a-40e9-ac8b-24ce938dfe09")
g,n=load_json(get_gvar(G)),'\n'
h,C='&1&'in'help?'and not'&1&'=='p','&1&'in'custom'
H=get_gvar('75ff3ec8-9b12-4adf-b8a8-d55565d57c38').split('##############')
if C:
	return f'-title "Wait, what multiverse is this?" -desc "{H[1]}"'
if h:
	return f'-title "Wait, what day is it?" -desc "{H[0]}" -f "Calendar Source|{G}"'
vn=f'Date_{ctx.guild.id}' if ctx.guild else 'Date'
dy=get(vn,'1')
dy=dy if'&1&'=='&1'+'&'else f'{dy}+1' if'&1&'in'next'else f'{dy}-1' if'&1&'in'prev'else '&2&' if'&2&'!='&2'+'&'and'&1&'in'set'else '&1&' if not(h or C)else '1'
dy=roll(dy)
M=[i.length for i in g.months]
N=[i.name for i in g.months]
mth,dy=1,(dy if dy<=g.length else dy%g.length)
set_uvar(vn, str(dy)) if '%1%' in 'setnextprev' else ''
D=dy
for x in range(len(M)):
	if mth>x and dy>M[x]:
		dy-=M[x]
		mth+=1
mth=mth-1
hldy=[i.name.replace("'","ʼ")for i in g.months[mth].holidays if int(i.day)==dy]if g.months[mth].holidays else[]
mn=(' | 'if sum([1 for i in[""]+range(20)if f"moon{i}"in g])<4 else n).join([f"""{f'''**{g.get(f'moonName{i}')}** '''if g.get(f'moonName{i}')else''}{g.get(f'moon{i}')[int(str(round((D-1)/(g.get(f'moonCycle{i}')),0))[:-2])%len(g.get(f'moon{i}'))]}"""for i in[""]+range(20)if f"moonCycle{i}"in g])
z=int(str(dy)[-1:])
sfx='th'if 14>int(dy)>10 else'st'if z==1 else'nd'if z==2 else'rd'if z==3 else'th'
wd=g.weekdays[(D-1)%len(g.weekdays)] if 'weekdays' in g else ''
season=[x.name for x in g.seasons if x.start<=D<=x.end][0]
hldy=f'-f "Holiday_{" "*30}_|{(", ".join(hldy))}"' if hldy else ''
wd=f'{wd}, ' if wd else ''
year=f", {year}" if exists('year') else ''
dy=f"{dy}{sfx} day of " if g.months[mth].length != 1 else ''
mn=f'-f "Moon Phase|{mn}|inline"' if mn else ''
season=f' -f "Season|{season}|inline"'
return f"""-title "{g.name}" -desc "{wd}{dy}{N[mth]}{year}" {mn} {season} {hldy} -footer "{D}/{g.length} - {ctx.prefix}{ctx.alias} help" """
</drac2>
