!alias bodt embed <drac2>
thumb=None
a="&*&"
g=load_json(get_gvar("c5b635c9-47cb-4ad8-b43d-8542640a49b3"))
if not a:
	return f'-title "**Downtime Activities**" -desc "{g[0].gend}"'
g=g[1:]
b=[act for act in g if (a.lower() in act.name.lower() or all([x in act.name.lower() for x in a.lower().split()])) ]
if not b:
	return 'f-desc "No matches found." '
if len(b)==1:
	c=b[0]
else:
	c=([act for act in g if a.lower()==act.name.lower()]+[None])[0]
if len(b)>1 and not c:
	B = "\n".join([act.name for act in b])
	return f'-title "Downtime Activities Containing: `{a}`" -desc "{B}" '

thumb =c.get('thum')
nl='\n'
if c.desc:
	# split description into fields to beat limits
	lines=c.desc.splitlines()
	header=None
	desc=""
	sections={}
	for l in lines:
		if l[:2]=='**' and l[-2:]=='**':
			header=l[2:-2]
			sections[header]=""
		elif header:
			sections[header]+=l+nl
		else:
			desc+=l+nl
	fields=[f'-f "{h}|{b[:1000]}"' for h,b in sections.items()]
	return f'-title "Downtime Activity: {c.name}" -desc "{desc[:2000]}" {" ".join(fields)} '
if c.iurl:
	return f'-title "Downtime Activity: {c.name}" -image "{c.iurl}" '
else:
	return f'-title "Downtime Activity: {c.name}" -desc "No information" '

</drac2>
{{ f'-thumb {thumb or "https://cdn.discordapp.com/attachments/813353462263578624/813362702293925908/w8V9tXW.png"} '}}
-color <color>
