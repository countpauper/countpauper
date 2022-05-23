embed {{args=[x.lower() for x in &ARGS&]+['X']}}
{{cc,n,logcheck,Set,help,status,power,archetype,desc,shortdesc,gaincon,losscon,hours,setup,image,rargs="Piety",'\n',args[0] in "log",args[0] in "set",args[0] in "help?",args[0] in "status",args[0] in "power",args[0] in "archetype",args[0] == "desc",args[0] == "shortdesc",args[0] in "gaincon",args[0] in "losscon",args[0] in "hours",args[0] == "setup",args[0] == "image",&ARGS&[1:]}}
{{c=character()}}
{{c.create_cc_nx(cc)}}
<drac2>
update = power or archetype or shortdesc or gaincon or losscon or hours or image
if not exists("piety"):
 c.set_cvar("piety",get_gvar("1e6e4808-fde3-48f2-aa35-f9ff109be307"))
piety=load_json(piety)
if desc and rargs:
 piety[0]["desc"] = " ".join(rargs).replace("/","\n")
if update and rargs:
 if gaincon or losscon:
  piety[0][args[0]] = "• "+" ".join(rargs).replace("/","\n• ")
 if hours and &ARGS&[1].isdigit():
  piety[0]["hours"] = int(piety[0]["hours"])+int(&ARGS&[1])
 if hours and &ARGS&[1]=="set":
  piety[0]["hours"] = str(&ARGS&[2])
 if shortdesc:
  piety[0][args[0]] = " ".join(rargs).replace("/","\n")
 if power or archetype:
  piety[0][args[0]] = " ".join(rargs)
 if image:
  if "image" in piety[0]:
   piety[0][args[0]] = " ".join(rargs)
  else:
   c.set_cvar("pietyImg",&ARGS&[1])
