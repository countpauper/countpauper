tembed <drac2>
# parse arguments
arg="&*&".lower()
data = load_json(get_gvar(get_svar("downtime","b566982b-c7f8-4ea4-af47-4fd82623b335")))
if arg=='?' or arg.lower()=='help' or arg=='' or not arg in data:
  return f'-title "{name} spends their free time getting help." -desc "`!downtime <activity>` where <activity> is one of {", ".join(data.keys())}."'

# check pre conditiosn
activity=data[arg]
particle=activity.get('particle',arg+'ing')
ccn='Downtime'
if not cc_exists(ccn):
  return f'-title "{name} has no time to {arg}" -desc "You need to have a Downtime counter to spend.\nYou can create it with `!cc create Downtime -max 4 -reset long`"'
downtime=get_cc(ccn)
if not downtime:
  return f'-title "{name} doesn\'t have time to {arg}" -desc "You have no more downtime left." -f Downtime|"{cc_str(ccn)}"|inline'

# Preconditions met, roll for results
title=f'{name} spends their free time {particle}. '
node = activity
code,desc,fields='','',''
while node:
    code+=node.get('code','')
    desc+=node.get('text','')
    table=node.get('table')
    if table:
        if roll_expr:=table.get('roll'):
            r = vroll('+'.join([str(get(e)) + f"[{e[:3]}]" if e.isidentifier() else e for e in roll_expr.split('+')]))
            fields += f'-f Roll|"{r.full}"|inline '
        elif skill:=table.get('skill'):
            r = vroll( character().skills[skill].d20())
            fields += f'-f {skill}|"{r.full}"|inline '
        else:
            r = vroll('1d100')
            fields += f'-f Roll|"{r.full}"|inline '
        options=[int(k) for k in table.keys() if (k.isdigit() or (k[0]=='-' and k[1:].isdigit())) and int(k) <= r.total]
        if options:
            node=table[str(max(options))]
        else:
            node=None
    else:
        node=None

# Apply the changes to the character
mod_cc(ccn,-1,True)

fields+=f'-f Downtime|"{cc_str(ccn)}"|inline '
if not desc:
   desc='Nothing happens.'
return f'-title "{title}" {code} -desc "{desc}" '+fields
</drac2>