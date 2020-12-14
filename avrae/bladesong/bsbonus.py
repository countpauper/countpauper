<drac2>
if ctx.alias in ('c','check'):
	err(f"The `bsbonus` snippet can only be used with `!save` for a bonus to concentration checks. You can use `!{ctx.alias} acro bladesong` instead. ")
if ctx.alias in ('a','attack'):
	err(f"The `bsbonus` snippet can only be used with `!save` for a bonus to concentration checks. You can use `!{ctx.alias} <attack> bladesong` instead. ")
n, c = 'Bladesong', combat()
me = c.me if c else None
if me and not me.get_effect(n):
	return '-f Bladesong|"Not Active" '
bonus = max(intelligenceMod,1)
return f'-b {bonus}[Bladesong] -f Bladesong|"Bonus {bonus}(int)" '
</drac2>