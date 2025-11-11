<drac2>
# Help 

# `!table [<svar/cvar/uvar>] [<dice>] [<comma,separated>] [...]`

# `<svar/uvar/cvar>`: Roll a random entry from one or more tables. 

# The tables are stored as text with one entry per line in any type of variable. Eg: 
# ```!uvar weapons
# sword 
# spear
# longbow```

# The order of precedence is: svar cvar, uvar. 
# If one of those contains a gvar guid, that is referred to instead. 
# It is not possible to give the gvar guid directly on the command line. 

# `<dice>` : A dice string can be supplied, which is simply rolled if it matches the format `<number>d<...>`.else

# `<comma,separated>` : An ad hoc table can be given as an argument, where each entry is separated with a comma. 

# **Examples** 
# `!table encounter` : will load the `!uvar encounter` and roll a die equal to the number of lines and print a random line.

# `!table encounter_table` : with `!gvar encounter_table 12345678-90AB-CDEF-1234-0ABCDEF` will load the table from that gvar.

# `!table reward 1d6` : will roll a random reward from `!svar rewards` and 1d6 (eg for 1d6*100 gold pieces).

# `!table heads,tails` : will roll random heads or tails.


hexchars = '0123456789abcdef'

def is_gvar(s):
    if len(s)!=63:
        return False 
    if any('-' != s[pos] for pos in (8,13,18,23)):
        return False
    s = s.replace('-', '').lower()
    if any(c not in hexchars for c in s):
        return False 
    return True

# get variable in order: cvar, uvar, svar
# if the content of these is a global variable then it is loaded in its place as a reference 
def load_var(name):
    content = get_svar(name, get(name,  None))
    if content is None:
        return None
    elif is_gvar(content):
        return get_gvar(content, None)
    else:
        return content 

# Return the SimpleRollResult if s matches <numeric>d... for !table encounter 1d6 
# else return a comma separated table for !table cheese,pickles 
def parse_table_argument(s):
    if (d_pos := s.find('d'))>=0 and s[:d_pos].isnumeric() and len(s[d_pos+1:]):
        return vroll(s)
    else:
        return s.split(',') 

# Load a variable name and split the content into a a list of lines
# empty lines are copied from the non empty line above it and trailing spaces are removed
def load_table(name):
    table = load_var(name)
    if table is None:
        return parse_table_argument(name)
    lines = table.split('\n')    
    prev = ""
    lines = [line.strip() or prev for line in lines if (prev:=(line or prev))]
    return lines

def roll_table(table):
    if typeof(table) == 'SimpleRollResult':
        return table, ''
    r = vroll(f'1d{len(table)}')
    return r, table[r.total-1]

table_names = &ARGS&
tables = [(name, load_table(name)) for name in table_names]
#return f"echo {tables}"
results = [(name, roll_table(table)) for name, table in tables]
#return f"echo {results}"
return "echo Tables:\n" + '\n'.join(f'{name}: {result} {r}' for name, (r, result) in results)

</drac2>
