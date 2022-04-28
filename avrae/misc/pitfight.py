<drac2>
ch=character()
ch.set_cvar_nx("pitrolls","ath, acro, con")
p,a,text,img,cs=pitrolls.split(", ",2), '''&*&''', "",image,ch.csettings
rr,rt=cs["reroll"] if "reroll" in cs else None,10 if "talent" in cs and cs["talent"]==True else None
cmd = f'{ctx.prefix}pitfight'
if "?" in a or "help" in a:
	title="Pit fighting 101!"
	text=f"""Arguments|**help**|**?** Displays this message.
**set**
Used to set what three rolls you want to make, separated by spaces. Use quotes around multi-word attack names. Ex.
 - `{cmd} set constitution mace athletics`
 - `{cmd} set acro \\\"Blood Spear\\\" con`
 Using just `{cmd} set` will show what it's set to.
**adv# dis# ea# -b# <bonus>**
 Applied to the roll # in order. Ex.
  - `{cmd} dis1` for disadvantage on the first roll
  - `{cmd} ea3` for eleven accuracy on the last (attack) roll
  - `{cmd} -b2 1d4[guidance] -b2 1d10[inspiration] adv3` for guidance and inspiration on the second roll and advantage on the 3rd roll
  """
	img=""
elif a.startswith("set"):
	a=' '.split(a)
	if len(a)==4:
		ch.set_cvar("pitrolls",", ".join(a[1:]))
		title=name+" sets their rolls!"
	else:
		title=name+" checks their rolls!"
	text="Rolls|"+"\n".join(pitrolls.split(", ",2))
else:
	title=name+" goes pit fighting!"
	text="Meta|"
	roll_args=argparse(a)
	for idx, t in enumerate(p):
		adv_dict=dict(adv=f'adv{idx+1}',dis=f'dis{idx+1}',ea=f'ea{idx+1}')
		bonuses=roll_args.get(f'b{idx+1}')
		if t.lower() in "athletics":
			text+=f"\n**Athletics:** {vroll('+'.join([ch.skills.athletics.d20(roll_args.adv(boolwise=True, custom=adv_dict),rr,rt if ch.skills.athletics.prof>=1 else None)] + bonuses))}"
		elif t.lower() in "acrobatics":
			text+=f"\n**Acrobatics:** {vroll('+'.join([ch.skills.acrobatics.d20(roll_args.adv(boolwise=True, custom=adv_dict),rr,rt if ch.skills.acrobatics.prof>=1 else None)] + bonuses))}"
		elif t.lower() in "constitution":
			bonuses = [f"1d{max([int(''.join([x for x in cc.name if x.isdigit()])) for cc in character().consumables if cc.name.startswith('Hit Dice')])}"] + bonuses
			text+=f"\n**Constitution:** {vroll('+'.join([character().skills.constitution.d20(roll_args.adv(boolwise=True, custom=adv_dict),rr,rt if ch.skills.constitution.prof>=1 else None)] + bonuses))}"
		else:
			for attack in character().attacks:
				if t.lower() in attack.name.lower():
					bonuses = [attack.raw["automation"][0]["effects"][0]["attackBonus"]] + bonuses
					roll_str='+'.join([["1d20","2d20kh1","3d20kh1","2d20kl1"][roll_args.adv(ea=True, custom=adv_dict)]+(f'ro{rr}' if rr else '')]+bonuses)
					text+=f"\n**{attack.name}**: {vroll(roll_str)}"
					break
</drac2>
-title "{{title}}"
-f "{{text}}"
-thumb {{img}}
-footer "{{cmd}} help"


