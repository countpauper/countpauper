tembed <drac2>
# TODO data https://www.dndbeyond.com/sources/xgte/downtime-revisited#undefined
#   Sub commnd xge to enble, phb for phb downtime nd dmg for dmg downtime stuff set in svar
# 'default' key for if no result applies (instead of stupid -100)
# gvar for downtime and craft with user facing skill names
# besides code, can have 'item' (auto added to bag) and other stuff
#	- item name added to description automatically like scroll found reading
#    - quantity for item, can be roll, roll string can contain var identifiers set by {code} and other cvars or quantity:numnber or quantity:str
#	- can also be gp or sp or cp, simply added to coin pouch as item, recognize by name? or 'coin' or find existing item to increment, else default bag
#	- also lose item and coins as bad stuff, if don't have, clip count? no result?
# image thumb per activity
# image per result (overrides or -image and activity is -thumb or img= and thumb= )
# exhaustion (or in general consumable +-) as a result option (nb avrae dev !exhaustion alias with cc ties into !lr already, just need to be compatible)
# tool precondition (fishing: tackle, reading: book, hunting bow or trap, burglary=crowbar, working with tool: tool)
# "Inspiration" snippet & cc, inspiration as a result option (NB inspiration effect and snippet used for bard)
# relation added to !relation, need a table for location?(channel)

# and make sure most of that is possible
sv = load_json(get_svar('downtime','{}'))
gv= sv.get('activities',['b566982b-c7f8-4ea4-af47-4fd82623b335'])
data={}
for dgv in gv:
	data.update(load_json(get_gvar(dgv)))

# parse arguments
arg="&*&".lower()
if arg=='?' or arg.lower()=='help' or arg=='' or not arg in data:
  return f'-title "{name} spends their free time getting help." -desc "`!downtime <activity>` where <activity> is one of {", ".join(data.keys())}."'

# recreate downtime consumable with current config, keeping value
char=character()
cc_max = sv.get('max',1)
cc_max = int(cc_max) if str(cc_max).isdigit() else None
cc_reset=sv.get('reset','long').lower()
ccn='Downtime'
if char.cc_exists(ccn):
    cc_val=char.get_cc(ccn)
    char.delete_cc(ccn)
else:
    cc_val=None
if cc_max>0:
    char.create_cc(ccn, maxVal=cc_max, reset=cc_reset)
    if cc_val is not None:
        char.set_cc(ccn,cc_val)

# check pre conditions
activity=data[arg]
particle=activity.get('particle',arg+'ing')
downtime=char.get_cc(ccn)
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
char.mod_cc(ccn,-1,True)
fields+=f'-f Downtime|"{cc_str(ccn)}"|inline '
if not desc:
   desc='Nothing happens.'
return f'-title "{title}" {code} -desc "{desc}" '+fields
</drac2>