<drac2>
C=combat()
if not C:
	return f'echo Can only {ctx.alias} the bonus in combat'
# get current state
keys={'bonus','kills'}
current=dict()
for key in keys:
	if data:=C.get_metadata(key):
		current[key] = load_json(data)

uv='bonus_undo'
undo = load_json(get(uv,'{}'))

# swap the undo and the current state
for key in current.keys():
	C.delete_metadata(key)

set_uvar(uv, dump_json(current))
for key, data in undo.items():
	C.set_metadata(key,dump_json(data))

return f'echo Bonus data restored.'
</drac2>