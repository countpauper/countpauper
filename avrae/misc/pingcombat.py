# this doesn't work, because when avrae sends command output to the console, it doesn't allow discord to mention

!alias pc echo <drac2>
controllers = {f'<@!{c.controller}>' for c in combat().combatants}
return ", ".join(controllers)
</drac2>

# Channels work at least

!test {{ f'<#{ctx.channel.id}>' }}

# roles would be
<@&role id>
# but no way to get them in code, just discord right click
