<drac2>
# Bladesong alias routine
# TODO
# simplify reusing !bladesong (with a use) when no arguments and no effect active
# or use bladesing -i convention to 'reuse'
# bladesong defense sub command
# help removed from command
# bladesong snippet for advantage or bonus, hopefully after https://github.com/avrae/avrae/issues/1294

arg=@@@

counter_name="Bladesong"
var_name="Bladesong"
if WizardLevel>1:
  create_cc_nx(counter_name, minVal=0, maxVal=2, reset='short', dispType='bubble')
  bladesongs = get_cc(counter_name)
else:
  bladesongs=None
  active=False
active = get(var_name,"False").lower()=="true"

# Help text in description if argument is help or ?
if arg and (arg[0].lower()=="help" or arg[0]=="?"):
  title = "-title Bladesong"
  description="■ `!bs` or `!bladesong`: Uses bladesong effect with 1 minute remaining.\n"
  description+="■ `!bladesong help` or `!bladesong ?`: this help text.\n"
  description+="■ `!bladesong <turns>`: Activates bladesong in combat, with a number of <turns> remaining. This can be used if bladesong was started before combat.\n"
  description+="■ `!bladesong off|0`: Deactivates bladesong. \n"
  description+="■ `!bladesong defense <level>`: use song of defense, expending the given spell <level> to reduce damage if bladesong is active.\n"
  description+="** Related Snippets**\n"
  description+="■ `!attack <weapon> victory`: add victory damage bonus (int) if bladesong is active.\n"
  description+="■ `!save Con bsbonus`: add your intelligence bonus to a concentration check if bladesong is active.\n"
  description+="■ `!check Acrobatics bsadv`: Advantage on acrobatics check if bladesong is active.\n"
  fields = "-f \"Remaining Uses\"|"
  if bladesongs!=None:
    fields += cc_str(counter_name)
  else:
    fields += "\"Not Available\""
else:
  effect_name="Bladesong"
  c=combat()
  me = c.me if c else None
  if me:
    song_in_effect=me.get_effect(effect_name)!=None
  else:
    song_in_effect=False
  if bladesongs==None:
    title = "-title Bladesong"
    description="This is only available to Bladesingers."
  elif arg and (arg[0].lower()=="defense"):
    title = "-title \"Song of Defense\""
    fields=""
    if WizardLevel<10:
      description="Song of defense requires wizard level 10 or higher."
    elif (c and not song_in_effect) or (not c and not active):
      description="Song of defense can only be used while bladesinging."
    else:
      slot = int(arg[1])
      if slot<1:
        description="You can not use a cantrip to absorb damage."
        dmg_absorbed = None
      else:
        max_slots = character().spellbook.get_max_slots(slot)
        if max_slots<=0:
          description="You do not have level "+str(slot)+" spell slots to absorb damage."
          dmg_absorbed = None
        else:
          remaining_slots = character().spellbook.get_slots(slot)
          if remaining_slots<=0:
            description="You do not have a level "+str(slot)+" spell slot left to absorb damage."
            dmg_absorbed = None
          else:
            description="You direct your magic to absorb damage while your Bladesong is active. When you take damage, you use your reaction to expend one spell slot and reduce that damage to you by an amount equal to five times the spell slot's level."
            dmg_absorbed = slot * 5
            remaining_slots -= 1
            set_slots(slot, remaining_slots)
          slot_str="◉"*remaining_slots + "〇"*(max_slots-remaining_slots)
          fields +="-f \"Spell Slot ["+str(slot)+"]\"|"+slot_str+"|inline"
      fields += " -f \"Damage Absorbed\"|"+str(dmg_absorbed)+"|inline"
  else:
    if not arg:
      turns=10
    elif arg[0]=="off":
      turns = 0
    else:
      # reapply when entering combat
      turns=int(arg[0])
    if turns>0:
      if not active:
        if bladesongs>0:
          mod_cc(counter_name, -1, True)
          set_cvar(var_name,"True")
          title = "-title Bladesong"
        else:
          title = "-title \"Bladesong not available.\""
          turns=None
      else:
        title = "-title \"Bladesong Active\""
    fields = "-f \"Remaining Uses\"|" + cc_str(counter_name)+"|inline"
    if turns is None:
      description="No more uses of bladesong remaining until the next short rest."
    elif turns>0:
      description="You invoke a secret elven magic called the Bladesong, provided that you aren't wearing medium or heavy armor or using a shield. It graces you with supernatural speed, agility, and focus.\n"
      description+="You can use a bonus action to start the Bladesong, which lasts for 1 minute. It ends early if you are incapacitated, if you don medium or heavy armor or a shield, or if you use two hands to make an attack with a weapon. You can also dismiss the Bladesong at any time you choose (no action required).\n"
      if WizardLevel>=10:
        description+="**Song of Defense**\n"
        description+="As a reaction, expend a spell slot to reduce damage by 5x the slot's level. `!bladesong defense <level>`.\n"

      # You gain a bonus to your AC equal to your Intelligence modifier (minimum of +1).
      ac_bonus = max(intelligenceMod,1)
      if me:
        me.remove_effect(effect_name)
        ac = me.ac
      else:
        ac = armor
      fields+=" -f AC|\""+str(ac)+" + "+str(ac_bonus)+"(int) = "+str(ac+ac_bonus)+"\"|inline"
      if me:
        me.add_effect(effect_name,"-ac +"+str(ac_bonus),turns)
      # Your walking speed increases by 10 feet
      speed_bonus = 10
      speed = get('speed',None)
      if speed:    # common cvar, not adjusted because it's hard to restore
        current_speed = int(speed.split(sep=" ")[0])
        new_speed = str(current_speed+speed_bonus)+" ft."
        fields+=" -f Speed|\""+str(current_speed)+" + "+str(speed_bonus)+" = "+new_speed+"\"|inline"
      else:
        fields+=" -f Speed|\"+ "+str(speed_bonus)+" ft.\"|inline"
      # You have advantage on Dexterity (Acrobatics) checks.
      fields+=" -f \"Acrobatics Advantage\"|\"`!check Acrobatics bsadv`\"|inline"
      # You gain a bonus to any Constitution saving throw you make to maintain your concentration on a spell equal to your Intelligence modifier (minimum of +1)
      con_bonus = max(intelligenceMod,1)
      fields+=" -f \"Concentration +"+str(con_bonus)+"(int)\"|\"`!save Con bsbonus`\"|inline"
      # Song of victory
      if WizardLevel>=14:
        dmg_bonus = max(intelligenceMod,1)
        fields+=" -f \"Song of Victory\"|\"Melee damage +"+str(dmg_bonus)+"\n`!a ... victory`\"|inline"
    else:
      description="Your bladesong ends if you are incapacitated, if you don medium or heavy armor or a shield, or if you use two hands to make an attack with a weapon. You can also dismiss the Bladesong at any time you choose (no action required).\n"
      title = "-title \"Bladesong ends\""
      if me:
        me.remove_effect(effect_name)
        ac = c.me.ac
      else:
        ac = armor
      set_cvar(var_name,"False")
      fields += " -f AC|"+str(ac)+"|inline"
return title+" -desc \""+description+"\" "+(fields if exists("fields") else "")
</drac2>