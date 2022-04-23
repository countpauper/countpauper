!alias works <drac2>
args=&ARGS&
pargs=argparse(args)
ch=character()
use=f'{ctx.prefix}{ctx.alias} <skill> [-b <bonus>] [-rr <count>] [adv] [dis]'
if any(h in args for h in ['help','?']) in args:
	return f'echo `{use}`'
mi = int(pargs.last('mi', default=(10 if ch.csettings.get('talent', False) else 0), type_=int))
if skills:={sn:s for sn, s in ch.skills if any(a.lower() in sn.lower() for a in args)}:
	sn, skill = list(skills.items())[0]
	rolls=[skill.d20(pargs.adv(boolwise=True), reroll=ch.csettings.get('reroll'), min_val=mi)]
else:
	return f'echo `{use}`'
rolls+=pargs.get('b')
amount=min(40, pargs.last('rr',1,type_=int))
cc='timetrack'
tt=ch.get_cc(cc) if ch.cc_exists(cc) else 0
tt_per_work=40
if tt<amount*tt_per_work:
	return f"echo You don't have enough downtime ({tt}) to work ({amount}) times."
mi=int(pargs.last('mi',default=(10 if ch.csettings.get('talent',False) else 0), type_=int))
results=[]
work=load_json(get_gvar('3fec562f-07aa-457f-a04e-5db2264e93f9')).work
tgp, tdt=0,0
for i in range(amount):
	r=vroll('+'.join(rolls))
	comp=None
	if (cr:=vroll('1d10')).total==1:
		comp_roll=vroll(f'1d{max(n for n in work.complication.keys())}')
		comp=[c for n, c in work.complication.items() if int(n)==comp_roll.total][-1]
	result=[q for s,q in work.result.items() if r.total>=int(s)][-1]
	gp = result.gp
	if comp:
		gp = int(gp*comp.get('mult',1))
		cd=f'{cr} Complication! {comp_roll}, {comp.desc}'
	else:
		cd=f'{cr} No complication.'
	results.append(f'{r} : {result.quality}, {gp} gp. {cd}')
	tgp+=gp
	tdt+=tt_per_work
	if comp and comp.get('dm'):
		results.append('`@Dungeon Master` **to resolve this complication**')
		break
nl='\n'
ch.mod_cc(cc,-tdt)
fields=f'-f "Downtime [-{tdt}]|{ch.cc_str(cc)}" -f "Total Earnings|`!coins {tgp}`"'
return f'embed -title "Downtime Activity: Work with {sn.title()}" -desc "{nl.join(r for r in results)}" {fields}'
</drac2>




# Tools part
tools = {t.strip(): 1 for t in get('pTools', '').split(',')}
tools.update({t.strip(): 2 for t in get('eTools', '').split(',')})
tools = {t: p for t, p in tools if (any(a.lower() in t.lower() for a in args))}
if tools:
	sn, p = list(tools.items())[0]
	roll = [['1d20', '2d20kh1', '2d20kl1'][pargs.adv()] + (f'mi{mi}' if mi else '') + f'ro{ro}' if ro else '']
	rolls.append(f'{ch.stats.profiencyBonus * p}')
# TODO stat
else: