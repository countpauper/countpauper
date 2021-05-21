embed {{ cc="DM Inspiration"}}
{{ character().create_cc_nx(cc,maxVal=1,minVal=0,dispType='bubble') }}
{{ character().set_cc(cc,1) }}
{{ f'-title "{name} receives DM inspiration!" '}}
{{ f'-f "{cc}"|"{cc_str(cc)}" '}}
{{ arg="&*&" }}
<drac2>
explain=f"""Use the `dmi` snippet in an `{ctx.prefix}attack`, `{ctx.prefix}check` or `{ctx.prefix}save` to your advantage, for instance `{ctx.prefix}save death dmi`. You can also reward it to another player using `{ctx.prefix}{ctx.alias} reward <target>`"""
return f'-desc "*{arg}*\n{explain}" ' if arg else f'-desc "{explain}"'
</drac2>
