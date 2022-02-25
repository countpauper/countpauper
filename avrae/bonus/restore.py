<drac2>
C=combat()
if not C:
	return f'echo Can only {ctx.alias} the bonus in combat'
# get current state
backup_key='bonus_backup'
backup=load_json(get(backup_key,'{}'))
channel_key='''&*&''' or str(ctx.channel.id)
db=backup.get(channel_key)
if not db:
	return f'echo No backup for {channel_key}.'

current=dict()
for key,data in db.items():
	current[key]=load_json(C.get_metadata(key))
	C.set_metadata(key,dump_json(data))
C.set_metadata('bonus_undo',dump_json(current))

return f'echo Bonus data restored.'
</drac2>