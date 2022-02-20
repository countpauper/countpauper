!alias tense embed <drac2>
arg='''&*&'''
uv='tension'
var = load_json(get(uv,'{}'))
pool=var.get(ctx.alias,[])
if arg=='roll':
  if not pool:
    return f'-title "Nothing happens"'
  else:
    rolls =[vroll(r) for r in pool]
    roll_str = '\n'.join(str(r) for r in rolls)
    if any(r.total==1 for r in rolls):
      return f'-title "A complication!" -desc "{roll_str}"'
    else:
      return f'-title "Luckily nothing happens" -desc "{roll_str}"'
if arg in ['clear','none']:
  pool=[]
  title = "The tension is cleared"
elif arg in ['reduce','-']:
  pool.pop(0)
  var[ctx.alias] = pool
  set_uvar(uv, dump_json(var))
  title=f"The tension is reduced"
elif arg in ['+', '']:
  title=f"The tension mounts"
  pool.append('1d6')
else:
  title=f"The tension mounts"
  pool.append(arg)
var[ctx.alias]=pool
set_uvar(uv,dump_json(var))
return f'-title "{title}." -desc "{"🎲 " * len(pool)}"'
</drac2>