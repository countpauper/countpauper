<drac2>
args=argparse(&ARGS&)
db=load_json(get_gvar('e8dfa775-0f63-4aaa-9869-4fab02edfbfc'))
fields={}
stack=[db]
dir=[]
dbg=[]
dbg_idx=0
dbg_break=100
key_sep='.'
while stack:
	head=stack.pop()
	dbg_idx+=1
	if dbg_idx>=dbg_break:
		dbg.append(f'[{dbg_idx}/{dbg__break}] break {stack}')
		break

	for k,v in head.items():
		top_key=k.split(key_sep)[-1]
		if typeof(v)=='str':
			fields[top_key]=v
			dbg.append(f'[{k}] {top_key}=`{v}`')
		elif typeof(v)=='SafeList':
			idx=randint(len(v))
			next=v[idx]
			stack.append({k:next})
			dbg.append(f'[{k}] `{idx}` / {len(v)}')
		elif typeof(v)=='SafeDict':
			decikeys=[int(k) for k in v.keys() if k.isdecimal()]
			if decikeys:
				if selection:=args.last(top_key):
					if match:=[vo for vo in v.values() if
							   	(typeof(vo)=='str' and vo.lower()==selection) or
								(typeof(vo)=='SafeDict' and vo.get(top_key).lower()==selection) or
								(typeof(vo)=='SafeList' and vo.lower() in vo)]:
						stack.append({k:match[0]})
						dbg.append(f'[{k}]: chose `{selection}`')
					else:
						dbg.append(f'[{k}]: missing `{selection}`')
				else:
					rand_key=randint(max(decikeys))
					mkeys=[rk for rk in decikeys if rk>rand_key]
					picked_key=min(mkeys)
					next=v[str(picked_key)]
					stack.append({k:next})
					dbg.append(f'[{k}] `{rand_key}` % {max(decikeys)}')
			txtkeys=[k for k in v.keys() if not k.isdecimal()]
			if txtkeys:
				for nk in txtkeys:
					nv=v[nk]
					stack.append({f'{k}{key_sep}{nk}': nv})
					dbg.append(f'[{k}] @ {typeof(nv)} [{len(nv)}]')
			if not decikeys and not txtkeys:
				dbg.append(f'[{k}] Empty')
		else:
			dbg.append('f[{k}] ? {typeof(v)}')
debugstr="\n".join(dbg)
return f'embed -title "NPC" -desc "{debugstr}" '+' '.join(f'-f "{k}|{v}"' for k,v in fields.items())
</drac2>