!alias math <drac2>
# use as a generic calculator with a C++ like syntax
# +-*/^ operators PEMDAS ^power (not ** python like)
# braces
# special: % divides the preceding number by 100.0
# later: constants e, pi,
# later: ln log sin cos tan etc but not in draconic so needs approximation.
# later ! operator, logic/binary operators, binary and hex numbers

formula='''&*&'''
formula = formula.replace(' ','') # remove whitespace

###  parse the formula into a tree: operator, left, [right]

# first parse parenthesis, returns a list with nested lists and strings
# every list corresponds to one matched pair of parentheses, except the outer list
# this is done with recursion and every call returns a string prefix, a list and a string posfix
# the strings may be empty

# helper to find nested enclosures, returns the position past the last close
def find_enclosure(s,open, close):
    idx = s.find(open)
    if idx==-1:
        return len(s)
    idx+=len(open)
    level=1
    while level>0 and idx>0:
        close_idx=s.find(close, idx)
        next_idx=s.find(open, idx)
        if next_idx!=-1 and next_idx<close_idx:
            level+=1
            idx=next_idx+len(open)
        elif close_idx!=-1:
            level-=1
            idx=close_idx+len(close)
        else:
            err(f"Missing closing '{close}` at {s} at level {level}")
    return idx

def parentheses(s):
    start=s.find('(')
    if start==-1:
        return s
    end=find_enclosure(s[start:],'(',')') + start
    prefix = s[0:start]
    middle= parentheses(s[start+1:end-1])
    postfix = parentheses(s[end:])

    if prefix and postfix:
        return prefix, [middle], postfix
    elif prefix:
        return prefix, [middle]
    elif postfix:
        return [middle], postfix
    else:
        return middle

return f'echo {parentheses(formula)}'
</drac2>
