<drac2>
args="&*&"
guild=ctx.guild
id=str(guild.id) if guild else 'None'
guild=guild.name if guild else 'Private Message'
uvar_name='me'
us = load_json(get(uvar_name,'{}'))
if args:
    if args.lower()=='set':
        args=name
    us[id] = args
    set_uvar(uvar_name,dump_json(us))
    return f'echo Set character for {guild} to {args}. Use `!me` to switch. Your current character is {name}.'
if me:=us.get(id):
    return f'char {me}'
return f'echo No character selected for {guild}. Use `{ctx.prefix}{ctx.alias} set` or `{ctx.prefix}{ctx.alias} <character name>`'
</drac2>