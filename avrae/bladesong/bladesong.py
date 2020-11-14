<drac2>
# TODO
# simplify reusing !bladesong (with a use) when no arguments and no effect active
# or use bladesong -i convention to 'reuse'

arg=&ARGS&
# check subclass precondition
ignore = '-i' in arg
singer = ignore
WizardLevel = get('WizardLevel',0)
if exists('subclass'):
	singer= singer or (load_json(subclass).get("WizardLevel")=="Bladesinger")
else:
	singer = singer or WizardLevel > 1

if not singer:
	return 'echo Bladesing is only available to Bladesingers.'

bladesongs = 0
if not ignore:
	counter_name='Bladesong'
	create_cc_nx(counter_name, minVal=0, maxVal=proficiencyBonus, reset='long', dispType='bubble')
	bladesongs = get_cc(counter_name)

# check whether bladesong is already active
var_name='Bladesong'
active = get(var_name,"False").lower()=='true'
effect_name='Bladesong'
c=combat()
me = c.me if c else None
if me:
	song_in_effect=me.get_effect(effect_name)!=None
else:
	song_in_effect=False
if not arg:
	turns=10
elif arg[0]=='off':
	turns = 0
elif arg[0].isdigit():
	# reapply when entering combat
	turns=int(arg[0])
else:
	turns=10

fields=''
title = 'Bladesong'
if not active:
	if turns > 0:
		if not ignore:
			if bladesongs>0:
				mod_cc(counter_name, -1, True)
			else:
				title = 'Bladesong not available.'
				turns=None
			fields+= f' -f "Remaining Uses|{character().cc_str(counter_name)}|inline"'
else:
	title = 'Bladesong already active'

if turns is None:
	description="No more uses of bladesong remaining."
elif turns>0:
	description="You invoke a secret elven magic called the Bladesong, provided that you aren't wearing medium or heavy armor or using a shield. It graces you with supernatural speed, agility, and focus.\n"
	description+="You can use a bonus action to start the Bladesong, which lasts for 1 minute. It ends early if you are incapacitated, if you don medium or heavy armor or a shield, or if you use two hands to make an attack with a weapon. "
	description+=f"You can also dismiss the Bladesong at any time you choose (no action required) using `!{ctx.alias} off`.\n"
	if ignore or WizardLevel>=10:
		description+="**Song of Defense**\n"
		description+=f"As a reaction, expend a spell slot to reduce damage by 5x the slot's level. `!{ctx.alias} defense <level>`.\n"

	set_cvar(var_name, 'True')
	# You gain a bonus to your AC equal to your Intelligence modifier (minimum of +1).
	ac_bonus = max(intelligenceMod,1)
	if me:
		me.remove_effect(effect_name)
		ac = me.ac
	else:
		ac = armor
	fields+=f' -f "AC|{ac}+{ac_bonus}[int] = {ac+ac_bonus}|inline" '
	if me:
		me.add_effect(effect_name,f'-ac +{ac_bonus}',turns)
	# Your walking speed increases by 10 feet
	speed_bonus = 10
	speed = get('speed',None)
	if speed:    # common cvar, not adjusted because it's hard to restore
		current_speed = int(speed.split(sep=" ")[0])
		new_speed = f'{current_speed+speed_bonus} ft.'
		fields+=f' -f Speed|"{current_speed} + {speed_bonus} = {new_speed}"|inline '
	else:
		fields+=f' -f Speed|"+{speed_bonus} ft."|inline'
	# You have advantage on Dexterity (Acrobatics) checks.
	fields+=' -f "Acrobatics Advantage|`!check Acrobatics bladesong`|inline"'
	# You gain a bonus to any Constitution saving throw you make to maintain your concentration on a spell equal to your Intelligence modifier (minimum of +1)
	con_bonus = max(intelligenceMod,1)
	fields+=f' -f "Concentration +{con_bonus}[int]|`!save Con bladesong`|inline"'
	# Song of victory
	if WizardLevel>=14 or ignore:
		dmg_bonus = max(intelligenceMod,1)
		fields+=f' -f "Song of Victory|Melee damage +{dmg_bonus}\n`!a ... victory`|inline"'
else:
	description="Your bladesong ends if you are incapacitated, if you don medium or heavy armor or a shield, or if you use two hands to make an attack with a weapon. You can also dismiss the Bladesong at any time you choose (no action required).\n"
	title = 'Bladesong ends'
	if me:
		me.remove_effect(effect_name)
		ac = c.me.ac
	else:
		ac = armor
	set_cvar(var_name,"False")
	fields += f' -f "AC|{ac}|inline"'
return f'embed -title "{title}" -desc "{description}" {fields if exists("fields") else ""} -thumb https://thumbs2.imgbox.com/2d/0e/BgEQebBE_t.jpg'
</drac2>