drac2>
cc='Ki Points'
ch=character()
args=&ARGS&

if ignore:=argparse(args).last('i',False):
	args.remove('-i')
MonkLevel = int(get('MonkLevel', 0))
dmg = None
if len(args):
	dmg = args[0]
	args = args[1:]

field=f"""-f "Deflect Missiles|If you reduce the damage of a deflected missile to 0, you can catch the missile if it is small enough for you to hold in one hand and you have at least one hand free. 
If you catch a missile in this way, you can spend 1 ki point to make a ranged attack (range 20/60 feet) with the weapon or piece of ammunition you just caught, as part of the same reaction. 
You make this attack with proficiency, regardless of your weapon proficiencies, and the missile counts as a monk weapon for the Attack." """

# Counter
if not ignore:
	if not ch.cc_exists(cc):
		err(f'You don\'t have {cc}, use `!level Monk {MonkLevel}`')
	if not ch.get_cc(cc):
		err(f'You don\'t have any {cc} left. You need to rest first.')

	ch.mod_cc(cc,-1)
	field+=f'-f "{cc}|{ch.cc_str(cc)}"'
else:
	field+=f'-f "{cc}|Not used"'

if not dmg:
	return f'embed -title "{name} attacks with a Deflected Missile!"  {field} -thumb {image}'

me = combat().me if combat() else None
if me:
	atk ='Deflected Missile'
	me.add_effect(atk,f'-attack "{proficiencyBonus+dexterityMod}|{dmg}|Return a deflected missile"',0,end=False,desc='If you catch a missile, spend 1 ki point to make a ranged attack 20/60ft')
	return f'a "{atk}" {" ".join(args)} {field}'
# split in supported and unsupported arguments for attack simulation.
supported={'adv':1,'dis':1,'ea':1,'-b':2,'-d':2}
unsupported=[]
i=0
while i<len(args):
	if args[i] in supported.keys():
		i+= supported[args[i]]
	else:
		unsupported.append(args.pop(i))
if unsupported:
	return f'echo Sorry, due to Avrae limitations it\'s not currently possible to execute a custom missile attack outside of combat with the following arguments: `{" ".join(unsupported)}`.'
# Simulate attack
args=argparse(args)
d=args.last('d')
hit=['1d20','2d20kh1','3d20kh1','2d20kl1'][args.adv(ea=True)]
hit+=f'+{proficiencyBonus+dexterityMod}'
if b:=args.last('b'):
	hit+=f'+{b}'
if d:=args.last('d'):
	dmg+=f'+{d}'
return f'embed -title "{name} attacks with a Deflected Missile!" -desc "**To Hit**: {vroll(hit)}\n**Damage**: {vroll(dmg)}" {field} -thumb {image}'
</drac2>
