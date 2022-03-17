<drac2>
bag_var='bags'
backup_var='bag_backup'
if backup_str:= get(backup_var,None):
	restored='Bag'
	backup = load_json(backup_str)
	# undo changes to coinpurse if any
	config = load_json(get_gvar(get_svar('quickbag', '71ff40cc-4c5f-4ae5-bdb6-32130f869090')))
	purse_name = config.get('purse', 'coinpurse').lower()
	if purse:=([b for b in backup if b[0].lower()==purse_name]+[None])[0]:
		backup.remove(purse)
		character().coinpurse.set_coins(purse[1].get('pp',0), purse[1].get('gp',0), purse[1].get('ep',0), purse[1].get('sp',0), purse[1].get('cp',0))
		restored+=' and Coinpurse'
	# apply the remaining bags
	character().set_cvar(bag_var,dump_json(backup))
	return f'echo {restored} restored for {name} .'
else:
	return f'echo No bag backup was available for {name}.'
</drac2>