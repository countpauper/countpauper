!alias measure
<drac2>
arg="%*%"
count = 5000
result={}
for _ in range(count):
	if vr:=vroll(arg):
		result[vr.total]=result.get(vr.total,0)+1
vmax=max(result.keys())
vmin=min(result.keys())
rolled=sum(result.values())
avg=sum([v*q for v,q in result.items()])/rolled
result_list=[f'`{v}`\t{100.0*result.get(v,0)/rolled:.1f}%' for v in range(vmin,vmax+1)]
sep='\n'
return f'echo {arg} : [{vmin}...{vmax}] ~{avg:.1f} {sep.join(result_list)}'
</drac2>
