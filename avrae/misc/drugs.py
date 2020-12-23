!alias drugs <drac2>
# configuration
cc='Dope'
item='Drugs'
addiction_dc=10
od_dc=15

#context
ch=character()
args=argparse(&ARGS&)

# bag dependency check
bag_var = 'bags'
bags = load_json(get(bag_var, '[]'))
if not bags:
	return f'echo You don\'t have a place to stash your {item}. Use `!bag $ Backpack` '

# adjust the counter, create it if needed
if not ch.cc_exists(cc):
	ch.create_cc(cc,minVal=-10, maxVal=10, desc=f'The {cc} in your system affects your behavior.')
	ch.set_cc(cc, 0)

save=vroll(ch.skills.wisdom.d20(args.adv(boolwise=True)))
fields = f' -f "Wisdom Save|{save}" '

if not args.last('take',False) and save.total>=addiction_dc:
	ch.mod_cc(cc,-1)
	fields += f' -f "{cc}|{cc_str(cc)}"'
	return f'embed -title "You resist the urge" {fields} '

drug_bag=None
for bag in bags:
	if bag[1].get(item,0)>0:
		drug_bag=bag[1]
if not drug_bag:
	ch.mod_cc(cc, -1)
	fields += f' -f "{cc}|{cc_str(cc)}"'
	return f'embed -title "You ran out of {item}" -desc "You furiously search through your bags but find nothing. Go get some and use `!bag Backpack + {item}`" {fields}'

used = max(1, len(args.get('take')))
if save.result.crit==2:
	used=max(used,2)

remaining = drug_bag[item]
used=min(remaining,used)
remaining-= used

if remaining:
	drug_bag[item] = remaining
else:
	drug_bag.pop(item)

ch.set_cvar(bag_var, dump_json(bags))
ch.mod_cc(cc, used)
fields += f' -f "{cc}|{cc_str(cc)}"'
if used==1:
	return f'embed -title "You use {item}" {fields}'
else:
	con_save = vroll(ch.skills.constitution.d20())
	fields+=f' -f "Con save|{con_save}"'
	if con_save.total>=od_dc:
		return f'embed -title "You use {used} {item}" {fields}'
	else:
		return f'embed -title "You overdose on {item}" {fields}'

</drac2>