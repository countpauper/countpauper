<drac2>
arg=&ARGS&
# check subclass precondition
ignore = '-i' in arg
title = 'Bladesong'
WizardLevel = int(get('WizardLevel',0))
if not ignore:
	if WizardLevel<2:
		return f'echo {title} is only available to level 2 wizards. Use `!update`'
	if exists('subclass'):
		subclass = load_json(subclass).get("WizardLevel","").lower()
		if  subclass not in ["bladesinger","bladesinging"]:
			return f'echo The Bladesinger subclass is not set. Use `!level Wizard {WizardLevel} Bladesinging`'

ch=character()
bladesongs = 0
cc = 'Bladesong'
if not ignore:
	if not ch.cc_exists(cc):
		return f'echo {title} counter is not created. Use `!level Wizard {WizardLevel} Bladesinger` or `!cc create Bladesong -min 0 -max {proficiencyBonus} -reset long -type bubble` to create it.'
	bladesongs = ch.get_cc(cc)

#  don't use var anymore
var_name='Bladesong'
ch.delete_cvar(var_name)

effect_name='Bladesong'
c=combat()
me = c.me if c else None
if me:
	active=me.get_effect(effect_name)!=None
else:
	active=False

if not arg:
	turns=10
elif arg[0]=='off':
	turns = 0
elif arg[0].isdecimal():
	# reapply when entering combat
	turns=int(arg[0])
else:
	turns=10

fields=''


if turns is None:
	description="No more uses of bladesong remaining."
elif turns>0:
	description=f"""You invoke a secret elven magic called the Bladesong, provided that you aren't wearing medium or heavy armor or using a shield. It graces you with supernatural speed, agility, and focus.
You can use a bonus action to start the Bladesong, which lasts for 1 minute. It ends early if you are incapacitated, if you don medium or heavy armor or a shield, or if you use two hands to make an attack with a weapon.
You can also dismiss the Bladesong at any time you choose (no action required) using `!{ctx.alias} off{' -i' if ignore else ''}`."""
	if ignore or WizardLevel>=10:
		fields+=f'-f "Song of Defense|As a reaction, expend a spell slot to reduce damage by 5x the slot\'s level.\n`!{ctx.alias} defense <level>{" -i" if ignore else ""}`."'
	if ignore or WizardLevel>=14:
		dmg_bonus = max(intelligenceMod,1)
		fields+=f' -f "Song of Victory|Melee damage +{dmg_bonus}\n`!a <attack> bladesong`|inline"'

	if not me:
		fields+=f' -f "Not in combat|Bladesong was activated outside of combat. You can apply the effect after joining combat using `!{ctx.alias} [<rounds>] -i`"'
	# You gain a bonus to your AC equal to your Intelligence modifier (minimum of +1).
	ac_bonus = max(intelligenceMod,1)
	speed_bonus = 10
	con_bonus = max(intelligenceMod,1)
	if me:
		me.remove_effect(effect_name)
		ac = me.ac
	else:
		ac = armor
	fields+=f' -f "AC|{ac}+{ac_bonus}[int] = {ac+ac_bonus}|inline" '
	if me:
		me.add_effect(effect_name, duration=10, passive_effects=dict(ac_bonus=ac_bonus), desc=f'Your AC increases by {ac_bonus}.\n - Walking speed increases by {speed_bonus} feet.\n - Advantage on dexterity(acrobatics) checks.\n - +{con_bonus} bonus to any Constitution saving throw you make to maintain your concentration on a spell.')
	# Your walking speed increases by 10 feet
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
	fields+=f' -f "Concentration +{con_bonus}[int]|`!save Con bladesong`|inline"'
	# Song of victory
else:
	description="Your bladesong ends if you are incapacitated, if you don medium or heavy armor or a shield, or if you use two hands to make an attack with a weapon. You can also dismiss the Bladesong at any time you choose (no action required)."
	title = 'Bladesong ends'
	if me:
		me.remove_effect(effect_name)
		ac = c.me.ac
	else:
		ac = armor
	fields += f' -f "AC|{ac}|inline"'

if not active:
	if turns > 0:
		if not ignore:
			if bladesongs>0:
				ch.mod_cc(cc, -1, True)
			else:
				title = 'Bladesong not available.'
				turns=None
			fields+= f' -f "Remaining Uses|{ch.cc_str(cc)}|inline"'
elif turns > 0:
	title = 'Bladesong already active'

return f'embed -title "{title}" -desc "{description}" {fields if exists("fields") else ""} -thumb https://thumbs2.imgbox.com/2d/0e/BgEQebBE_t.jpg'
</drac2>
