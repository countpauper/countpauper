!snippet clockwork <drac2> 
c=character()
ccname = "Clockwork Amulet"
try:
    cc=c.cc(ccname)
    cc.mod(-1, strict=True)
    return f'-attackroll 10 -f "{ccname}|{cc}"'
except "ConsumableException":
    return f'-f "{ccname}|No counter"'
</drac2>
