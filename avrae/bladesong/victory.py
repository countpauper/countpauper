{{ n, c = 'Bladesong', combat() }}
{{ me = c.me if c else None }}
{{ sing = (me and me.get_effect(n)) or (not me and get("Bladesong","").lower()=="true") }}
{{ req = WizardLevel >= 14 }}
{{ bonus = max(intelligenceMod,1) }}
{{ bonusflag = "-b "+str(bonus)+"[Victory] " if (req and sing) else ""}}
{{ field = " -f \"Song of Victory\"|" }}
{{ field += "\"Damage +"+str(bonus)+"(int)\"" if req and sing else ("\"Wizard Level < 14\"" if not req else "\"Bladesong not Active\"") }}
{{ phrase = " -phrase \"Singing victoriously\"" if sing else ""}}
{{ bonusflag+field+phrase }}