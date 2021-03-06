<drac2>
roster_var='userRoster'
args=&ARGS&
roster = load_json(get(roster_var, '[]'))

# TODO args = category

# show the whole roster in a table embed
active = get('sayActive', name).lower()
header=f'`   {"Id":<12} {"Name":<24} {"Color":<10} Thumbnail`\n'
# TODO: group by category
chars=[f'`{"* " if u.d.lower()==active else "  "} {u.d:<12} {u.n:<24} {u.c:<10}` {f"[link]({u.i})" if u.i else "[none]"}' for u in roster]
nl='\n'
return f'embed -title Characters -desc "{header}{nl.join(chars)}" -footer "* active"'
</drac2>
