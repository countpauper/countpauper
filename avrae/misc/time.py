!alias time
<drac2>
A=&ARGS&
cal=load_json(get_gvar("1665221a-908a-40e9-ac8b-24ce938dfe09"))
envs=load_json(get_gvar("e035815d-80c9-465c-91d4-60d4c6446c23"))
di=0
do={}
d=None
tv = get_svar("ictime")   TODO default value
tv = load_json(tv) if tv else {}
TODO: if argument is a known channel, set that channel's name instead

cn=ctx.channel.name
tvc = tv.get(cn,{})
ct=float(tvc.get('time',0.0))
if not A:
  code=get_gvar("e8d1289d-a4b9-4a71-a285-5e32123c61ad").replace(@T@,str(tc))
  cmd=f"tembed "+code
  return ""
if A[0].lower() in ('help','?'):
  return "echo help"
for a in A:
  a = a.lower()
  if a[0] in '+-':
    if d or di>0:
      err("Unexpected +/-")
    d=1 if a[0]=='+' else -1
    a=a[1:]
    m=None
  else:
    m=[idx for idx in range(len(cal.months)) if cal.months[idx].name.lower().startswith(a)]
    if m:
      if d:
        err("+/- with month")
      di=1
      a=str(m[0])
  y=cal.length
  if a.isdigit():
    if di==0:
      do[1]=max(0,int(a)-1)
    elif di==1:
      do[1]=(do[1] if do[1] else 0)+sum([m.length for m in cal.months[:int(a)-1]])
    elif di==2:
      do[2] = int(a)*y
    else:
      err("Too many date digits")
    di+=1
  elif a in envs.light:
    do['l']=a
  elif a in envs.environment.keys():
    env=envs.environment[a]
    do['l']=env.get('light',do.get('l',None))
    do['dc']=env.get('timedc',do.get('dc',None))
  elif a[:2]=="dc":
    do['dc']=int(a[2:])
  elif ':' in a:
    t=a.split(':')
    do[0]=int(t[0])/24.0 if t[0] else 0
    do[0]+=int(t[1])/1440.0 if t[1] else 0
    do[0]+=int(t[2])/86400.0 if len(t)>2 and t[2] else 0
  else:
    err("Unknown argument "+a)
if 'l' in do:
  tvc['light']=do['l']
if 'dc' in do:
  tvc['dc']=do['dc']
cmd=f"echo {do}"
if not d:
  dd=[do.get(0,ct%1),do.get(1,(ct)%y),do.get(2,y*int(ct/y))]
  ct,d=0,1
ct+=d*sum([x for x in dd if x])
tvc['time']=ct
tv[cn]=tvc
cmd+=f"-{dd}"
cmd="svar ictime "+dump_json(tv)
</drac2>{{cmd}}