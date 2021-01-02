!alias disguise cast
{{spell='Disguise Self'}}
{{spell}} -i
{{cc,c,ignore=spell,character(),'-i' in &ARGS&}}
-f "Shadow Touched|You learn {{spell}}. You can cast this spell without expending a spell slot once until you finish a long rest."
{{'' if ignore else c.mod_cc(cc,-1) if c.get_cc(cc) else err(f'Shadow Touched: {cc} is no longer available today.')}}
-f "{{cc}}|{{c.cc_str(cc)}}"
%*%