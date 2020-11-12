<drac2>
cc='DM Inspiration'
char=character()
if not char.cc_exists(cc) or  char.get_cc(cc)<=0:
  return f'echo {cc} is not available. Good luck?'
elif character().death_saves.is_stable():
  return f'echo You are already stable and shouldn\'t waste your {cc}.'
elif character().death_saves.is_dead():
  return f'echo You are already dead. Not even {cc} can save you now.'
else:
   character().mod_cc(cc,-1)
   return f'game ds adv -phrase "{cc} used. Remaining: {cc_str(cc)}" '
</drac2>