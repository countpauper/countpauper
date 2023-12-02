<drac2>
# Ideas: query attack, spell for base, skill
# * Add all -b
#*  use adv and replace first 1d20
# * multiple AC/DC,  targets, report chance for each
#*  Target: combatant
#      (will also roll damage), hp and damage rolls to see if you dying or dead
#* qualitative chance for target unlocked with -s
#     * svar/uvar to set default behavior (show=False allow=True or list of ids, char or user names or just True/False)
#     * svar to set levels per user
# 	  - sub commands to config for user (block (all) allow [<id>]  and hide(all/id) show [<id>] and for uvar : config [-show]
#     insight/nature/religion check for quality or quantity level (how to decide which for what?), based on race? https://github.com/avrae/avrae/issues/1292
#     - monster database: undead, fiends are religion, beasts, monstrosities,aberations are nature, humanoids are insight?
#         - DC related to CR? may not make sense. it's obvious a dragon has high AC, but
#         - 5 over the DC shows detailed, under is blocked
# *-i <name in group> to use current initiative character instead of selected character
#* Spell database to identify spell attacks and saves
#*  when checking against target DC, also hide the roll string of their save
#  - DC only attacks
#  - run multiple expressions vs multiple targets (when targeting multiple with one spell or attack)
#  x [fail query]  handle spells without target (self range, touch range) as rollstr 0 dc 0 (certain)
# - opposed checks are roll - opposing roll (lots of term negating)
#     !chance grapple -t for opposed grapple check (auto select acrobatics or athletics)
#* take -b effects into account for attacks automatically

# rr and say how the chance of how many will hit/miss (and crit? might get messy)
# also split arguments differently (first - arg) and allow separate terms like !chance 1d20 1d20+1d4 -ac 12
# !chance death (saves (1d20, should be with luck, but https://github.com/avrae/avrae/issues/1388)) with -rr for total chance to die (take into account crits then)
# x   option to show as techo (-h <sec>)
# x  Always show precise number a spoiler
#* replace vars in expression for custom attacks:

# approach: cut expression into + terms (*/ later at lower level)
# for each term determine the range and the expression governing the chance? or the chance for each value in the range
# it should recognize number and die size, ro, kh[1] and kl[1] at least (until generalized)
# then iterate, starting with a 0,0 range and empty change array. One by one sum the min and max and multiply the chance
# dict per term in array {term=str, min=int, max=int, p=[probability mass function]}
# first d20: apply adv and remember 1 and 20 as fail and crit. in the end miss% = min(fail,sum) hit=min(crit,sum)

# simple maths: http://www.darkshire.net/~jhkim/rpg/systemdesign/torben_rpg_dice.pdf
# maths: https://stats.stackexchange.com/questions/3614/how-to-easily-determine-the-results-distribution-for-multiple-dice
# dnd maths: https://stats.stackexchange.com/questions/116792/dungeons-dragons-attack-hit-probability-success-percentage/116913#116913
# dice maths: https://www.omnicalculator.com/statistics/dice
# max dice math: https://math.stackexchange.com/questions/1696623/what-is-the-expected-value-of-the-largest-of-the-three-dice-rolls/
# roller (seem to do it the hard way but good to compare): http://topps.diku.dk/torbenm/troll.msp
# troll rolls: 4d6kh3 * 6 stats treshold 80: repeat a:=sum({6 # sum(largest 3 4d6)}) until a>=80


# TODO: kh1 seems broken
# is crit range taken from character?

#### Parse configuration: svar > cvar > uvar
config=load_json(get('Chance','{}'))
config.update(load_json(get_svar('Chance','{}')))

# get allowed setting: don't run at all if not allowed
allowed=config.get('allowed',True)
if typeof(allowed)=='str':
	allowed=allowed.split(',')
if typeof(allowed)=='SafeList':
	allowed={id:True for id in allowed}
if typeof(allowed)=='SafeDict':
	# match with ids, user names, nick names, character names, unmatched(*) in order of precedence
	allowed={id.lower():allow for id,allow in allowed.items()}
	allowed = allowed.get(str(ctx.author.id), allowed.get(str(ctx.author).lower(), allowed.get(ctx.author.display_name.lower(),allowed.get(character().name.lower(), allowed.get('*',False)))))
