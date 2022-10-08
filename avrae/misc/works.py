<drac2>
args=&ARGS&
pargs=argparse(args)
ch=character()
use=f'{ctx.prefix}{ctx.alias} "<skill>|<tool>" [-b <bonus>] [-rr <count>] [-with <stat>] [adv] [dis]'
mi = int(pargs.last('mi', default=0, type_=int))

all_tools={
	"Alchemist's supplies": "intelligence",
	"Brewer's supplies": "constitution",
	"Calligrapher's Supplies": "dexterity",
	"Carpenter's tools": "strength",
	"Cartographer's tools": "dexterity",
	"Cobbler's tools": "strength",
	"Cook's utensils": "wisdom",
	"Glassblower's tools": "dexterity",
	"Herbalism Kit": "wisdom",
	"Jeweler's tools": "dexterity",
	"Leatherworker's tools": "dexterity",
	"Mason's tools": "strength",
	"Painter's supplies": "dexterity",
	"Potter's tools": "dexterity",
	"Smith's Tools": "strength",
	"Tinker's tools": "dexterity",
	"Weaver's tools": "dexterity",
	"Woodcarver's tools": "dexterity",
	"Bagpipes": "charisma",
	"Drum": "charisma",
	"Dulcimer": "charisma",
	"Flute": "charisma",
	"Lute": "charisma",
	"Lyre": "charisma",
	"Horn": "charisma",
	"Pan flute": "charisma",
	"Shawm": "charisma",
	"Viol": "charisma",
	"Navigator's tools": "wisdom",
	"Thieves' tools": "dexterity",
	"Poisoner's Kit": "intelligence",
	"Disguise Kit": "charisma",
	"Forgery Kit": "dexterity"
}
if with_stat_arg:=pargs.last('with'):
	all_stats='strength','dexterity','constitution','intelligence','wisdom','charisma'
	with_stat=([stat for stat in all_stats if stat.startswith(with_stat_arg.lower())]+[None])[0]
	if not with_stat:
		return f'echo Unknown -with stat: `{with_stat_arg}`. Choose one from {", ".join(all_stats)}.'
else:
	with_stat=None

if not args:
	return f'echo `{use}`'
select=args[0]

if skills:={sn:s for sn, s in ch.skills if select.lower() in sn.lower()}:
	sn, skill = list(skills.items())[0]
	sn = sn.title()
	mi=max (mi, (10 if ch.csettings.get('talent', False) and skill.prof>=1 else 0))
	if with_stat:
		rolls=[skill.d20(pargs.adv(boolwise=True), reroll=ch.csettings.get('reroll'), min_val=mi,
			mod_override=skill.bonus + int(skill.prof*proficiencyBonus)),
			f'{ch.stats.get_mod(with_stat)}[{with_stat[:3]}]']
	else:
		rolls=[skill.d20(pargs.adv(boolwise=True), reroll=ch.csettings.get('reroll'), min_val=mi)]
elif tools:=[t for t in all_tools.keys() if select.lower() in t.lower()]:
	sn = tools[0]
	stat_name=with_stat or all_tools[sn]
	stat_skill=ch.skills[stat_name]	 # TODO -with skill instead
	tool_profs = {t.strip().lower(): 1 for t in get('pTools', '').split(',')}
	tool_profs.update({t.strip().lower(): 2 for t in get('eTools', '').split(',')})
	tool_prof = int(max(tool_profs.get(sn.lower(), 0), stat_skill.prof) * proficiencyBonus)
	base_stat_mod = stat_skill.value - (stat_skill.bonus + int(stat_skill.prof*proficiencyBonus))
	mi=max(mi, (10 if ch.csettings.get('talent', False) and tool_prof>=1 else 0)) or None
	rolls=[stat_skill.d20(pargs.adv(boolwise=True), reroll=ch.csettings.get('reroll'), min_val=mi, mod_override=stat_skill.bonus+tool_prof),
					  f'{base_stat_mod}[{stat_name[:3]}]']
else:
	return f'echo Unknown skill or tool `{select}`.'
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
		cd=f'{cr} Complication! {comp_roll}\n- {comp.desc}'
	else:
		cd=f'{cr} No complication.'
	results.append(f'{r} : {result.quality.capitalize()}, {gp} gp - {cd}')
	tgp+=gp
	tdt+=tt_per_work
	if comp and comp.get('dm'):
		results.append('`@Dungeon Master` **to resolve this complication**')
		break
nl='\n'
ch.mod_cc(cc,-tdt)
ch.coinpurse.modify_coins(gp=tgp)
fields=f'-f "Downtime [-{tdt}]|{ch.cc_str(cc)}" -f "Total Earnings [+{tgp}gp]|{ch.coinpurse}"'
return f'embed -title "{name} works with {sn}" -desc "{nl.join(r for r in results)}" {fields}'
</drac2>