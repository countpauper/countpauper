!alias sleep <drac2>
args=&ARGS&
if len(args)<2 or args[0].lower() in "help?" or not args[0].isdecimal():
	return f'''embed -title "{ctx.author.name} is feeling sleepy" -desc "This alias is for checking HP totals against the total rolled from the sleep spell. After you've cast the *sleep* spell, input the rolled total as the first argument and put your targets as any other arguments. If you don't put any targets or none of your targets are found in the combat, the alias will just evaluate everyone in the combat. e.g. `!sleep 40 go1 go2 or2 or3 og1`" '''
total=int(args.pop(0))
c=combat()
if not c:
	return f'echo The channel has to be in combat.'
if missing:=[x for x in args if not c.get_combatant(x)]:
	return f'echo The following targets were not recognized: `{",".join(missing)}`'
targets=[c.get_combatant(x) for x in args if c.get_combatant(x)]
ranked=[]
[ranked.append(x) for y in range(1,total+1) for x in targets if x.hp==y and str(x.creature_type).lower()!='undead']
Sum=0
affected=[set("Sum",Sum+x.hp) or x for x in ranked if Sum+x.hp<=total]
[x.add_effect("Sleeping","",10) for x in affected]
nl='\n'
target_result={x.name:x.get_effect('sleep') if x in affected else None for x in targets}
target_desc=[f'{x} - {e if e else "*Not affected*"}' for x,e in target_result.items()]
return f'''embed -title "{name} put {total} HP to sleep." -desc "{nl.join(target_desc)}" '''
</drac2>



!alias sleep embed {{args=&ARGS&}}{{help=not args or args[0].lower() in "help?"}}{{total=int(args[0].isdigit() and args.pop(0) if args else 0)}}{{c=combat()}}{{help=help or total==0 or not c}}{{targets=[] if help else [c.get_combatant(x) for x in args if c and c.get_combatant(x)]}}{{combatantDict={} if help else {x.name:x.hp for x in targets or (c.combatants if c else [])} }} {{ranked={} }} {{ranker=[ranked.update({x:combatantDict[x] for x in combatantDict if combatantDict[x]==y}) for y in range(total)]}}{{Sum=0}} -f "Ranked|{{ranked}}" {{affected=[set("Sum",Sum+combatantDict[x]) or x for x in ranked if (Sum+combatantDict[x])<total] }}
-title "<name> {{'is feeling sleepy' if help else f'rolled to affect {total} HP total'}}..."
{{[c.get_combatant(x).add_effect("Sleeping","",10) for x in affected]}}
-desc "{{'''This alias is for checking HP totals against the total rolled from the sleep spell. After you've cast the *sleep* spell, input the rolled total as the first argument and put your targets as any other arguments. If you don't put any targets or none of your targets are found in the combat, the alias will just evaluate everyone in the combat. e.g. `!sleep 40 go1 go2 or2 or3 og1`''' if help else ('**Targets:**\n'+'\n'.join([x.name for x in targets])+'\n\n' if targets else '')+'**Good Night!**\n'+'\n'.join([f'''{x}: {c.get_combatant(x).get_effect('sleep')}''' for x in affected])}}"
