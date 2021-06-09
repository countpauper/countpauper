<drac2>
args="""&*&""".strip('"\' ')

if not args:
	return f'help {ctx.alias} destroy -here'

return f'i remove {args}'
</drac2>
