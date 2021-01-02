!alias alacrity cast "Gift of Alacrity" -i
{{cc,ccs,c='Gift of Alacrity','Sorcery Points', character() }}
-f "Fey Touched|You learn one 1st level Divination or Enchantment spell. You can cast this spell without expending a spell slot once until you finish a long rest."
{{targets,ignore=argparse(&ARGS&).get('t'),'-i' in &ARGS&}}
{{'' if ignore else c.mod_cc(cc,-1) if c.get_cc(cc) else err(f'Fey Touched: {cc} is no longer available today.')}}
{{targets=targets if targets else [name]}}
{{hint='`!i join -b 1d8`'}}
{{' '.join([f' -f "{t}|{hint}|inline" ' for t in targets])}}
{{twin=len(targets)>1 if not ignore else False}}
{{'' if not twin else c.mod_cc(ccs,-1) if c.get_cc(ccs) else err(f'{ccs} insufficient to twin {cc}.') }}
{{f' -f "Twinned: {ccs}|{c.cc_str(ccs)}" ' if twin else ''}}
-f "{{cc}}|{{c.cc_str(cc)}}"
%*%