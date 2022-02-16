<drac2>
C=combat()
if not C:
	return f'echo Can only {ctx.alias} the bonus in combat'
mdb_key = 'bonus'
mdb = load_json(C.get_metadata(mdb_key,'[]'))
mdk_key = 'kills'
mdk = load_json(C.get_metadata(mdk_key,'[]'))
C.set_metadata('bonus_undo',dump_json(dict(kills=mdk, bonus=mdb)))
C.delete_metadata(mdb_key)
C.delete_metadata(mdk_key)
return f'echo Bonus data cleared'
</drac2>