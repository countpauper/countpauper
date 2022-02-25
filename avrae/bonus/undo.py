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

undo_key='bonus_undo'
undo = load_json(C.get_metadata(undo_key) or "{}")

# swap the undo and the current state
for key in current.keys():
	C.delete_metadata(key)

C.set_metadata(undo_key,dump_json(current))
for key, data in undo.items():
	C.set_metadata(key,dump_json(data))

# Also update the backup
backup_var='bonus_backup'
backup=load_json(get(backup_var,"{}"))
backup[str(ctx.channel.id)]=undo
set_uvar(backup_var, dump_json(backup))

return f'echo Bonus data restored.'
</drac2>