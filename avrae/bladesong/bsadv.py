{{ n, c = 'Bladesong', combat() }}
{{ me = c.me if c else None }}
{{ sing = (me and me.get_effect(n)) or (not me and get("Bladesong","").lower()=="true") }}
{{ "adv -f Bladesong|Advantage" if sing else "-f Bladesong|\"Not Active\"" }}