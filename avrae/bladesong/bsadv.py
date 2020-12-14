<drac2>
if ctx.alias in ('s','save'):
	err(f"The `bsadv` snippet  can only be used with `!check`. You can use `!{ctx.alias} con bladesong` instead. ")
if ctx.alias in ('a','attack'):
	err(f"The `bsadv` snippet  can only be used with `!check`. You can use `!{ctx.alias} <attack> bladesong` instead. ")

n, c = 'Bladesong', combat()
me = c.me if c else None
if me and not me.get_effect(n):
	return '-f Bladesong|"Not Active" '

return 'adv -f Bladesong|Advantage '
</drac2>