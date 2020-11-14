embed <drac2>
counter_name='Bladesong'
var_name='Bladesong'
effect_name='Bladesong'
bladesongs=get_cc(counter_name) if cc_exists(counter_name) else None
active = get(var_name,'False').lower()=='true'
c=combat()
me = c.me if c else None
if me:
	song_in_effect=me.get_effect(effect_name)!=None
else:
	song_in_effect=False
title = '-title "Song of Defense"'
fields = ""
if bladesongs==None:
	description="This is only available to Bladesingers."
elif WizardLevel<10:
	description="Song of defense requires wizard level 10 or higher."
elif (c and not song_in_effect) or (not c and not active):
	description="Song of defense can only be used while bladesinging."
else:
	arg = "&*&"
	slot = int(arg) if arg else 1
	if slot<1:
		description='You can not use a cantrip to absorb damage.'
		dmg_absorbed = None
	else:
		max_slots = character().spellbook.get_max_slots(slot)
		if max_slots<=0:
			description=f'You do not have level {slot} spell slots to absorb damage.'
			dmg_absorbed = None
		else:
			remaining_slots = character().spellbook.get_slots(slot)
			if remaining_slots<=0:
				description=f'You do not have a level {slot} spell slot left to absorb damage.'
				dmg_absorbed = None
			else:
				description="You direct your magic to absorb damage while your Bladesong is active. When you take damage, you use your reaction to expend one spell slot and reduce that damage to you by an amount equal to five times the spell slot's level."
				dmg_absorbed = slot * 5
				remaining_slots -= 1
				set_slots(slot, remaining_slots)

			fields +=f'-f "Spell Slot [{slot}]|{character().spellbook.slots_str(slot)}|inline" '
	fields += f' -f "Damage Absorbed|{dmg_absorbed}|inline" '
return title+f' -desc "{description}" '+(fields if exists('fields') else '')
</drac2>  -thumb https://thumbs2.imgbox.com/2d/0e/BgEQebBE_t.jpg