!alias gems embed
<drac2>
args=&ARGS&
gp = int(args[0]) if args else (5*int(1.5**randint(32)))
min_gp=gp*0.8
max_gp=gp*1.2
data = load_json(get_gvar("228489aa-20bb-4568-83e3-4b5bb0b43ad9"))
gems, qualities, sizes = data["gems"], data["qualities"], data["sizes"]
ret=[]
iterations=100
total=0
while total<min_gp:
	iterations-=1
	if iterations<=0:
		break
	gem = list(gems.keys())[randint(len(gems))]
	size = list(sizes.keys())[randint(len(sizes))]
	quality  = list(qualities.keys())[randint(len(sizes))] if randint(4)==0 else "%s"
	gem_cost=gems[gem] * sizes[size] * gems[gem]

	max_gems = int((max_gp - total) / gem_cost)
	if max_gems<=0 or max_gems>100:
		continue
	else:
		amount=1+randint(max_gems)
	total+=amount*gem_cost
	ret.append(f'{amount}x {size % (quality % gem)} ({gem_cost}gp)')
</drac2> 
-title Gems -desc "{{ '\n'.join(ret) }}" -f "Value|{{total}}/{{gp}}gp"