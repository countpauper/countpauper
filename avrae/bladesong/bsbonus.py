{{ n, c = 'Bladesong', combat() }}
{{ me = c.me if c else None }}
{{ sing = (me and me.get_effect(n)) or (not me and get("Bladesong","").lower()=="true") }}
{{ bonus = max(intelligenceMod,1) }}
{{ ("-b "+str(bonus)+"[Bladesong] -f Bladesong|\"Bonus +"+str(bonus)+"(int)\"") if sing else "-f Bladesong|\"Not Active\"" }}