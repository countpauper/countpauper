!alias hd_hp <drac2>
args=&ARGS&
if not args:
	return f'echo `{ctx.prefix}{ctx.alias} xdy +z` where x is the number of hitdice, y the hit die and z the constitution bonus'
dice, die_size = args[0].split('d')
dice=int(dice)
die_size=int(die_size)
con = int(args[1]) if len(args)>1 else 0
con_bonus=dice*con
hp=(dice*(die_size+1))//2 + con_bonus
return f'echo {dice}d{die_size}+{con_bonus} = {hp}'
</drac2>