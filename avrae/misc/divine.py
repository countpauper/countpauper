<drac2>
c=character()
cleric_lvl = c.levels.get('Cleric')
header='Divine Strike'
if cleric_lvl<8:
	return f'-f "{header}|Cleric level too low." '
sub_classes=load_json(get('subclass','{}'))
sub_class=sub_classes.get('ClericLevel')
dice='1d8' if cleric_lvl<14 else '2d8'
if sub_class is None:
	sub_class='Unknown'

damage_type=dict(war='',life='[radiant]',forge='[fire]', order='[psychic]', nature='[chromatic]', tempest='[thunder]', trickery='[poison]',  twilight='[radiant]')
dice+=damage_type.get(sub_class.lower(), '')

desc=f'Once on each of your turns when you hit a creature with a weapon attack, you can cause the attack to deal an extra {dice} damage to the target.'
return f' -f "({sub_class}) Domain: {header} |{desc}" -d {dice}'
</drac2>
