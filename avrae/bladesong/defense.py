<drac2>
arg=&ARGS&
ch=character()
WizardLevel=int(get('WizardLevel',0))
c=combat()
me = c.me if c else None
ignore = '-i' in arg
effect_name='Bladesong'
title='Song of Defense'
if not ignore:
	if WizardLevel<10:
		return f'echo {title} requires wizard level 10 or higher. You can overrule using `!{ctx.alias} defense -i`'
	if me and me.get_effect(effect_name) == None:
		return f'echo {title} can only be used while bladesinging. You can start singing with `!{ctx.alias}`'

slot = 1
if len(arg) and arg[0].isdigit():
	slot = int(arg[0])
if slot<1:
	return f'echo You can not use a cantrip to absorb damage with {title}.'
max_slots = ch.spellbook.get_max_slots(slot)
if max_slots<=0:
	return f'echo You do not have level {slot} spell slots to absorb damage with {title}.'
remaining_slots = ch.spellbook.get_slots(slot)
if remaining_slots<=0:
	return f'echo You do not have a level {slot} spell slot available to absorb damage with {title}.'

fields = ''
description="You direct your magic to absorb damage while your Bladesong is active. When you take damage, you use your reaction to expend one spell slot and reduce that damage to you by an amount equal to five times the spell slot's level."
dmg_absorbed = slot * 5
remaining_slots -= 1
ch.spellbook.set_slots(slot, remaining_slots)
fields +=f'-f "Spell Slot [{slot}]|{ch.spellbook.slots_str(slot)}|inline" '
if ch.hp<ch.max_hp:
	if dmg_absorbed<(ch.max_hp-ch.hp):
		fields += f' -f "Damage Absorbed|{dmg_absorbed}\nYou can restore it with `!g hp +{dmg_absorbed}`|inline" '
	else:
		fields += f' -f "Damage Absorbed|{dmg_absorbed}\nYou can restore it to full with `!g hp max`|inline" '

else:
	fields += f' -f "Damage Absorbed|{dmg_absorbed}|inline" '


return f'embed -title "{title}" -desc "{description}" '+fields+' -thumb https://thumbs2.imgbox.com/2d/0e/BgEQebBE_t.jpg'
</drac2>