<drac2>
sv='ic_settings'
settings=load_json(get_svar(sv,'{}'))
args=argparse('&*&')
footer_key='footer'
if footer:=args.last(footer_key):
	if footer.lower() not in ['false','none','reset','','off']:
		settings[footer_key]=footer
	elif footer_key in settings:
		settings.pop(footer_key)

xp_key='xp'
if (xp:=args.last(xp_key)) is not None:
	if xp:
		settings[xp_key]=xp
	elif xp_key in settings:
		settings.pop(xp_key)

return f'svar {sv} {dump_json(settings)}'
</drac2>