embed <drac2>
conditions=dict(
	blinded=':see_no_evil:',
	charmed=':heart_eyes:',
	deafened=':hear_no_evil:',
	frightened=':scream:',
	 grappled=':people_wrestling:',
	incapacitated=':face_with_spiral_eyes:',
	invisible=':bust_in_silhouette:',
	paralyzed=':ice_cube:',
	petrified=':rock:',
	poisoned=':snake:',
	prone=':upside_down:',
	restrained=':chains:',
	stunned=':dizzy_face:',
	unconscious=':sleeping:',
	concentrating=':brain:')
descs=[f'{icon} - {txt}' for txt,icon in conditions.items()]
args=argparse('''&*&''')
if timeout:=args.last('t', 30, type_=int):
	timeout = f'-t {timeout}'
else:
	timeout = ''
return f'-title Conditions -desc "{", ".join(descs)}" {timeout}'
</drac2>