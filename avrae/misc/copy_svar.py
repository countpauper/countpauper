!alias tvar <drac2>
args=&ARGS&
if len(args)!=2:
	return f'echo Use: {ctx.prefix}{ctx.alias} <svar> <gvar guid>'
val= get_gvar(args[1])
if len(val)>=10000:
	err('svar size limit (10000) exceeded')

return f'svar {args[0]} {val} '
</drac2>