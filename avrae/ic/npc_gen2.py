!alias gen <drac2>
# TODO: update with svar/uvar
db=load_json(get_gvar('e8dfa775-0f63-4aaa-9869-4fab02edfbfc'))
fields={}
stack=[db]
dir=[]
dbg=[]
dbg_idx=0
dbg_break=100
while stack:
	head=stack.pop()
	dbg_idx+=1
	if dbg_idx>=dbg_break:
		dbg.append(f'[{dbg_idx}/{dbg__break}] break {stack}')
		break

	for k,v in head.items():
		if typeof(v)=='str':
			fields[k]=v
			dbg.append(f'[{k}]=`{v}`')
		elif typeof(v)=='SafeList':
			idx=randint(len(v))
			next=v[idx]
			stack.append({k:next})
			dbg.append(f'[{k}] `{idx}` out of [{len(v)}]')
		elif typeof(v)=='SafeDict':
			decikeys=[int(k) for k in v.keys() if k.isdecimal()]
			if decikeys:
				rand_key=randint(max(decikeys))
				mkeys=[rk for rk in decikeys if rk>=rand_key]
				if not mkeys:
					err(f'No random key {rand_key} in {decikeys}')
				picked_key=min(mkeys)
				next=v[str(picked_key)]
				stack.append({k:next})
				dbg.append(f'[{k}] `{rand_key}` out of {max(decikeys)}')
			txtkeys=[k for k in v.keys() if not k.isdecimal()]
			if txtkeys:
				for nk in txtkeys:
					nv=v[nk]
					stack.append({f'{k}.{nk}': nv})
					dbg.append(f'[{k}] @ {typeof(nv)} [{len(nv)}]')
			if not decikeys and not txtkeys:
				dbg.append(f'[{k}] Empty')
		else:
			dbg.append('f[{k}] ? {typeof(v)}')
debugstr="\n".join(dbg)
return f'embed -title "Random NPC" -desc "{debugstr}" '+' '.join(f'-f "{k}|{v}"' for k,v in fields.items())
</drac2>