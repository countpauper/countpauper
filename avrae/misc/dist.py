!alias dist <drac2>
args=[float(a) for a in &ARGS& if a.replace('.','0',1).isdecimal()]
s = sum(a*a for a in args)
return f'echo The length of ({", ".join(str(a) for a in args)}) = `{sqrt(s)}`'
</drac2>