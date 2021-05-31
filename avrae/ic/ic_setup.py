<drac2>
sv='ic_settings'
settings=load_json(get_svar(sv,'{}'))
args=argparse('&*&')

definition={
	"footer":dict(reset=['false','none','reset','','off']),
	"xp":dict(reset=['0','none','']),
	"location":dict(reset=['false','none','reset','','off'])
}

# iterate over defined settings and parse them according to their syntax
for arg,syntax in definition.items():
	if (val:=(args.get(arg)+[None])[0]) is not None:
		key=syntax.get('setting',arg)
		if (reset:=syntax.get('reset')) and val.lower() in reset:
			settings.pop(key)
		elif not val:	# no reset defined, falsey is reset
			setting.pop(key)
		else:	# not reset, set
			settings[key]=val

return f'svar {sv} {dump_json(settings)}'
</drac2>