<drac2>
args=argparse(&ARGS&)
default_db='e8dfa775-0f63-4aaa-9869-4fab02edfbfc'
dbs=load_json(get('generate_db','{}'))
db=load_json(get_gvar(dbs.get('npc',default_db)))

def is_uuid(s):
	return typeof(v)=='str' and len(v)==36 and v.count('-')==4

loaded=dict()
for k,v in db.items():
	if is_uuid(v):
		sub_db = load_json(get_gvar(v))
		db[k] = sub_db
		loaded[k]=len(sub_db)

fields={}
ptr_prefix='@'
# TODO: should be svar, with default is [gvar]
stack=[{k:v for k,v in db.items() if k and k[0]!=ptr_prefix}]
dir=[]
dbg=[]
missing=dict()
if dbg_break:=args.last('d'):
	if dbg_break.isdecimal():
		dbg_break=int(dbg_break)
	else:
		dbg_break=1000

key_sep='.'
while stack:
	if dbg_break and len(dbg)>=dbg_break:
		dbg.append(f'[{dbg_break}] break {stack}')
		break
	head=stack.pop()

	for stack_ref,stack_value in head.items():
		top_key=stack_ref.split(key_sep)[-1]
		# None for weighted tables that can have no result, like not having a tattoo or hair
		if stack_value is None:
			dbg.append(f'[{stack_ref}] none')
			continue
		if typeof(stack_value)=='str':
			if not stack_value:
				continue

			# reference "@a.b.c" links to another entry db[a][b][c] assuming all are dicts
			if stack_value[0]==ptr_prefix:
				pointer=stack_value[1:].split(key_sep)
				# if first pointer is not filled in, ie "@." then relative to current stack_ref
				if not pointer[0] and len(pointer)>1:
					pointer.pop(0)	# remove relative
					# if first pointer after . is present in stack_ref, then splice them together
					# eg stack_key = race.Human.gender  @.gender.pronoun => race.Human.gender.pronoun
					entry_point=pointer[0]
					base_pointer=stack_ref.split(key_sep)
					if entry_point and entry_point in base_pointer:
						base_pointer=base_pointer[:base_pointer.index(entry_point)]
					else:	# else base pointer is current race @.Human.gender => race.Human.gender
						base_pointer=stack_ref.split(key_sep)
					pointer=base_pointer+pointer

				# reparse pointer to get parent on .. and parse {field} references
				pointer,prep=[],pointer
				for p in prep:
					if not p:	# .. is navigate back
						pointer = pointer[:-1]
					elif p[0]=='{' and p[-1]=='}':	 # {field}: replace if exists
						pointer.append(fields.get(p[1:-1],p))
					else:
						pointer.append(p)

				# replace the stack head with the referred data, by traversing the absolute path from the original db
				ref=db
				prev_key=None
				dbg_ptr = key_sep.join(pointer)
				while pointer:
					p=pointer.pop(0).strip()
					# Highest priority is referring to a @key, prefixed explicitly for sharing
					# eg @human_names -> @human_names:['foo'] would refer to ['foo']
					if typeof(ref)=='SafeDict' and (pp:=f'@{p}') in ref:
						prev_key=p
						ref=ref[pp]
					# next referencing normal branch keys into a dict tree
					# eg @gender.pronoun -> gender:{pronoun:"she"} would refer to "she"
					elif typeof(ref)=='SafeDict' and p in ref:
						prev_key=p
						ref=ref[p]
					# for referencing inside of random weighted dict, based on a sub key matching the referenced VALUE
					# eg @race.Human -> race:{14:{race:"Human",age:{...}}} would refer to {age:{...}}}
					elif typeof(ref)=='SafeDict' and (chosen_ref:=[v for k,v in ref.items() if k.isdecimal() and v.get(prev_key)==p]):
						ref=chosen_ref[0]
						# prev_key stays the same
					# for referencing inside of random list, based on a sub key matching the referenced VALUE
					# eg @race.Human -> race:[{race:"Human",age:{...}}] would refer to {age:{...}}}
					elif typeof(ref)=='SafeList' and (chosen_ref:=[i for i in ref and i.get(prev_key)==p]):
						ref=chosen_ref[0]
					# prev_key stays the same
					else:
						dbg.append(f'[{stack_ref}] missing **{p}** in `@{dbg_ptr}`')
						ref=None
						break
				if ref:
					stack.append({stack_ref:ref})
					dbg.append(f'[{stack_ref}] {stack_value}=>`{dbg_ptr}`')

			#  dice string detected. TODO: doesn't check for rr/ro/kh/kl
			elif len(stack_value)>1 and all(c in '0123456789+-*/d()' for c in stack_value):
				r = vroll(stack_value)
				fields[top_key]=r.total
				dbg.append(f'[{stack_ref}] {top_key}={r}')

			# a straight string field result, store it in fields
			else:
				fields[top_key]=stack_value
				dbg.append(f'[{stack_ref}] {top_key}=`{stack_value}`')
		elif typeof(stack_value)=='SafeList':
			# lists are like even weighted tables. pick a random one
			# TODO: could still check for arguments key:[{key:argument}]
			if stack_value:
				idx=randint(len(stack_value))
				next=stack_value[idx]
				stack.append({stack_ref:next})
				dbg.append(f'[{stack_ref}] `{idx}` / {len(stack_value)}')
			else:
				next = None
				stack.append({stack_ref:next})
				dbg.append(f'["{stack_ref}"] empty list')
		elif typeof(stack_value)=='SafeDict':
			# all numeric keys are treated as a weighted table to roll on, values are thresholds
			# it can be mixed with sub dicts
			# This is queued in the stack before branching keys so it is executed last (FILO)
			#  which serves to have specific (deep) fields override generic

			# tables with any integer values are considered percentages and converted to weighted tables
			if stack_value and any(typeof(v)=='int' for v in stack_value.values()):
				dict_value,offset=dict(),0
				for txt, pct in stack_value.items():
					offset+=int(pct)
					dict_value[offset]=txt
				dbg.append(f'[{stack_ref}] {offset}%')
			else:	# otherwise decimal keys are converted to integers
				dict_value={int(k) if k.isdecimal() else k:v for k,v in stack_value.items()}

			decikeys=[k for k in dict_value.keys() if typeof(k)=='int']
			if decikeys:
				if selection:=args.last(top_key):
					selection=selection.lower()
					if match:=[vo for vo in dict_value.values() if
								(typeof(vo)=='str' and vo.lower()==selection) or
								(typeof(vo)=='SafeDict' and vo.get(top_key).lower()==selection) or
								(typeof(vo)=='SafeList' and vo.lower() in vo)]:
						stack.append({stack_ref:match[0]})
						dbg.append(f'[{stack_ref}]: chose `{selection}`')
						if top_key in missing:
							missing.pop(top_key)
					else:
						missing[top_key] = selection
						dbg.append(f'[{stack_ref}]: missing `{selection}` in `{dict_value.values()}`')
				else:
					rand_key=randint(max(decikeys))
					mkeys=[rk for rk in decikeys if rk>rand_key]
					picked_key=min(mkeys)
					next=dict_value[picked_key]
					stack.append({stack_ref:next})
					dbg.append(f'[{stack_ref}] `{rand_key}` / {max(decikeys)}')
			# text keys form the main hierarchy of related properties, eg location->race->gender->pronoun
			# children are tables whose chance/fields may depend on the grandparent
			# siblings are unrelated fields
			txtkeys = [k for k in dict_value.keys() if typeof(k)=='str']
			if txtkeys:
				txtkeys.reverse()
				stack+=[{f'{stack_ref}{key_sep}{nk}': dict_value[nk]} for nk in txtkeys]
				dbg.append(f'[{stack_ref}] - `{", ".join(txtkeys)}`')
			if not decikeys and not txtkeys:
				dbg.append(f'[{stack_ref}] Empty')
		else:
			dbg.append('f[{stack_ref}] ? {typeof(stack_value)}')

