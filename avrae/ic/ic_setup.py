<drac2>
sv='ic_settings'
settings=load_json(get_svar(sv,'{}'))
args=argparse('&*&')

definition={
	"footer":dict(reset=['false','none','reset','','off']),
#	"xp":dict(reset=['0','none','']),
	"location":dict(reset=['false','none','reset','','off']),
	"channels": dict(reset=['none'],list=True)
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
			if syntax.get('list',False):
				oldvals=settings.get(key,[])
				newvals=[]
				for v in val.split(","):
					if v.startswith('+'):
						newvals.append(v[1:])
					elif v.startswith('-'):
						v=v[1:]
						if v in oldvals:
							oldvals.remove(v)
					else:
						oldvals=[]
						newvals.append(v)
				settings[key]=oldvals+newvals
			else:
				settings[key]=val

return f'svar {sv} {dump_json(settings)}'
</drac2>