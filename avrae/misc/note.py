!alias note <drac2>
args='''&*&'''
cv='notes'
notes=load_json(get(cv,'{}'))
cmd=f'embed -title Notes -color {color} -footer "{ctx.prefix}{ctx.alias} [<topic> [-/+ <note>]]"'
nl='\n'
if not args:
	if not notes:
		return f'{cmd} -desc "None"'
	else:
		return f'{cmd} -desc "**Categories:**\n{nl.join(f"{topic} [{len(n)}]" for topic,n in notes.items())}"'
# TODO add/remove more notes at once by splitting multi and not a list
if '+' in args:
	topic, note=(a.strip() for a in args.split('+',1))
	add=True
elif '-' in args:
	topic, note=(a.strip() for a in args.split('-',1))
	add=False
else:
	topic=args
	note=None

topics={c:ns for c,ns in notes.items() if topic.lower()==c.lower()}
if not topics:
	topics = {c:ns for c, ns in notes.items() if topic.lower() in c.lower()}

if note is None:
	fields=[f'-f "{c}|{nl.join(n for n in ns)}"' for c,ns in topics.items()]
	if not fields:
		fields=[f'-f "{topic}|*Unknown topic*"']
	return f'{cmd} {nl.join(fields)}'

if add:
	match = (list(topics.keys()) + [topic])[0]
	topics[match] = topics.get(match,[]) + [note]
	# TODO: show the whole list with added in cursive
	desc = f'**Added to {match}:**\n{note}'
else:
	modified=[]
	for t, candidates in topics.items():
		if modified := [n for n in candidates if note.lower() in n.lower()]:
			break
	if modified:
		match = t
		if len(modified) == len(candidates):
			notes.pop(match)
			topics={}
		else:
			topics[match] = [n for n in candidates if n not in modified]
		# TODO: show the whole list with removed crossed out
		desc=f'**Removed from {match}:**\n{nl.join(modified)}'
	else:
		return f'echo `{note}` not found in `{topic}`.'

notes.update(topics)
character().set_cvar(cv, dump_json(notes))
return f'{cmd} -desc "{desc}"'
</drac2>