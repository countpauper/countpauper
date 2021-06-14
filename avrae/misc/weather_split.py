TODO: fahrnheit to kelvin for th and tl, 255+(t*5/9) but only for offsets, variation do by hand, about a half die size and half offset


!test <drac2>
h=get_gvar('49efccd8-7fe3-471a-b58e-b489587949b9').split()
w=dict()
for idx in range(0,len(h),15):
	w[h[idx]]=dict(weather=[f'1d20+{i}' for i in h[idx+3:idx+7]], th=[f'{255+(int(i)*5)//9}+{h[idx+1]}' for i in h[idx+11:idx+15]], tl=[f'{255+(int(i)*5)//9}-{h[idx+1]}' for i in h[idx+11:idx+15]], wind=[f'{h[idx+2]}-{h[idx+2]}+{i}' for i in h[idx+7:idx+11]])
character().set_cvar('weather',dump_json(w))
</drac2>


!test <drac2>
b=load_json(get_gvar('16b912b6-817a-4923-9f66-f765bd69f346')).biome
o=[]
for n,p in b.items():
	o.append(f"""{n}:"th":{dump_json([f'{int(t.split("+")[0])-273}+{t.split("+")[1]}' for t in p.th])},\n   "tl":{dump_json([f'{int(t.split("-")[0])-273}+{t.split("-")[1]}' for t in p.tl])},""")
return "\n".join(o)
</drac2>