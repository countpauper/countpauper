!alias eredh <drac2>
charname='eredh'
if name.lower()!=charname:
	return f'char {charname}'

args="&*&"
roster=load_json(get('userRoster','[]'))
roster=[user for user in roster if user['n'].lower()==name.lower()]
selection={user['d']:user['i'] for user in roster if user['d'].lower().startswith(args)}
if not selection or not args:
	return f'echo {", ".join(user["d"] for user in roster)}'
selected=list(selection.keys())[0]
img=selection.get(selected)
character().set_cvar('sayActive',selected)
return f'portrait update {img}'
</drac2>