if not allowed:
	return f'echo `{ctx.prefix}{ctx.alias}` is blocked for you by the server adminstrator.'

# get show setting: show detailed result (%)
show=config.get('show',False)
if typeof(show)=='str':
	show=show.split(',')
if typeof(show)=='SafeList':
	show={id:True for id in show}
if typeof(show)=='SafeDict':
	# match with ids, user names, nick names, character names, unmatched(*) in order of precedence
	show={id.lower():s for id,s in show.items()}
	show = show.get(str(ctx.author.id), show.get(str(ctx.author).lower(), show.get(ctx.author.display_name.lower(),show.get(character().name.lower(), show.get('*',False)))))

# TODO: if typeof(show)=='SafeList') : (or string split, or dict: match)

#### Parse arguments
syntax=f'`{ctx.prefix}{ctx.alias} <attack>/<skill>/<save>/<roll> [-ac <number>][-dc <number>][-t <target>]... [-b <bonus roll>] [adv|dis|ea] [-h[ide]] [-s[how]] [-i [<combatant>]]`'
args=&ARGS&
if not args:
	return f'echo You did not specify a query. Use: {syntax}'
query=args[0]
args=argparse(args[1:])

dbg=args.last('dbg',config.get('debug',False))
show_query=dbg or not args.last('hide',args.last('h',False, type_=bool), type_=bool)
show_target=dbg or args.last('show',args.last('s',show, type_=bool), type_=bool)

### Parse the query  and translate it into a query (description) and expression (roll string)
expression=query
dice_expression='exp' if query[0].isnumeric() else False
reroll_luck=None
criton=20
reliability=None
attack=None
skill=None
save_query=None

# select the executor
executor=character()	# default current character

# if in combat and using -i option, select another executor
if init_name := args.last('i'):
	executor=None
	if fight:=combat():
		current=combat().current
		if init_name is True or str(init_name).startswith('-'):
			if current:
				if current.type=='group':
					executor=current.combatants[0]
				else:
					executor=current
			else:
				err('No combatant active in initiative.')
		else:
			init_name=str(init_name).lower()
			if current:
				if current.type=='combatant' and current.name.lower().startswith(init_name):
					executor=current
				elif current.type=='group':
					# prefer matches in current group
					if matches:=[combatant for combatant in current.combatants if combatant.name.lower().startswith(init_name)]:
						executor=matches[0]
					# fallback to any matches
			# if not matches with current just fal back to any in combat
			if executor is None:
				if matches:=[combatant for combatant in combat().combatants if combatant.name.lower().startswith(init_name)]:
					executor=matches[0]
				else:
					err(f'Combatant `{init_name}` not found.')
	else:
		err(f'Error `-i` only works in a channel where combat is active.')

if executor!=character() and executor.controller!=ctx.author.id and not dice_expression:
	show_query=not show_target

