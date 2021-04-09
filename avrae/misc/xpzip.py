!alias xpzip tembed <drac2>
# create a backup but don't overwrite an existing backlog
backup_var='xplog_backup'
xp_var='xplog'
if not exists(backup_var) and exists(xp_var):
	character().set_cvar(backup_var,get(xp_var))

# get the current xp log var, fall back to beta test gvar if deleted
xp_in=load_json(get(xp_var, get_gvar('1975c6cb-b625-41e3-b827-df8560eab9b5')))

# interpret the log
xp={}	# date: int(xp) (comment discarded)
for d,s in xp_in.items():
	idx=min(i for i,c in enumerate(s+"|") if c not in '+- 0123456789')	# split integer part
	xp[d]=int(s[:idx].replace(' ',''))

# zip the log, v1: to a single entry
xp_out={list(xp_in.keys())[0] : f'{sum(xp.values())} = {"+".join(str(v) for v in xp.values())}'}

# persist
character().set_cvar(xp_var,dump_json(xp_out))
</drac2>
{{get_gvar("d903eb12-7842-4fd9-bb00-58d3db55acdc").replace("&*" + "&", "'log'").replace("&AR" + "GS&", "['log']")}}