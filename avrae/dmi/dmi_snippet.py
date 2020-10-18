{{cc='DM Inspiration'}}
{{'adv -phrase "DM Inspiration" ' if character().cc_exists(cc) and character().get_cc(cc)>0 else '' }}
{{character().mod_cc(cc,-1)}}
{{f' -f "{cc}"|"{cc_str(cc)}" '}}