criton=20
reroll_luck=None
reliability=None
spell_list=load_json(get_gvar('13dc3e0a-a230-40ca-8fb3-a39846300b18'))
save_query = None
spell_query = None
if executor:
	if typeof(executor)=='AliasCharacter':	# duck say quack ?
		criton = executor.csettings.get('crit_on', 20)
		reroll_luck = executor.csettings.get('reroll')
		reliability = 10 if executor.csettings.get('talent') else None

	queryId=query.replace(' ','').lower()

	if saves:={save_name:skill for save_name, skill in character().saves if save_name.startswith(queryId)}:
		query=tuple(saves.keys())[0]
		skill=saves[query]
		save_query=save_query
		expression = skill.d20(reroll=reroll_luck)
	elif skills:={skill_name:skill for skill_name,skill in executor.skills if skill_name.lower().startswith(queryId)}:
		if len(skills)>1:
			return f'echo `{ctx.prefix}{ctx.alias} {" | ".join(skills.keys())}'
		query=tuple(skills.keys())[0]
		skill=skills[query]

		expression = skill.d20(reroll=reroll_luck, min_val=reliability if skill.prof >= 1 else None)
	elif attacks:={atk.name:atk for atk in executor.attacks if atk.name.lower().startswith(query.lower())}:
		query=tuple(attacks.keys())[0]
		attack=attacks[query]
		if automation:=attack.raw.get('automation',[]):
			effects=[]
			for auto in automation:
				effects+=auto.get('effects',[])
			if len(effects)>1:
				err(f'Implementation limitation: Multiple attack effects for {query} not yet supported')
			effect=effects[0]
			effect_type=effect.get('type','none')
			if effect.get('type')=='attack':
				expression='1d20'
				if reroll_luck is not None:
					expression+=f'ro{reroll_luck}'
				if bonus:=effects[0].get("attackBonus",0):
					expression+=f'+{bonus}'
			else:
				# TODO: with save set dc to save and find save type on target, same as spells
				err(f'Implementation limitation: Attack effect type `{effect_type}` for {query} not yet supported.')
	elif executor.spellbook.dc:	 # optimization, don't scan 500 spells if not a caster
		for query, spell_query in {spell_name:spell for spell_name,spell in spell_list.items() if spell_name.startswith(query.lower())}.items():
			if spell_query.get('attack'):
				save_query=None
				if reroll_luck is None:
					expression = f'1d20+{executor.spellbook.sab}'
				else:
					expression = f'1d20ro{reroll_luck}+{executor.spellbook.sab}'
				break
			elif save_query:=spell_query.get('save'):
				break
			else:
				spell_query=None
	else:
		pass # keep query, TODO spells, but need access to 'automation' info (which save or spell attack)


	# replace limited set of variables
	vars={'proficiencyBonus':executor.stats.prof_bonus,
		'strengthMod':-5 + executor.stats.strength//2,
		'dexterityMod':-5 + executor.stats.dexterity//2,
		'constitutionMod':-5 + executor.stats.constitution//2,
		'intelligenceMod':-5 + executor.stats.intelligence//2,
		'wisdomMod':-5 + executor.stats.wisdom//2,
		'charismaMod':-5 + executor.stats.charisma//2,
		'level':executor.levels.total_level,
		'spell':executor.spellbook.sab or 0}

	for var,val in vars.items():
		expression=expression.replace(var,str(val))

#####  parse targets
cancrit=None
targets={}
if not skill:
	for ac in args.get('ac'):
		if not ac.isdecimal():
			return f'echo AC `{ac}` is not a number, Use {syntax}'
		targets[f'AC {ac}']={'target':int(ac),'crit':True, 'show':True}

if not attack:
	for dc in args.get('dc'):
		if not dc.isdecimal():
			return f'echo DC `{dc}` is not a number, Use {syntax}'
		targets[f'DC {dc}']={'target':int(dc),'crit':False, 'show':True}

opposed_skills={
	"acrobatics":"athletics",
	"athletics":"athletics",
	"deception":"insight",
	"initiative":"initiative",
	"insight":"deception",
	"perception":"stealth",
	"sleightOfHand":"perception",
	"stealth":"perception",
}
if fight:=combat():
	for t in args.get('t'):
		# TODO: could also split target arguments | and specify opposed skill dc or extra bonuses, adv
		if combatant:=fight.get_combatant(t):
			target_name=combatant.name
			if spell_query and save_query:
				if len(args.get('t'))>1:	# only one show_query and only one expression right now
					err('Implementation limitation: multiple saving throws not yet supported.')
				targets[f'{target_name} {save_query} save'] = {'target': executor.spellbook.dc, 'crit': False, 'reverse':True, 'show':show_query}
				show_query=show_target
				if saves := [(save_name, save) for save_name, save in combatant.saves if
							 save_name.lower().startswith(spell_query.save)]:
					expression = saves[0][1].d20(args.adv(eadv=False, boolwise=True))  # advantage here to avoid ea
				else:
					err(f'`{spell_query.save} save for `{query}` not found for `{target_name}`')
			elif save_query:
				targets[f'{target_name} Spell DC']={'target':combatant.spellbook.dc, 'crit':False, 'show':show_target}
			elif attack or dice_expression or spell_query:
				targets[f'{target_name} AC']={'target':combatant.ac, 'crit':True, 'show':show_target}
			elif skill:
				if opposed_skill_name:= opposed_skills.get(query):
					opposed_skill = combatant.skills[opposed_skill_name]
					targets[f'{target_name} passive {opposed_skill_name}']={'target':10+opposed_skill.value, 'crit':False, 'show':show_target}
				else:
					pass #

