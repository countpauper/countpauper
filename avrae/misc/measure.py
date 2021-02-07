!alias measure
<drac2>
arg="%*%"
count = 5000
result={}
for _ in range(count):
	if vr:=vroll(arg):
		result[vr.total]=result.get(vr.total,0)+1
vmax=max(result.keys())
rolled=sum(result.values())
result_list=[f'{100.0*result.get(v,0)/rolled:.1f}%' for v in range(vmax+1)]
return f'echo {arg} : {", ".join(result_list)}'
</drac2>