c.set_cvar("piety",dump_json(piety))
piety=load_json(piety)
</drac2>
{{pi,gain=c.get_cc(cc),int(''.join([x for x in args[:3] if x.strip('-+').isdigit()] or ["0"])) if "&*&" and not (help or update) else 0}}
{{calendar=load_json(get_gvar("1aec09a0-9e25-4700-9c2d-42d79cb0163b"))}}
{{hourOffset=calendar.get('hourOffset',0)+int(get('timezone',0))}}
{{baseYear=calendar.get("yearOffset",1970)}}
{{Time=time()+(3600*hourOffset)}}
{{totalDayCount=int((Time)//86400)}}
{{yearsPassed=totalDayCount//calendar.length}}
{{numLeapYears=len([x for x in range(baseYear,baseYear+yearsPassed-4) if x//calendar.get('leapCycle',4)==x/calendar.get('leapCycle',4)]) if calendar.get('leapCycle') else 0}}
{{yearStartDay=yearsPassed*calendar.length+numLeapYears}}
{{totalDays=totalDayCount-yearStartDay}}
{{year=int((totalDayCount-numLeapYears-1)//calendar.length)+baseYear}}
{{isLeapYear=year//calendar.get('leapCycle',4)==year/calendar.get('leapCycle',4) if numLeapYears else 0}}
{{calendarDay=totalDays%(calendar.length+isLeapYear) or calendar.length+isLeapYear}}
{{hour=int(Time%86400//3600)}}
{{minute=int(Time%86400%3600//60)}}
{{second=int(Time%86400%3600%60)}}
{{monthLengths=[x.length+(isLeapYear and x.name==calendar.get('leapMonth','February')) for x in calendar.months]}}
{{day,month=calendarDay,1}}
{{[(day:=day-monthLengths[x],month:=month+1) for x in range(len(monthLengths)) if month>x and day>monthLengths[x]]}}
{{timestamp=f'{day:02}.{month:02}.{str(year)[2:]} ({hour:02}:{minute:02}:{second:02})'}}
{{pilog=load_json(get('pilog','{}'))}}
{{pilog.update({timestamp:"&*&".replace("set","Total set to",1) if "&*&"[:3]=="set" else "&*&"}) if ((Set or gain) and not (logcheck or help or update)) else ""}}
{{listed=[f'**{x[:x.index(" ") if (" " in x and "".join(pilog).count(x[:x.index(" ")])<2) else None]}:** {pilog[x]}' for x in pilog]}}
<drac2>
pages=['' for x in range(20)]
page=0
for x in listed:
 page=page+1 if len(pages[page]+x+'\n')>2000 else page
 pages[page]=pages[page]+x+'\n'
</drac2>
{{pages=[x for x in pages if x] or ["No entries found."]}}
{{calledPage=gain-1 if gain else len(pages)-1}}
{{c.set_cvar('pilog',dump_json(pilog))}}
{{totals=[3,10,25,50]}}
{{gain=0 if logcheck else gain}}
{{total=pi*(not Set)+gain if not (logcheck or help) else 0}}

-title "{{f"{name}{' sets their Piety total to'*Set or ' gains'*(gain>0) or ' loses'} {gain:,} {cc}!".replace('-','') if gain else "Outlands Piety Tracker"}}"

{{f'-desc "{pages[calledPage]+(f"{n}{n}Page {calledPage+1}/{len(pages)}{n}Use `!piety log #` to view another page." if len(pages)>1 else "") if logcheck else ""}"' if logcheck else '''-desc "**— Help —**\n\nPiety is a measure of your favor with one of the many Powers of the multiverse. This favor is not gained through faith or belief alone, but rather by way of specific actions undertaken to honor and expand the influence of the Power of your devotion. Conversely, actions that diminish or dishonor the Power lead to a loss of Piety.\n\nAt certain thresholds of Piety (3, 10, 25, 50), you may perform deeds of devotion to receive an otherworldly boon. All *mechanical* aspects of these boons are determined by one of fifteen Power Archetypes. Use `!powers` to learn about these archetypes and the boons they grant.\n\nAll aspects of the *lore* surrounding your chosen Power can be flavored as seen fit. For example, you may find that the boons granted by the Archetype of The Elements are a good fit for Talos, the Faerûnian god of storms, or you may reflavor them with a Power of your own creation in mind. Within reason, you may choose to serve any established or original Power in the multiverse.\n\nUse `!piety setup` for instructions on setting up your Power/Piety.\n\n**— Commands —**\n\n`!piety` - Displays your current Piety total.\n`!piety (-)# [notes]` - Adds (or subtracts) to (or from) your Piety total. Include notes on how you gained/lost Piety directly following the amount gained. (Do not include digits in the notes.)\n`!piety set # [notes]` - Sets your Piety to the number provided.\n`!piety setup` - Displays Power/Piety setup instructions.\n`!piety status` - Displays a range of information on your Power.\n`!piety desc` - Set and display a long-form description of your Power.\n`!piety image <url>` - Sets an image of your Power. Linked file must end in .png or .jpg and be no larger than 2MB.\n`!piety log` - Displays a log of all your Piety gains, losses, and sets.\n`!piety hours (set) #` - Adds to (or sets) the number of hours spent on the Gaining Piety downtime activity."''' if help or (pi == 0 and not (Set or gain or logcheck or setup or status or update or desc)) else '''-desc "**— Setup —**\n\n**1.** Choose an archetype and name for your Power. Use `!powers` to learn about archetypes and their boons.\n\n**2.** Write a brief description of the Power\'s nature, character, place in the multiverse, history, or other lore surrounding it.\n\n**3.** Formulate 3-4 deeds or conditions that might cause your Power to grant you favor, and another 3-4 that might cause them to rescind it. These should ideally be unambiguous and neither too broad nor too specific. E.g. \\\"Fight the forces of evil\\\" is too broad, \\\"Destroy the demon prince Orcus\\\" is too specific, but \\\"Destroy a powerful evil being\\\" is a happy medium.\n\n**4.** Post your Power\'s name, archetype, description, and piety gain/loss conditions in <#696433600467566682> and ping the <@&695505599898845215> role for approval.\n\n**5.** Once approved, use `!piety status` and follow the instructions to finish setting up the alias. These instructions may also be found as a pinned message in <#768950161099063376>.\n\n**6.** Finally, roleplay an act of devotion at your character\'s current location that accounts for their initial gaining of their Power\'s favor and gain your first point of Piety.\n\n"''' if setup else f'-desc "**— Status —**\n\n**Power**: {piety[0].power}\n\n**Archetype**: {piety[0].archetype}\n\n**Short Description**: {piety[0].shortdesc}\n\n**Piety Gain Conditions**\n{piety[0].gaincon}\n\n**Piety Loss Conditions**\n{piety[0].losscon}\n"' if (status or update) else f'-desc "**Power**: {piety[0].power}\n\n**— Description —**\n\n{piety[0].desc}"' if desc else ""}}

{{f'''-f "Log Entry Added:| **{timestamp}:** &*&"'''.replace("set","Total set to",1) if (gain and not update) else ''}}
{{update or c.set_cc(cc,gain)if Set else gain and c.mod_cc(cc,gain)}}
{{'' if logcheck or help or setup or status or update or desc or pi==0 else f'-f "Current Piety|{total:,}"'}}
{{'' if int(piety[0].hours)<=0 or pi<= 0 or not (status or update) else f'-f "** **|**— Piety Downtime —**\nHours of activity collected: {piety[0].hours}\nTotal hours needed for next Piety point: {pi*80}"'}}
<drac2>
if status or update:
 if not exists("pietyImg") and (not "image" in piety[0] or piety[0].image == ""):
  return f'-f "Power Image|Use `!piety image <URL>` to set an image of your Power. Linked file must end in .png or .jpg and be no larger than 2MB."'
</drac2>
-color <color> -thumb {{pietyImg if exists("pietyImg") else piety[0].image if "image" in piety[0] else ""}}
-footer "!piety help | setup | status"