# d8 is a valid expression, defaults to 1 die
if expression[0]=='d' and len(expression)>1 and expression[1].isdecimal():
	expression='1'+expression

# dice expression has to start with a number or d to be a valid spell/skill/attack
if not expression[0].isnumeric():
	return f'echo Invalid expression query `{expression}`.'

# replace first 1d20 with advantage, this is almost the same as !roll, which only does it for the first term
expression = expression.replace('1d20',['1d20','2d20kh1','3d20kh1','2d20kl1'][args.adv(eadv=True)],1)

# retrieve effects
if executor==character():
	effects=fight.me.effects if fight and fight.me else []
else:
	effects=executor.effects

# append argument and effect bonuses to the expression
effect_prefix=None
if attack:
	effect_prefix='b'
elif skill:
	effect_prefix='cb'
# TODO save 'sb' but look at all targets instead of executor effects. SimpleComabtant.save includes it but it doesn't include sdis/sadv yet
else:
	effect_prefix = None
expression='+'.join([expression]+args.get('b')+[e.effect.get(effect_prefix,'0') for e in effects if effect_prefix in e.effect])

# change the query < constant
expression=expression.replace(' ','')
expression=expression.replace('+-','-').replace('-','+-')

########  Create the probability mass function for the expression
expression_terms=expression.split('+')
terms=[]
crit=None
fail=None
firstd20=True
for term in expression_terms:
	sign=1
	t = None
	if term[0]=='-':
		sign=-1
		term=term[1:]

	if term.isnumeric():
		v=int(term)
		terms+=[{'term':f'{"-" if sign<0 else ""}{term}','min':sign*v,'max':sign*v}]
	elif 'd' in term:
		## Parse number of rolled dice
		dice_exp=term.split('d',maxsplit=1)
		if not dice_exp[0].isdecimal():
			err(f'Dice expression syntax error: number of dice must be positive number: `{term}`')
		dice=int(dice_exp[0]) if dice_exp[0] else 1

		## split in numbers and non numbers
		numeric=None
		dice_part=[]
		for dice_char in dice_exp[1]:
			if numeric is not dice_char.isnumeric():
				dice_part.append('')
				numeric = dice_char.isnumeric()
			dice_part[-1]+=dice_char

		## die size
		if not dice_part or not dice_part[0].isdecimal():
			err(f'Dice expression syntax error: die size must be a positive number: `{term}`')
		die_size=int(dice_part[0])
		dice_part=dice_part[1:]
		p=[0]+[1.0/die_size]*die_size # 0 is drop chance, p[v] for rolling v

		## handle operators
		drop_set={}
		keep_hi=None
		keep_lo=None
		drop_chance=0
		dice_ops=[[dice_part[i],int(dice_part[i+1])] for i in range(0,len(dice_part),2) if dice_part[i+1].isnumeric()]
		while dice_ops:
			op=dice_ops.pop(0)
			arg_v=op[1]
			operator=op[0].lower()

			# drop lowest dice is the same as keep highest rest
			if operator=='pl':
				keep_hi=dice-arg_v
			elif operator=='ph':
				keep_lo=dice-arg_v
			elif operator=='kh':
				keep_hi=arg_v
			elif operator=='kl':
				keep_lo=arg_v
			else:
				# not dropping hi or lo, handle the other operators which do not reduce the number of dice
				arg_v=min(max(0,arg_v),die_size)
				if operator[-1]=='<':
					operator=operator[:-1]
					selection=range(1,arg_v)
				elif operator[-1]=='>':
					operator=operator[:-1]
					selection=range(arg_v+1,die_size+1)
				elif arg_v>0 and arg_v<=die_size:
					selection=[arg_v]
				else:
					selection=[]
				if operator=='k':
					# TODO: k1k2k3 doesn't work because it drops everything
					p[0]+=sum(pv for v,pv in enumerate(p) if v not in selection and v>0)
					p=[pv if (v==0 or v in selection) else 0 for v,pv in enumerate(p)]
				elif operator=='p':
					p[0]+=sum(pv for v,pv in enumerate(p) if v in selection)
					p=[pv if (v==0 or v not in selection) else 0 for v,pv in enumerate(p)]
				elif operator=='ro':
					reroll_chance = sum(pv for v,pv in enumerate(p) if v in selection)
					p = [(reroll_chance/die_size if v>0 else 0) + (0 if v in selection else pv) for v, pv in enumerate(p)]
				elif operator=='rr':
					reroll_chance = sum(pv for v,pv in enumerate(p) if v in selection)
					rerolled_chance = reroll_chance / (die_size - len(selection))
					p = [pv if v==0 else 0 if v in selection else pv + rerolled_chance for v, pv in enumerate(p)]