def limit_str(s, max_len=32):
	s=str(s)
	if len(s)<max_len:
		return s
	else:
		s = s[:max_len-1]
		s += "…"
		if s.count('`')%2 == 1:
			s+='`'
		return s

if dbg_break:
	# generate the output
	nl="\n"
	debugstr=nl.join(limit_str(dstr,256) for dstr in dbg)[:4096]
	remaining_fields=list(fields.items())[24:]
	fields = [f'-f "{k}|{limit_str(v,256)}"' for k, v in fields.items()][:24]
	if remaining_fields:
		fields.append(f'''-f "...|{nl.join(f'{k}=`{limit_str(v)}`' for k,v in list(remaining_fields))}"''')
	return f'embed -title "NPC" -desc "{debugstr}" '+' '.join(fields)
else:
	if missing:
		return f'echo The following selection was not matched: {",".join(f"{k}={v}" for k,v in missing.items())}'
	# specific height hack
	height_key='height'
	if l:=fields.get(height_key):
		ft=l//12
		if (inch := l-(12*ft))>0:
			fields[height_key]=f"{ft} ft. {inch} in."
		else:
			fields[height_key]=f"{ft} ft."

	# Take the title and text field and replace the other field
	title=fields.get('title','NPC')
	text=fields.get('text', '{name} {surname} the {age} {gender} {race}.')
	def replace_vars(s, vars):
		if '{' not in s:
			return s
		for k, v in vars.items():
			if typeof(v)!='str':
				continue
			var = '{' + k + '}'
			if var in s:
				v = replace_vars(str(v), vars)
				s = s.replace(var, v)
			# capitalized field names get capitalized values
			if v[0].islower():
				var = '{' + k.capitalize() + '}'
				s = s.replace(var, v.capitalize())
		return s

	text = replace_vars(text, fields)
	title = replace_vars(title, fields)


	# field for !npc
	out_fields=[]
	if get('npc_local_roster'):
		first_name=fields.get('name','Nemo')
		npc_name=f'{first_name} {surname}' if (surname:=fields.get('surname')) else first_name
		cmd = f'!npc edit {first_name.lower()} -name \\"{npc_name}\\"'
		out_fields.append(f'''-f "Roster|`{cmd}`" ''')
	return f'embed -title "{title}" -desc "{text}" {" ".join(out_fields)}'
</drac2>