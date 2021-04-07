<drac2>
args="&*&"

if not args:
	return f'help {ctx.alias} create -here'
if not combat():
	return f'echo The channel needs to be in initiative. Use `{ctx.prefix}{ctx.alias}` begin or `{ctx.prefix}i begin`'
args=args.split('-',maxsplit=1)
name=args[0]

# parse notes and options
args=argparse('-'+args[1]) if len(args)>1 else argparse('')
notes=[]
# fixed fields should match edit
fixed_fields={'dm':'DM', 'l':'Location','tz':'TZ'}
for arg,prefix in fixed_fields.items():
	if note:=args.last(arg,type_=str):
		notes.append(f'{prefix}: {note}')

notes+=args.get('n',type_=str)
notes+=args.get('note',type_=str)

if notes:
	sep=', '
	notes=f'-note "{sep.join(notes)}"'
else:
	notes=''

return f'i add 0 "{name}" -h {notes}'
</drac2>
