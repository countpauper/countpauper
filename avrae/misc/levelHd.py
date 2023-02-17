!servalias hd <drac2>
ch=character()
HDs=dict(wizard=6,sorcerer=6,
		 artificer=8, bard=8, cleric=8, druid=8, monk=8, rogue=8, warlock=8,
		 fighter=10, paladin=10, ranger=10,
		 barbarian=12)
maxHD_per_cls=[(HDs[cls.lower()], lvl) for cls, lvl in ch.levels]
maxHD_per_die={die:sum(amt for hd,amt in maxHD_per_cls if hd==die) for die in [6,8,10,12]}
cc_prefix='Hit Dice (d'
hit_dice_cc={int(cc.name[len(cc_prefix):-1]):cc for cc in ch.consumables if cc.name.startswith(cc_prefix)}
new_CCs=[]
for hd, dice in maxHD_per_die.items():
	if cc:= hit_dice_cc.get(hd):
		difference = dice-cc.max
		newCC = ch.edit_cc(cc.name, maxVal=maxHD_per_die[hd])
		new_CCs.append(newCC)
		# grant the new HD immediately as leveling would
		if difference>0:
			newCC.set(cc.value + difference)
	elif dice>0:
		new_CCs.append(ch.create_cc(cc_prefix+hd+")", minVal=0, maxVal=dice, reset='long'))

class_desc = "\n".join(f"**{cls}**: {lvl}d{HDs[cls.lower()]}" for cls, lvl in ch.levels)
separator="⎯"* 18
cc_desc = "\n".join(f"**{cc.name}** : {str(cc)}" for cc in new_CCs)
return f'''embed -title "{name} updates {get('their','their')} hit dice." -desc "{class_desc}\n{separator}\n{cc_desc}\n{reset_info}"'''
</drac2>
