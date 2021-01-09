<drac2>
args="&*&"
guild=ctx.guild
uvar_name='me'
us = load_json(get(uvar_name,'{}'))
if args:
    us[str(guild.id)] = args
    set_uvar(uvar_name,dump_json(us))
    return f'echo Set character for {guild.name} to {args}'
if me:=us.get(str(guild.id)):
    return f'char {me}'
return f'echo No character selected for {guild.name}. Use `{ctx.prefix}{ctx.alias} <character name>`'
</drac2>