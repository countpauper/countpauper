<drac2>
# TODO: regonize packs as argument and use the official gvar to add them
# improve syntax for adding stacks to 'Dagger 10 arrows 5 gp'
# when adding packs, add the backpack as a bag instead
# recognize backgrounds from a gvar and add all background items
# recognize classes from (the same) gvar and add all standard class items
# add recognized coin types to the pouch (`!qb 10 gp 20 sp 5 ep`)
# add recognized class weapons to a worn/equipped bag instead
# add ammo to ammo bags instead

bv='bags'
bags=load_json(get(bv,'[]'))
args=&ARGS&
# create a default "bag if there's none"
if not bags:
	bags.append(['Bag',{}])

#select a bag if that's the first argument, else the first bag
bag=bags[0]
forbidden=['coin pouch']
chosen_bags=[b for b in bags if args[0].lower() in b[0].lower() and b[0].lower() not in forbidden]
if chosen_bags:
	bag=chosen_bags[0]
	args=args[1:]

# add the items
content=bag[1]
add=[a.split('|',maxsplit=1) for a in args]
add={a[0].title():(int(a[1] if len(a)==2 and a[1].isnumeric() else 1)) for a in add}

new_items={i:q+content.get(i,0)+content.get(i.lower(),0) for i,q in add.items()}
content.update(new_items)

#update the variable
character().set_cvar(bv,dump_json(bags))
return f'echo Added {add} to {bag[0]}'
</drac2>