#					inv_reroll_chance = 1-sum(pv for v,pv in enumerate(p) if v in selection)
#					p = [pv if v==0 else 0 if v in selection else pv/inv_reroll_chance for v, pv in enumerate(p)] # to FIX 1d4p1rr<3  should be roughly 25%, 0.0%, 0.0%, 37.5%, 37.5%
				elif operator=='mi':
					selection=range(1,arg_v)
					missed_chances=sum(pv for v,pv in enumerate(p) if v in selection)
					p=[0 if v in selection else pv for v,pv in enumerate(p)]
					p[arg_v]+=missed_chances
				elif operator=='ma':
					selection=range(arg_v+1,len(p))
					missed_chances=sum(pv for v,pv in enumerate(p) if v in selection)
					p=[0 if v in selection else pv for v,pv in enumerate(p)]
					p[arg_v]+=missed_chances
				elif operator=='e':
					err(f'Implementation limitation: Exploding dice not supported in `{term}`')
				else:
					err(f'Dice expression syntax error: Unrecognized operator `{operator}` in `{term}`')

		# precomputed binomial coefficient, TODO move to gvar
		bnc = [[], [1], [1, 2], [1, 3, 3], [1, 4, 6, 4], [1, 5, 10, 10, 5], [1, 6, 15, 20, 15, 6],
			   [1, 7, 21, 35, 35, 21, 7], [1, 8, 28, 56, 70, 56, 28, 8], [1, 9, 36, 84, 126, 126, 84, 36, 9]]

		if keep_hi is not None and keep_hi<dice:
			if keep_hi!=1:
				err(f'Implementation limitation: `{term}` Keeping more or less than 1 die is not yet supported.')	 # also covers keep_hi>1 or keep_lo>1
			pp=p[:]
			for v in range(len(p)):
				np = []
				for d in range(dice):
					np = [sum([npi * pvi for npi in np for pvi in pp[:v]])]
					np += [bnc[dice][-(d + 1)] * pp[v] ** (d + 1)]
				p[v]=sum(np)
			dice=keep_hi
		if keep_lo is not None and keep_lo<dice:
			if keep_lo!=1:
				err(f'Implementation limitation: `{term}` Keeping more or less than 1 die is not yet supported.')
			pp=p[:]
			for v in range(len(p)):
				np = []
				for d in range(dice):
					np = [sum([npi * pvi for npi in np for pvi in pp[v+1:]])]
					np += [bnc[dice][-(d + 1)] * pp[v] ** (d + 1)]
				p[v]=sum(np)
			dice=keep_lo

		if sign<0:
			p.reverse()
			t={'term':'-'+term,'min':sign*die_size,'max':0,'p':p}
		else:
			t={'term':term,'min':0,'max':sign*die_size,'p':p}

		# TODO: This can be optimized here, knowing all pmf are the same. At least for two dice can go through the triangle and double
		# if all chances were even, binomial coefficients could count instead of enumerate the combinations for multiple dice, but comb and factorial aren't available anyway
		terms+=[t]*dice
	else:
		err(f'Dice expression syntax error: Unrecognized term format: `{term}`')

### combine all terms
lo=sum(t['min'] for t in terms)
hi=sum(t['max'] for t in terms)
crit_chance=None
fail_chance=None
pmf=[]
for t in terms:
	if tp:=t.get('p'):
		if criton is not None and t['max']==20 and crit_chance is None:
			fail_chance=tp[1]
			crit_chance=sum(tp[criton:])

		# combine probability mass functions into a new one
		if not pmf:
			#TODO: why is this case special?
			pmf=tp
		else:
			np=[0]*(len(pmf)+len(tp))
			for v1 in range(len(tp)):
				for v0 in range(len(pmf)):
					np[v0+v1]+=pmf[v0]*tp[v1]
			pmf=np

