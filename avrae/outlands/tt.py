embed <drac2>
a=&ARGS& if len(&ARGS&)>1 else &ARGS&+["&2&"] if len(&ARGS&)==1 else ["&1&","&2&"]
c,t,r,u,l,h=character(),"timetrack","Weekly Roleplay XP","ttchar",1586044800,get_gvar("23f20593-d7d6-460f-b7c0-95a28b895bd7")
ue,wk=uvar_exists(u),int(floor(((time()-l)/604800)))
lo,ulo,nh=a[0]=="lock",a[0]=="unlock",a[0]!="help" and a[0]!="?"
if lo:
	set_uvar(u,c.upstream)
elif ulo:
	delete_uvar(u)
else:
	wc=ue and get(u)!=c.name and get(u)!=c.upstream
	if not wc:
		ex=c.cc_exists(t)
		su=not ex
		if not c.cc_exists(r):
			c.create_cc(r,0,1,None,'bubble')
			c.set_cc(r,0)
		if su:
			c.create_cc(t,0)
			c.set_cc(t,80)
			c.set_cvar_nx("last_reset",wk)
		mi=c.get_cc_max(t)==240
		if mi:
			temp=c.get_cc(t)
			c.create_cc(t,0)
			c.set_cc(t,temp)
		ud = int(get("last_reset",0))
		if (wk>ud) and ex:
			hr=(wk-ud)*80
		else:
			ud=None
			hr=0
		sp=min([int(a[0]),-1*int(a[0])]) if a[0].strip("-").isdigit() else min([int(a[1]),-1*int(a[1])]) if str(a[0]+a[1]).strip("-").isdigit() else int(a[0]) if a[0].strip("+").isdigit() else int(a[1]) if a[0]=="+" and a[1].isdigit() else ""
		st=a[0]=="set" and ex and a[1].isdigit()
		if ud:
			c.set_cc(t, hr + c.get_cc(t))
			c.set_cvar("last_reset",wk)
			c.set_cc(r,0)
		if (sp and ex):
			c.mod_cc(t,sp,strict=True)
		if st:
			c.set_cc(t,int(a[1]),strict=True)
	elif nh:
		err("You're using the wrong character! Switch to your character on this server, or use `!tt unlock` to unlock the alias.")
</drac2>
-title "{{"The modrons resume normal activity." if ulo else "The modrons focus on "+c.name+"." if lo else "The modrons are here to help!" if not nh else "The modrons have migrated!" if mi else f"The modrons begin watching {name}." if su else "The modrons adjust their clocks!" if st else f"The modrons grant {name} more time!" if ud else "The modrons are here to help!" if not sp and a[0]!="&1&" else f"The modrons are watching {name}."}}"
-desc "{{"This alias is no longer locked to one character." if ulo else "Usage of this alias is now locked to "+c.name+"." if lo else h if not nh or a[0]!="&1&" and not(sp or su or st) else (f"\nYou gain {hr} hours.\nWeekly Roleplay XP reset." if ud else "")+(f"\nYou spend {-1*sp} hours." if sp and sp < 0 else "")+(f"\nYou gain {sp} hours." if sp and sp > 0 else "")+f"\n\nYou have **{c.get_cc(t)} hours** remaining.\n{r}: {c.cc_str(r)}" if ex and nh else h}}"
-thumb "https://cdn.discordapp.com/attachments/813353462263578624/813506071548264468/Y8xQBTR.png"
-footer "{{f"For assistance, use !timetrack help\nThe Outlands have existed for {wk} weeks."}}"