!alias wildshape <drac2>
p=ctx.prefix
A=int(get("DruidLevel","0"))==20
G=load_json(get_gvar("4be574e3-e431-4666-a9be-3447018d3ba4"))
g=load_json(get_gvar(G[0]))
a=" ".join(&ARGS&).title()
q="Wild Shape"
c=combat()
f=f'-footer "{p}{ctx.alias} help for more information | Developed by @'+'silverbass#2407"'
m=0 if"end"in a.lower()else -1 if "help" in a.lower()or not a else 2 if"element"in a.lower() else 1
v=A or (cc_exists(q)and get_cc(q)>0 and a)
cf=[x.name for x in c.combatants if name in x.name and name!=x.name]if c and c.combatants else""
cf=cf[0] if cf else ""
dt=max(0,-(c.get_combatant(cf).hp)if c and cf and c.get_combatant(cf) else 0)
TODO: first command get_gvar(G[1]).replace('!ws',f'{p}{ctx.alias}')+f" -color {color} -thumb {image} {f}" if m<0 else "multiline"
v=v if m>0 else v
mod_cc(q,0 if A else -m)if v and m>0 else""
</drac2>
{{f'{p}embed -title "{name} {"transforms into a "+a.title() if v else "tries to transform"}{" with Elemental Wild Shape" if m>1 else ""}!" {f} -desc "{"" if v else "You do not have this ability" if not cc_exists(q) else G[2] if not a else G[3]}" -f "{q}|{"Unlimited" if A else cc_str(q) if cc_exists(q) else "None"}" -thumb {image} -color {color}' if m>0 else f'{p}embed {f} -title "{name} ends their Wild Shape" -f "Damage Transfered|{dt}" -thumb {image} -color {color}' if m==0 else ""}}
{{f'{p}i opt "{name}" -group "{name+" (Wild Shape)"}"\n{p}i madd "{a}" -name "{a.title()} ({name})" -h -group "{name+" (Wild Shape)"}" ' if v and m>0 and c else  ""}}
{{f'{p}i remove {cf}\n{p}i opt "{name}" -group None\n{p}i hp "{name}" -{dt}' if m==0 and c and cf else ""}}