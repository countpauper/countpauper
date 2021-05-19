<drac2>
cc='Channel Divinity'
ch=character()
args='&*&'.strip('".')
ignore=argparse(args).last('i')
if not ignore:
	if not ch.cc_exists(cc):
		return f"echo You don't have `{cc}`, use `!level`"
	if ch.get_cc(cc)<1:
		return f"echo You are out of `{cc}` : {ch.cc_str(cc)}. You need to rest."
	ch.mod_cc(cc,-1)
	cc_field=f'-f "{cc}|{ch.cc_str(cc)}|inline"'
else:
	cc_field=f' -f "{cc}|*Ignored*|inline"'

item = args[:args.index('-')] if '-' in args else args
item=item.strip()
title=f"{name} uses Artisan's Blessing"
item_field=''
if item:
	an = ''
	item_table=load_json(get_gvar('19753bce-e2c1-42af-8c4f-baf56e2f6749'))
	bag_settings=load_json(get('bagSettings','{}'))
	item_table.update(bag_settings.get('customWeights'))
	item_match=([i for i in item_table if item.lower()==i.lower()]+[i for i in item_table if item.lower() in i.lower()]+[None])[0]
	if item_match:
		item=item_match
		if price:=item_table[item].get('cost',None):
			price='10sp' if price=='1gp' else price	# quick conversion for ammo bundles
			coin=''.join(c for c in price if c.isalpha()) or 'gp'
			price=int(''.join(c for c in price if c.isdigit()))

			bundle=item_table[item].get('bundle')
			if bundle is not None:
				price*= 10 / float(bundle)
				item=f'10 {item if item[-1]=="s" else item+"s"}'
			else:
				an='an' if item[0].lower() in 'aeiou' else 'a'

			price = f'{int(price)} {coin}'
			item=item.title().replace("'S","'s")
			item_field=f'-f "{item}|{price}|inline"'
		else:
			item=item.title().replace("'S","'s")
			item_field=f'-f "{item}|*Price unknown*|inline"'
	else:
		an = 'an' if item[0].lower() in 'aeiou' else 'a'

	title = f'{title} to create {an} {item}.'
else:
	title = f'{title}.'

desc="""You conduct an hour-long ritual that crafts a nonmagical item that must include some metal: a simple or martial weapon, a suit of armor, ten pieces of ammunition, a set of tools, or another metal object. The creation is completed at the end of the hour, coalescing in an unoccupied space of your choice on a surface within 5 feet of you.

The thing you create can be something that is worth no more than 100 gp. As part of this ritual, you must lay out metal, which can include coins, with a value equal to the creation. The metal irretrievably coalesces and transforms into the creation at the ritual’s end, magically forming even nonmetal parts of the creation.

The ritual can create a duplicate of a nonmagical item that contains metal, such as a key, if you possess the original during the ritual."""

return f'embed -title "{title}" -desc "{desc}" {cc_field} {item_field} -thumb {image} -color {color}'
</drac2>