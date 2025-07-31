<drac2>
# !alchemy <item> ... use spell slot of at least required level to get each item 
# !alchemy rest <item> [<...>] use sum of charges to get those items
# !alchemy <table> to show the table 
# !alchemy <table> <level> ... Use spell slots of at least level to randomly roll on that table. Tables: q[uaff] t[hrow] 
# !cc create Alchemy -min 0 -max {(ArtificerLevel +3)/4} -type bubble -reset long
using(baglib="4119d62e-6a98-4153-bea9-0a99bb36da2c")

# Load data
tabledata = get_gvar('8680a6a7-d723-4de0-96f7-d24c636af15d')
tabledata = [[i.strip() for i in line.split(',')] for line in tabledata.split('\n')]
tablenames = {l[0].lower() for l in tabledata}
tables = {table: [l[1:] for l in tabledata if l[0].lower() ==table] for table in tablenames }
ch = character()
charges_cc = 'alchemy'
nl='\n'

# Helper functions
def find_potion(name):
    for table, rows in tables.items():
        for row in rows:
            for idx, col in enumerate(row):
                if name.lower() in col.lower():
                    return table, idx+1, col
    return None, None, None

def format_row(columns):
    return f'    {columns[0]} | '+ ' | '.join(f'{col[:12]:>12}' for col in columns[1:])

def format_bill_item(item):
    return f'`{item[-1]}` ({item[0]})'

# Format list with [(cost, [...,] item), ...]
def format_bill(items, sep=', '):
    if not items:
        return "nothing"
    if len(items)==1:
        return format_bill_item(items[0])
    if len(items)==2:
        return ' and '.join(format_bill_item(i) for i in items)
    return sep.join(format_bill_item(i) for i in items[:-1])+f', and {format_bill_item(items[-1])}'

def format_expired(expired, sep=nl):
    if not expired:
        return ""
    return f"Old potions have expired:\n" + sep.join(f'{count}x {item}' for item, count in expired.items()) + nl
 
def format_resources(sep='\t'):
    return f'`{charges_cc}` {ch.cc_str(charges_cc)}{sep}'+sep.join(ch.spellbook.slots_str(n) for n in range(1,6) )

# Parse arguments
syntax = f'!{ctx.alias} [<item>] [<rest>] [<item>] ... [<table> [<level>]] ...  '
args=&ARGS&
selected_table = None

charges = None   # charges that are used
slots = []     # slots that are used
potions=[]
rest=False
for arg in args: 
    if arg.lower() == 'rest':
        rest=True
        charges = charges or []
        continue
    for t in tablenames: 
        if t.startswith(arg.lower()):
            selected_table = t
            break
    else:
        if arg.isdecimal() and selected_table:
            slot = int(arg)
            slots.append((slot, selected_table))
        else:
            table, level, full_name = find_potion(arg)
            if table is None:
                return f"echo Unknown potion `{arg}`"
            if charges is not None:
                charges.append((level, full_name)) 
            else:
                slots.append((level, full_name))
# sort both costly to cheap to reduce unfafourable rounding
if slots:
    slots.sort()
    slots.reverse()
if charges:
    charges.sort()
    charges.reverse()

# Early output of information 
if not charges and not slots:
    if selected_table:
        selected_rows = tables.get(selected_table, [])
        die = len(selected_rows)
        header = [f'1d{die}']+[f'{charge} Charge' for charge in range(1,6)]
        output = format_row(header) + '\n' + nl.join(format_row([idx]+row) for idx, row in enumerate(selected_rows))
        return f'echo {selected_table}\n```{output}\n``` {format_resources()}'
    else:
        return f'echo `{syntax}`\n{format_resources()}'

# Check Resources 

total_bill=[]

if charges: 

    available_charges = ch.get_cc(charges_cc)
    total_charges = sum(c for c, _ in charges) 
    if total_charges > available_charges:
        return f'echo {ch.cc_str(charges_cc)} {charges_cc} charges is not enough for {format_bill(charges)}'
    total_bill+=[(f'{c} {charges_cc}', i) for c, i in charges]
else:
    total_charges=None

if slots:
    cost_slots = []
    sb = ch.spellbook
    available_slots = [sb.get_slots(lvl) for lvl in range(1,6)]
    for required_lvl, input in slots:
        for lvl, availlable in enumerate(available_slots):
            if not availlable:
                continue
            if required_lvl > lvl+1:
                continue 
            available_slots[lvl]-=1
            cost_slots.append((input, lvl+1))
            break
        else:
            return f'echo {sb.slots_str(required_lvl)} is not enough for {format_bill(cost_slots+[(input, required_lvl)])}'
    
    # roll tables and deduct slots 
    for input, slot in cost_slots:
        if random_table:=tables.get(input):
            die = vroll(f'1d{len(random_table)}')
            row = random_table[die.total-1]
            col_idx = min(slot, len(row))-1
            total_bill.append((f'lvl {slot} spell slot to roll {str(die)} from {input}', row[col_idx]))
        else:
            total_bill.append((f'lvl {slot} spell slot', input))
        sb.use_slot(slot)

if total_charges:
    ch.mod_cc(charges_cc, -total_charges, True)


# Add items to the bag 
bags = baglib.load_bags(ch)
potion_bag_name="Potion Belt"
potion_bag = baglib.get_bag(bags, potion_bag_name, True, True)
if rest: 
    expired = dict(potion_bag[1])
    potion_bag[1]={}
for _, item in total_bill:
    potion_bag[1][item] = potion_bag[1].get(item, 0) + 1
baglib.save_bags(bags, False, ch)

return f"""echo {format_expired(expired)}
New potions added to {potion_bag_name}:
{format_bill(total_bill, nl)}
{format_resources()}
"""

</drac2>