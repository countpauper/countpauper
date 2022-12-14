# real python
import sys
import json

def split_paragraph(s):
	if not s.startswith('<p>'):
		return '', None
	index = s[3:].find('</p>')
	if index==-1:
		return '',None
	return s[index+7:], s[3:index+3]

def split_paragraphs(s):
	result=[]
	s,p = split_paragraph(s)
	while p is not None:
		result.append(p)
		s,p = split_paragraph(s)
	return result

def split_actions(s):
	if s is None:
		return dict()
	paragraphs = split_paragraphs(s)
	return {h:b.strip() for a in paragraphs for h,b in [split_action_header(a)] if h and b}


def split_action_header(s):
	header_prefix='<em><strong>'
	header_postfix='</strong></em>'
	if not s.startswith(header_prefix):
		return (None, None)
	header_end=s.find(header_postfix)
	if header_end==-1:
		return (None, None)
	header=s[len(header_prefix):header_end].strip('.')
	tailer=s[header_end+len(header_postfix):]
	return header,tailer

def parse_multi_attack(s):
	actions = split_actions(s)
	extra_attacks=[b for h,b in actions.items() if h=='Multiattack']
	if not extra_attacks:
		return 1
	text_count=dict(one=1, two=2, twice=2, three=3, four=4, five=5, six=6)
	text_count.update({'1d4':2.5})
	counts = [q for txt,q in text_count.items() if txt in extra_attacks[0]]
	if counts:
		return max(counts)
	return 1	# Hydra and other weirdness

def parse_to_hit(s):
	index=s.find('to hit')
	if index==-1:
		return None
	return int(s[:index].strip())

def parse_reach(s):
	prefix = 'reach'
	end_index=s.find('ft')
	start_index=s.find(prefix)
	if start_index>0:
		return int(s[start_index + len(prefix):end_index].strip())
	prefix='range'
	start_index=s.find(prefix)
	if start_index==-1:
		return None
	return (int(range) for range in s[start_index + len(prefix):end_index].strip().split('/'))


def parse_target(s):
	if 'target' in s:
		return 'target'
	if 'creature' in s:
		return 'creature'
	return None

def parse_dice(s):
	open_index=s.find('(')
	close_index=s.find(')')
	if open_index==-1 or close_index==-1:
		return None
	return s[open_index+1:close_index]

def parse_dmg_type(s):
	if not s:
		return None
	types=['piercing','slashing','bludgeoning',
		   'cold','fire','lightning','acid','poison',
		   'necrotic','radiant',
		   'thunder','psychic','force']
	return [t for t in types if t in s.lower()] or None

def parse_avg_dmg(s):
	index=min(idx for idx,c in enumerate(s) if c not in " 0123456789")
	try:
		return int(s[:index-1])
	except ValueError:
		return None

def convert_attack(action):
	if not action.startswith('<em>'):
		return None
	type_end=action.find('</em>')
	if type_end==-1:
		return None
	hit_prefix='<em>Hit:</em>'
	hit_index=action.find(hit_prefix)
	if hit_index==-1:
			raise exception("No hit in attack")
	type_str=action[4:type_end].lower()
	hit_str=action[type_end+5:hit_index]
	damage_str=action[hit_index+len(hit_prefix):]
	return dict(type=type_str,
				spell_attack='spell' in type_str,
				weapon_attack='weapon' in type_str,
				melee='melee' in type_str,
				ranged='ranged' in type_str,
				to_hit=parse_to_hit(hit_str),
				reach=parse_reach(hit_str),
				target=parse_target(hit_str),
				avg_dmg=parse_avg_dmg(damage_str),
				dice_dmg=parse_dice(damage_str),
				dmg_type=parse_dmg_type(damage_str))

	#melee = 'melee' in type_str
	#ranged = 'ranged' in type_str
	#weapon='weapon' in type_str
	#spell='spell' in type_str


def convert_attacks(s):
	actions = split_actions(s)
	return {n:atk for n, a in actions.items() for atk in [convert_attack(a)] if atk is not None}


def first_number(s):
	first = s.split(' ')[0]
	if '/' in first:
		numerator, denominiator = first.split('/')[:2]
		return float(numerator)/float(denominiator)
	else:
		return int(first)

def parse_size(s):
	sizes=[size for size in ['tiny','small','medium','large','huge','gargantuan'] if size in s.lower()]
	if not sizes:
		return None
	else:
		return sizes[-1]

def convert(monster):
	return dict(
		name = monster.get('name'),
		size=parse_size(monster.get('meta')),
		# TODO: type
		# TODO: alignment
		cr=first_number(monster.get('Challenge')),
		ac=first_number(monster.get("Armor Class")),
		speed=first_number(monster.get('Speed')),	# TODO: split by comma, detect type, assume walking, detect hover
		hp=first_number(monster.get('Hit Points')),
		stats={key:int(monster.get(key)) for key in ['STR', 'DEX', 'CON', 'INT', 'WIS', 'CHA']},
		# TODO: perception
		# TODO: skill
		resistances=parse_dmg_type(monster.get('Damage Resistances')),	# TODO: not the same as for weapons, doesn't detect "nonmagical" and silvered and so on
		immunities=parse_dmg_type(monster.get('Damage Immunities')),
		condition=[c.strip() for c in monster.get('Damage Immunities', '').split(',')],
		attack_number=parse_multi_attack(monster.get('Actions','')),
		#TODO: traits, generalize html parsing
		attacks=convert_attacks(monster.get('Actions')),
		img=monster.get('img_url')
	)

def avg(c):
	return sum(c)/len(c)

if __name__ == "__main__":
	with open("srd_monsters.json", "r") as file:
		data = json.load(file)
	monsters = [convert(m) for m in data]
	print('\n'.join(str(m) for m in monsters))

	print('cr, #, min, avg, max, dpr, hit, hp, ac	')
	for cr in [0,0.125,0.25,0.5]+list(range(1,25)):
		avg_dmg,avg_dpr,avg_hit, avg_ac,avg_hp=[],[],[],[],[]
		for m in monsters:
			if m.get('cr')!=cr:
				continue
			dmgs=[a.get('avg_dmg') for a in m.get('attacks',dict()).values()]
			dmgs=[d for d in dmgs if d is not None]
			avg_dmg+=dmgs
			avg_hit+=[a.get('to_hit') for a in m.get('attacks',dict()).values()]
			avg_ac+=[m.get('ac')]
			avg_hp+=[m.get('hp')]
			avg_dpr+=[avg(dmgs or [0]) * m.get('attack_number', 1)]
		avg_dmg=[d for d in avg_dmg if d is not None]
		avg_hit=[h for h in avg_hit if h is not None]
		if not avg_dmg:
			avg_dmg=[-1]
		if not avg_hit:
			avg_hit=[-1]
		if avg_hp:
			print(f'{cr}, {len(avg_dmg)}, {min(avg_dmg)}, {avg(avg_dmg)}, {max(avg_dmg)}, {avg(avg_dpr)}, {avg(avg_hit)}, {avg(avg_hp)}, {avg(avg_ac)}')
		else:
			print(f'{cr}, , , , , , , ,')