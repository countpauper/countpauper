!alias mushroom embed
{{cc,char,arg="Mushroom",character(),"&*&"}}
{{v=char.cc_exists(cc) and char.get_cc(cc)}}
{{t=char if not arg else (combat().get_combatant(arg) if combat() else (char if char.name.startsWith(arg) else None))}}
{{n=t.name if t else name}}
{{t.modify_hp(1, overflow=False) if v and t else ""}}
{{char.mod_cc(cc, -1) if v else ""}}
-title "{{f"{n} {'eats a' if v else 'would love a'} {cc}!"}}"
-desc "{{f"Eating a mushroom restores 1 hit point, and the mushroom provides enough nourishment to sustain a creature for one day. The mushrooms lose their potency if they have not been consumed within 24 hours of the casting of this spell." if v else "You have no more mushrooms." if cc_exists(cc) else "You don't have a counter named Mushroom."}}"
{{f' -f "{cc}|{cc_str(cc)}|inline"' if char.cc_exists(cc) else ''}}
{{f' -f "HP|{t.hp_str()}|inline"' if t else ''}}
-thumb https://img0.etsystatic.com/066/0/5384143/il_570xN.780860730_bqiq.jpg