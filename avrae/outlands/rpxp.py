embed <drac2>
a = &ARGS&[0].lower() == "gain" if &ARGS& else ""
cc = "Weekly Roleplay XP"
h = get_gvar("2d3f5e90-7fbf-4d50-8f2f-2231f9e7760f")
c = character()
if not c.cc_exists(cc):
	c.create_cc(cc, 0, 1, None, 'bubble')
	c.set_cc(cc, 0)
if c.get_cc(cc) == 0 and a:
	gain = True
	total=int(float(get('xp',0)))
	xplog=load_yaml(get('xplog','{}'))
	if not all(typeof(k)=='int' for k in xplog.keys()):
		err("Your xp log is using the old format. Please run `!xp`")
	total +=  level*100
	c.set_cc(cc, 1)
	c.set_cvar('xp',str(total))

	time_2020 = 26297280
	time_Minute=int(time()/60)-time_2020
	xplog.update({time_Minute:f"{level*100} Weekly Roleplay XP"})
#	listed=[f'**{x[:x.index(" ") if (" " in x and "".join(xplog).count(x[:x.index(" ")])<2) else None]}:** {xplog[x]}' for x in xplog]
#	pages=['' for x in range(20)]
#	page=0
#	for x in listed:
#		page=page+1 if len(pages[page]+x+'\n')>2000 else page
	#	pages[page]=pages[page]+x+'\n'
	#pages=[x for x in pages if x] or ["No entries found."]
	#calledPage=gain-1 if gain else len(pages)-1
	c.set_cvar('xplog',dump_yaml(xplog))
	totals=[0,300,900,2700,6500,14000,23000,34000,48000,64000,85000,100000,120000,140000,165000,195000,225000,265000,305000,355000]
	next=min(20,level+1)
	nexttotal=totals[next-1] if level<20 and next<21 else 0
	required=totals[min(20,level)-1]
	ding=total>=totals[level] if level<20 else 0
else:
	gain = False
</drac2>
-title "{{f'{name} gains {level*100} Weekly Roleplay Experience'+(f" and levels up to {next}{'nd'*(next==2) or 'rd'*(next==3) or 'th'} level"*ding)+'!' if gain else f'{name} has already gained roleplay experience this week!' if a else 'Weekly Roleplay XP'}}"
-desc "{{f'Your experience total needs to be at least {required:,} to match your current level!' if gain and total<totals[min(20,level)-1] else f"Don't forget to update your sheet with your proper experience total and level and `!update`." if gain and level<20 and total>=(totals[min(19,level)]) else "" if gain else f'**{cc}**: {c.cc_str(cc)}\n\nYour Weekly Roleplay XP counter will automatically reset the next time you gain hours via the `!timetrack` command, freeing you up to collect more roleplay experience.' if a else h}}"
{{f'''-f "{cc}: {c.cc_str(cc)} (+1)|** **"''' if gain else ''}}
{{f'''-f "Current Experience| {total:,}"''' if gain else ''}}
{{f'-f "Next Level: {next}|{nexttotal:,} *({nexttotal-total:,} remaining)*"' if gain else ""}}
-color <color> -thumb <image>