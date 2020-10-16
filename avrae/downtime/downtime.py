tembed <drac2>
# TODO data https://www.dndbeyond.com/sources/xgte/downtime-revisited#undefined
#    - 'default' key for if no result applies (instead of stupid -100)
# gvar for downtime and craft with user facing skill names
# besides code, can have 'item' (auto added to bag) and other stuff
#	- item name added to description automatically like scroll found reading
#    - quantity for item, can be roll, roll string can contain var identifiers set by {code}
#	- can also be gp or sp or cp, simply added to coin pouch as item, recognize by name? or 'coin'
#	- also lose item and coins as bad stuff, if don't have, clip count? no result?
# tool precondition (fishing: tackle, reading: book, hunting bow or trap, burglary=crowbar, working with tool: tool)
# exaustion +- as a result option (nb avrae dev !exhaustion alias with cc ties into !lr already, just need to be compatible)
# "Inspiration" snippet & cc, inspiration as a result option (NB inspiration effect and snippet used for bard)
# relation added to !relation, need a table for location?(channel)
# server configuration for downtime parameters (default 1/long rest)
#   also for train and craft
#   Automatically created instead of complain
# image thumb per activity
# image per result

# and make sure most of that is possible

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