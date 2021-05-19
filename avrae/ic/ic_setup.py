<drac2>
sv='ic_settings'
settings=load_json(get_svar(sv,'{}'))
args=argparse('&*&')
footer_key='footer'
if footer:=args.last(footer_key):
	if footer.lower() in ['false','none','reset','']:
		if footer_key in settings:
			settings.pop(footer_key)
	else:
		settings[footer_key]=footer
return f'svar {sv} {dump_json(settings)}'
</drac2>