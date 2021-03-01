<drac2>
args=&ARGS&
roster_var='userRoster'
roster = load_json(get(roster_var, '[]'))
if not args:
	# Temporarily show the whole roster in a table embed
	active = get('sayActive', name).lower()
	header=f'`   {"Id":<12} {"Name":<24} {"Color":<10} Thumbnail`\n'
	chars=[f'`{"* " if u.d.lower()==active else "  "} {u.d:<12} {u.n:<24} {u.c:<10}` [link]({u.i})' for u in roster]
	nl='\n'
	return f'embed -title Characters -desc "{header}{nl.join(chars)}" -t 8 -footer "* active"'
else:
	# Select character with first argument
	select=args[0].lower()
	switch=True
	if select[0]=='-':
		select=get('sayActive',name).lower()
		switch=False

	args = argparse(args)
	match = [u for u in roster if u.d.lower() == select]
	match += [u for u in roster if u.n.lower() == select]
	match += [u for u in roster if u.d.lower().startswith(select)]
	match += [u for u in roster if u.n.lower().startswith(select)]

	# add new character if unknown
	new=False
	if not match:	# create a new character
		roster.append(dict(d=select, n=name, i=image, c=get('color',randint(1<<24)) ))
		selected=roster[-1]
		new=True
	else:
		selected=match[0]

	# Apply changes
	changes=[]
	if n:=args.last('name'):
		selected['n']=n
		changes.append(f'**Name:** {n}')
	if c:=args.last('color'):
		palette = load_json(get_gvar('74f3bd10-7163-4838-ad27-344ad0fb6b83'))
		hex_digits='0123456789abcdefABCDEF'
		if any(color_char not in hex_digits for color_char in c) and c not in palette.keys():
			err(f'Unknown color `{c}')
		selected['c']=c
		changes.append(f'**Color:** {c}')
	if i:=args.last('thumb'):
		selected['i']=i
		changes.append(f'**Thumb:** {i}')

	# apply
	if new or changes:
		set_uvar(roster_var,dump_json(roster))
	character().set_cvar('sayActive',selected.d)

	# Report
	change_str=' '.join(changes)
	if new:
		return f'techo {2+len(changes)} {ctx.alias} **New** character {selected.d} selected {change_str}'
	elif switch:
		return f'techo {1+len(changes)} {ctx.alias} active character set to `{selected.d}` {change_str}'
	elif changes:
		return f'techo {1+len(changes)} {ctx.alias} character `{selected.d}` modified to  {change_str}'
	else:
		return f'echo Use `{ctx.prefix}{ctx.alias} set [<id>/"<name>"] [-name "<name>"] [-color <color>|<rgb>] [-thumb <url>]`'
</drac2>