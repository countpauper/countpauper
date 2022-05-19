embed <drac2>
ch=character()
ch.set_cvar_nx("pitrolls","ath, acro, con")
cmd = f'{ctx.prefix}{ctx.alias}'
if a:='''&*&''':
	r=(a.split(' ')+["ath","con","acro"])[:3]
	ch.set_cvar("pitrolls",", ".join(r))
	title=name+" sets their rolls!"
else:
	title=name+" checks their rolls!"
text="Rolls|"+"\n".join(pitrolls.split(", ",2))
</drac2>
-title "{{title}}"
-f "{{text}}"
-thumb {{image}}
-footer "{{cmd}} help"