### Output the report
# TODO: if dbg

report = []
if dbg:
	report.append(f'{query} / {expression} : [{lo}] {", ".join(f"{p * 100:.2f}%" for p in pmf)}  [{hi}]')
quality_description = ['flimsy', 'low', 'average', 'decent', 'high', 'certain']

title = f'{query.title()} by {executor.name}'
if show_query:
	title+=f' `{expression}`'

decimals=args.last('d',2)
for target_name,target in targets.items():
	target_value=target.get('target')
	target_description = f'{target_name}'
	show_target=target.get('show')
	if show_target:
		target_description+=f' (`{target_value}`)'

	query_description = f' VS **{target_description}**'

	show_chance=show_target and show_query
	if target_value is None:
		report.append(f'{query_description}: Not Applicable')
	else:
		if target_value > hi:
			miss = 1
		elif target_value < lo:
			miss = 0
		else:
			miss = sum(pmf[i] for i in range(target_value - lo))
		if target.get('reverse',False):
			miss=1-miss

		if target.get('crit'):
			if crit_chance:
				miss=min(miss,1-crit_chance)
			else:
				crit_chance=0
			if fail_chance:
				miss=max(miss,fail_chance)
			hit=1-miss
			if show_chance:
				report.append(f'{query_description}: Miss: `{miss * 100.0:.{decimals}f}%`, Hit: `{hit * 100:.{decimals}f}%` Crit: `{(crit_chance) * 100:.{decimals}f}%`')
			else:
				quality=quality_description[int(hit*(len(quality_description)-1))] if hit>0 else 'impossible'
				report.append(f'{query_description}: Hit chance: `{quality}`, Crit : `{(crit_chance)*100:.{decimals}f}%`')
		else:
			hit=1-miss
			if show_chance:
				report.append(f'{query_description}: Failure: `{miss * 100.0:.{decimals}f}%`, Success: `{hit * 100:.{decimals}f}%`')
			else:
				quality=quality_description[int(hit*(len(quality_description)-1))] if hit>0 else 'impossible'
				report.append(f'{query_description}: Success chance: `{quality}`')

sep='\n'
if show_query:
	if not pmf:
		pmf=[1]
	average = sum((i + lo) * p for i, p in enumerate(pmf))

	possible_lo = min(i for i, p in enumerate(pmf) if p > 0)
	possible_hi = max(i for i, p in enumerate(pmf) if p > 0)

	report.append(f'Minimum: `{possible_lo + lo}` Average: `{average:.{decimals}f}` Maximum: `{possible_hi + lo}`')

	stripped_pmf = pmf[possible_lo:possible_hi + 1]

	label_var,title_var,color_var,data_var='$L$','$T$','$C$','$D$'
	base_url= 'https://quickchart.io/chart?c={type:%27bar%27,data:{labels:$L$,datasets:[{label:%27$T$%27,backgroundColor:%27$C$%27,data:$D$}]}}'
	labels=[i + lo for i in range(possible_lo, possible_hi + 1)]
	graph_title=expression.replace('+', '%2B')

	accumulated_chance = [100 * sum(stripped_pmf[i:]) for i in range(len(stripped_pmf))]
	url=base_url.replace(label_var,dump_json(labels).replace(' ', '')).replace(title_var,'%25%20%3C%3D%20'+graph_title).replace(color_var,'green').replace(data_var,dump_json([f'{p:3.1f}' for p in accumulated_chance]).replace(' ', ''))
	report.append(f'Graph: [link]({url})')

	pmf_url=base_url.replace(label_var,dump_json(labels).replace(' ', '')).replace(title_var,graph_title).replace(color_var,'red').replace(data_var,dump_json(stripped_pmf).replace(' ', ''))
	report.append(f'PMF: [link]({pmf_url})')


if not report:
	report=['No targets']

# concatenate report within field size limit
field_len=1024-len(title)-3
rep_str=''
for r in report:
	field_len-=len(r)+len(sep)
	if field_len<0:
		break
	rep_str+=r+sep

return f'embed -title "Chance" -f "{title}|{rep_str}"'
</drac2>
