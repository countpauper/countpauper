embed {{ cc="DM Inspiration"}}
{{ character().create_cc_nx(cc,maxVal=1,minVal=0,dispType='bubble') }}
{{ character().set_cc(cc,1) }}
{{ f'-title "{name} receives DM inspiration!" '}}
{{ f'-f "{cc}"|"{cc_str(cc)}" '}}
{{ arg="&*&" }}
{{ f'-desc "{arg}" ' if arg else '' }}