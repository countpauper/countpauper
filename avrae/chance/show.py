<drac2>
var='Chance'
key='show'
value=True

ch=character()
args=&ARGS&
config = load_json(ch.cvars.get(var, '{}'))
if args:
	shown=config.get(key,False)
	if typeof(shown)=='bool':
		shown={'*':shown}
	if typeof(shown)=='str':
		shown = shown.split(',')
	if typeof(shown)=='SafeList':
		shown={id:True for id in shown}
	for a in args:
		shown[a]=value
	config[key] = shown
	set_uvar(var, dump_json(config))
	return f'echo {"Showing" if value else "Hiding"} chance % for {", ".join(args)}'
else:
	config[key]=value
	ch.set_cvar(var,dump_json(config))
	return f'echo {"Showing" if value else "Hiding"} chance % for {ch.name}'
</drac2>