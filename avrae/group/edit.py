<drac2>
args="&*&"

if not args:
	return f'help {ctx.alias} edit -here'
c=combat()
if not c:
	return f'echo The channel needs to be in initiaive. Use `{ctx.prefix}{ctx.alias}` begin or `{ctx.prefix}i begin`'
args=args.split('-',maxsplit=1)
groupname=args[0].strip('"\' ')

# parse notes and options
args=argparse('-'+args[1]) if len(args)>1 else argparse('')
group=c.get_combatant(groupname)
if not group:
	return f'echo No group named `{groupname}` found.'

# existing notes
notes=[n.strip() for n in group.note.split(',')] if group.note else []

# add or replace fixed args (insert in reverse order)
fixed_fields={'dm':'DM', 'l':'Location','tz':'TZ',}
fixed_args=list(fixed_fields.keys())
fixed_args.reverse()
for a in fixed_args:
	if note:=args.last(a,type_=str):
		prefix=f'{fixed_fields[a]}:'
		notes = [n for n in notes if not n.startswith(prefix)]
		if note.lower()!='none':
			notes.insert(0,f'{prefix} {note}')

# add general notes
for note in args.get('n',type_=str)+args.get('note',type_=str):
	note=note.strip()
	if note.lower()=='none':
		# clean out generic notes
		notes=[n for n in notes if any(n.startswith(prefix) for prefix in fixed_fields.values())]
	elif note:
		notes.append(note)

if notes:
	notes=f'{", ".join(notes)}'
else:
	notes=''

return f'i note "{groupname}" {notes}'
</drac2>
