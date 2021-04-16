!snippet maguidance -f "Magical Guidance|You can tap into your inner wellspring of magic to try to conjure success from failure. When you make an ability check that fails, you can spend 1 sorcery point to reroll the d20, and you must use the new roll, potentially turning the failure into a success."
<drac2>
if (sl:=int(get('SorcererLevel',0)))<5:
	err(f"Magical guidance is a 5th level sorcerer feature. You are level {sl}")
cc="Sorcery Points"
ch=character()
if not ch.cc_exists(cc):
	err(f"You don't have sorcery points. Use `!level Sorcerer`")
if ch.get_cc(cc)<1:
	err(f"You don't have any sorcery points. You need to rest.")
ch.mod_cc(cc,-1)
return f'-f "{cc}|{ch.cc_str(cc)}"'
</drac2>