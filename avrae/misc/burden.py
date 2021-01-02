!alias burden embed -title "Shared Burden"
-desc "Whenever you would deal damage to a creature with a warlock spell or cantrip, you can choose to add your Charisma modifier [{{charismaMod}}] to the spell's damage. When you do, you take psychic damage equal to half your Charisma modifier, rounded up. This psychic damage ignores resistances and immunities."
-thumb https://i.redd.it/2vkwv8q2eao21.png
-footer "Item: Shared Burden, the Griffon's Saddlebag"
# parse the arguments into a ParsedArgumens
{{args=argparse(&ARGS&)}}
# Find a list of all -t targets
{{targets=args.get('t')}}
# assign combat, but mostly error out early if no targets are given, drac1 is jank for trying to early out
{{c=combat() if targets else err(f'You must share your burden with someone. `!{ctx.alias} -t <target> [-t <additional>...] [-rr <repeat>]`')}}
# convert the string list of targets to SimpleCombatants using list comprehension, if possible (only in combat, else error out)
{{targets=[c.get_combatant(t) for t in targets] if c else err('You can only share your burden in combat')}}
# find the -rr argument with default 1
{{mul=args.last('rr',1,int)}}
# use an f-string for the dice for damage to yourself, multiplied by number of targets and rr
{{myshare=f'{len(targets)*mul}*{int(ceil(charismaMod/2))}'}}
# apply the damage to self and create a field with the name, damage and end resulting hp, using  a tripple quoted f-string for all the nested sub strings
{{f""" -f "{name} [{combat().me.damage(myshare)['total']}]|{c.me.hp_str()}|inline" """ if c.me else err('You must be in combat to share your burden.')}}
# use another f-string for the damage to each target
{{dmg = f'{charismaMod}*{mul}'}}
# join multiple f-strings to fields, for each targets, using a list comprehension, same format of the field as above.
{{' '.join([f"""-f "{t.name}  [{t.damage(dmg)['total']}]|{t.hp_str()}|inline" """ for t in targets])}}
