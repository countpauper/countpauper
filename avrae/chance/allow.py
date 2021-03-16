<drac2>
var='Chance'
key='allowed'
value=True

config=load_json(get_svar(var,'{}'))

if args:=&ARGS&:
	allowed=config.get(key,True)
	if typeof(allowed)=='bool':
		allowed={'*':allowed}
	if typeof(allowed)=='str':
		allowed = allowed.split(',')
	if typeof(allowed)=='SafeList':
		allowed={id:True for id in allowed}
	for a in args:
		allowed[a]=value
else:
	allowed=value
config[key]=allowed
return f'svar {var} {dump_json(config)}'
</drac2>