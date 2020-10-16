# TODO: target:
#in comabt	target set to not dying if in combat, also with healer feat can heal to +1 or extra
#   it needs SimpleCombatant.deathsaves to stabilize targets with a heal kit :https://github.com/avrae/avrae/issues/1279

<drac2>
# Healing kit alias routine
arg=@@@

# Help text in description if argument is help or ?
if arg and (len(arg)>1 || (arg[0]=="help" or arg[0]=="?")):
  description="■ `!hk`: Show the number of healer's kit uses left.\n"
  description+="■ `!hk help` or `!hk ?`: Show this help text.\n"
  description+="■ `!hk <target>` Heals the given target. Outside of combat this will only subtract a use and show the result. It has to be manually applied. In combat the target is stabilized and healed.\n"
  description+="  Stabilize: A dying creature becomes stable at 0hp or 1hp if you have the healers feat.\n"
  description+="  If you have the healer feat: As an action, you can spend one use of a healer's kit to tend to a creature and restore 1d6 + 4 hit points to it, plus additional hit points equal to the creature's maximum number of Hit Dice. **The creature can't regain hit points from this feat again until it finishes a short or long rest.**\n"
  description+="to add the healers feat use `!manage feat add Healer` or `!cvar feats Healer`.\n"
else:
  description="Todo"
return +-description "+description
</drac2>