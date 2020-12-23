<drac2>
args=&ARGS&
if args and args[0]!='-i':
	if not args[0].isdigit():
		return f'echo `!{ctx.alias} [<spell level>] [-i]`'
	slot = int(args[0])
else:
	slot=None
ignore = argparse(args).last('i')
cc='Harness Divine Power'
ccd='Channel Divinity'
#            0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20
power_level=[0,0,1,1,1,1,2,2,2,2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3]
cleric_level = int(get('ClericLevel',0))
max_power = power_level[cleric_level]

# recreate appropriate counter since it's not in level
ch=character()
if ch.cc_exists(cc):
	power=min(ch.get_cc(cc),max_power)
	ch.delete_cc(cc)
else:
	power=max_power

title = f'-title "{cc}"'
if not ignore:
	if max_power:
		ch.create_cc(cc,minVal=0, maxVal=max_power, reset='long', dispType='bubble')
		ch.set_cc(cc, power)
	else:
		return f'embed {title} -desc "You can\'t harness any divine power." -f "Cleric Level|{cleric_level}"'

	if not power:
		return f'embed {title} -desc "You are no longer able to harness your divine power." -f "{cc}|{cc_str(cc)}" '

	if not ch.cc_exists(ccd):
		return f'embed {title} -desc "You do not have any divine power." -f "{cc}|{cc_str(cc)}|inline" -f "{ccd}|None|inline" '
	if ch.get_cc(ccd)==0:
		return f'embed {title} -desc "You have no more divine power." -f "{cc}|{cc_str(cc)}|inline" -f "{ccd}|{cc_str(ccd)}|inline" '

max_slot = (proficiencyBonus+1)//2
if slot is None:
	for s in range(max_slot,0,-1):
		if ch.spellbook.get_max_slots(s) - ch.spellbook.get_slots(s):
			slot=s
			break
elif slot>max_slot:
	return f'embed {title} -desc "You can not harness that much divine power." -f "Cleric [{cleric_level}]|Highest level slot: `{max_slot}`" '

if not slot:
	slot=max_slot

slot_max = ch.spellbook.get_max_slots(slot)
used_slot = ch.spellbook.get_slots(slot)
if not slot_max-used_slot:
	return f'embed {title} -desc "You do not need to harness your divine power." -f "Spell Slots|{ch.spellbook.slots_str(slot)}" '

# do it
ch.spellbook.set_slots(slot,used_slot+1)
if not ignore:
	ch.mod_cc(cc, -1)
	ch.mod_cc(ccd, -1)
	return f'embed {title} -desc "You harness your divine power." -f "{cc}|{cc_str(cc)}|inline" -f "{ccd}|{cc_str(ccd)}|inline" -f "Spell Slots|{ch.spellbook.slots_str(slot)}|inline" '
else:
	return f'embed {title} -desc "You harness your divine power." -f "Spell Slots|{ch.spellbook.slots_str(slot)}|inline" '
</drac2>