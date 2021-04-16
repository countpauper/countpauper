!servalias harness <drac2>
args=&ARGS&
if args and args[0]!='-i':
	if not args[0].isdigit():
		return f'echo `{ctx.prefix}{ctx.alias} [<spell level>] [-i]`'
	slot = int(args[0])
else:
	slot=None

ignore = argparse(args).last('i')
cc='Harness Divine Power'
ccd='Channel Divinity'
ch=character()

title = f'-title "{name} harnesses {get("their","their")} Divine Power"'
if not ignore:
	if not ch.cc_exists(ccd) or not ch.cc_exists(cc):
		return f'echo You do not have any divine power to harness. Use `{ctx.prefix}level Cleric`'
	if ch.get_cc(ccd)==0 or ch.get_cc(cc)==0:
		return f'echo You have no more divine power. {cc} : {cc_str(cc)} / {ccd} : {cc_str(ccd)}'

cleric_level=int(get('ClericLevel',0))
max_slot = (proficiencyBonus+1)//2
if slot is None:
	for s in range(max_slot,0,-1):
		if ch.spellbook.get_max_slots(s) - ch.spellbook.get_slots(s):
			slot=s
			break
elif slot>max_slot:
	return f'echo You can not harness that much divine power as a Level {cleric_level} Cleric. Highest level slot: `{max_slot}`'

if not slot:
	slot=max_slot

slot_max = ch.spellbook.get_max_slots(slot)
used_slot = ch.spellbook.get_slots(slot)
if not slot_max-used_slot:
	return f'echo You do not need to harness your divine power. Spell Slots:{ch.spellbook.slots_str(slot)}'

desc="You can expend a use of your Channel Divinity to fuel your spells. As a bonus action, you touch your holy symbol, utter a prayer, and regain one expended spell slot, the level of which can be no higher than half your proficiency bonus (rounded up)."
# do it
ch.spellbook.set_slots(slot,used_slot+1)
slot_field=f'-f "Spell Slots|{ch.spellbook.slots_str(slot)} *+1*|inline"'
if not ignore:
	ch.mod_cc(cc, -1)
	ch.mod_cc(ccd, -1)
	return f'embed {title} -desc "{desc}" -f "{cc}|{cc_str(cc)}|inline" -f "{ccd}|{cc_str(ccd)}|inline" {slot_field}'
else:
	return f'embed {title} -desc "{desc}" {slot_field}'
</drac2>