!servalias drunkard embed
{{cc,ch="Amulet of the Drunkard",character()}}
{{ch.create_cc_nx(cc,0,1, reset='long',dispType='bubble')}}
{{err(f'{cc} is out of charges until the next dawn.') if ch.get_cc(cc)<1 else ''}}
{{ch.mod_cc(cc, -1, True)}}
{{hp_roll=vroll("4d4+4")}}
{{ch.modify_hp(hp_roll.total,overflow=False)}}
-title "<name> has a drink!"
-desc "This amulet smells of old, ale-stained wood. While wearing it, you can regain 4d4 + 4 hit points when you drink a pint of beer, ale, mead, or wine. Once the amulet has restored hit points, it can't do so again until the next dawn."
-f "{{cc}}|{{ch.cc_str(cc)}}|inline"
-f "Healed {{vroll("4d4+4")}}|{{ch.hp_str()}}|inline"
-thumb https://media-waterdeep.cursecdn.com/avatars/thumbnails/9170/66/1000/1000/637199798951570191.png
-footer "Explorer's Guide to Wildemount , pg. 265"
-color <color>
