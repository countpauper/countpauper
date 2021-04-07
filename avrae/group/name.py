<drac2>
args="&*&"
args=args.split(maxsplit=1)
if len(args)!=2:
	return f'help {ctx.alias} name -here'
return f'i opt {args[0]} -name "{args[1]}"'
</drac2>