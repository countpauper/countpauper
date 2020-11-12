<drac2>
cc='DM Inspiration'
char=character()
alias_name = ctx.alias.split()[0]
if not char.cc_exists(cc) or char.get_cc(cc)<=0:
	return f'echo You need to have Dungeon Master Inspiration to reward another player. Use `!{alias_name}` if you\'re supposed to have it.'
target="&*&"
if not target:
	return f'echo Use `!{ctx.alias} <name>` to use your own Dungeon Master Inspiration to reward another player.'
char.mod_cc(cc,-1)
return f'embed -title "{name} rewards {target} with Dungeon Master Inspiration" -desc "{target} can use `!{alias_name}` to receive it and the `dmi` snippet to use it." -f "{cc}|{char.cc_str(cc)}"'
</drac2>