{{cc='DM Inspiration'}}
{{c=character()}}
{{f'adv -phrase "DM Inspiration" {c.mod_cc(cc,-1)} -f "{cc}"|"{c.cc_str(cc)}"' if c.cc_exists(cc) and c.get_cc(cc)>0 else f'-f "{cc}"|"Not Available" ' }}