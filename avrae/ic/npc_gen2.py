<drac2>
args=argparse(&ARGS&)
db=load_json(get_gvar('e8dfa775-0f63-4aaa-9869-4fab02edfbfc'))
fields={}
stack=[db]	# TODO: should be svar, with default is [gvar]
dir=[]
dbg=[]
dbg_idx=0
dbg_break=100
key_sep='.'
while stack:
	head=stack.pop()
	dbg_idx+=1
	if dbg_idx>=dbg_break:
		dbg.append(f'[{dbg_idx}/{dbg_break}] break {stack}')
		break

	for stack_ref,stack_value in head.items():
		top_key=stack_ref.split(key_sep)[-1]
		# unspecified for weighted tables that can be unspecified, like tattoos
		if stack_value is None:
			dbg.append('[{stack_ref}] none')
			continue
		if typeof(stack_value)=='str':
			if not stack_value:
				continue
			# reference "@a.b.c" links to another entry db[a][b][c] assuming all are dicts
			if stack_value[0]=='@':
				pointer=stack_value[1:].split(key_sep)
				# TODO: relative. for all empty pointers at the start, cut back k
				ref=db
				prev_key=None
				while pointer:
					p=pointer.pop(0)
					# for referencing keys into a dict tree
					# eg @gender.pronoun -> gender:{pronoun:"she"} would refer to "she"
					if typeof(ref)=='SafeDict' and p in ref:
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
						dbg.append(f'[{stack_ref}] missing **{p}** in `{stack_value}`')
						ref=None
						break
				if ref:
					stack.append({stack_ref:ref})
					dbg.append(f'[{stack_ref}] ref {stack_value}')
			# TODO: dice string detection
			# TODO: gvar detection
			# a straight field result, store it in fields
			else:
				fields[top_key]=stack_value
				dbg.append(f'[{stack_ref}] {top_key}=`{stack_value}`')
		elif typeof(stack_value)=='SafeList':
			# lists are like even weighted tables. pick a random one
			# TODO: could still check fro arguments key:[{key:argument}]
			idx=randint(len(stack_value))
			next=stack_value[idx]
			stack.append({stack_ref:next})
			dbg.append(f'[{stack_ref}] `{idx}` / {len(stack_value)}')
		elif typeof(stack_value)=='SafeDict':
			# all numeric keys are treated as a waited table to roll on, values are tresholds
			# it can be mixed with sub dicts
			# This is queued in the stack before branching keys so it is executed last (FILO)
			#  which serves to have specific (deep) fields override generic
			decikeys=[int(k) for k in stack_value.keys() if k.isdecimal()]
			if decikeys:
				if selection:=args.last(top_key):
					selection=selection.lower()
					if match:=[vo for vo in stack_value.values() if
							   	(typeof(vo)=='str' and vo.lower()==selection) or
								(typeof(vo)=='SafeDict' and vo.get(top_key).lower()==selection) or
								(typeof(vo)=='SafeList' and vo.lower() in vo)]:
						stack.append({stack_ref:match[0]})
						dbg.append(f'[{stack_ref}]: chose `{selection}`')
					else:
						dbg.append(f'[{stack_ref}]: missing `{selection}`')
				else:
					rand_key=randint(max(decikeys))
					mkeys=[rk for rk in decikeys if rk>rand_key]
					picked_key=min(mkeys)
					next=stack_value[str(picked_key)]
					stack.append({stack_ref:next})
					dbg.append(f'[{stack_ref}] `{rand_key}` % {max(decikeys)}')
			# text keys form the main hierarchy of related properties, eg location->race->gender->pronoun
			# children are tables whose chance/fields may depend on the grandparent
			# siblings are unrelated fields
			txtkeys=[k for k in stack_value.keys() if not k.isdecimal()]
			if txtkeys:
				for nk in txtkeys:
					nv=stack_value[nk]
					stack.append({f'{stack_ref}{key_sep}{nk}': nv})
					dbg.append(f'[{stack_ref}] @ **{nk}** {typeof(nv)} [{len(nv)}]')
			if not decikeys and not txtkeys:
				dbg.append(f'[{stack_ref}] Empty')
		else:
			dbg.append('f[{stack_ref}] ? {typeof(stack_value)}')
# generate the output
debugstr="\n".join(dbg)
return f'embed -title "NPC" -desc "{debugstr}" '+' '.join(f'-f "{k}|{v}"' for k,v in fields.items())
</drac2>