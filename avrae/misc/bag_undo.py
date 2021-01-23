<drac2>
bag_var='bags'
backup_var='bag_backup'
if backup:= get(backup_var,None):
	character().set_cvar(bag_var,backup)
	return f'echo Bag restored.'
else:
	return f'echo No bag backup was available.'
</drac2>