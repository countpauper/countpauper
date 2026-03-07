!snippet eldritch <drac2> 
c=character()
ccname = "Eldritch Staff"
try:
    cc=c.cc(ccname)
    if cc.value<=0:
        return f'-f "{ccname}|out of charges"'
    cc.mod(-1)
    dmg = '-d 1d8[lightning]'
    if cc.value>0:
        return f'{dmg} -f "{ccname}|{cc}"'

    r = vroll('1d20')
    destroyed = ": *Destroyed!*" if r.total==1 else ""
    return f'{dmg} -f "{ccname}|Last charge used {r}{destroyed}"'
except "ConsumableException":
    return f'-f "{ccname}|No counter"'
</drac2>
