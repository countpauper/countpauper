!alias step cast "Misty Step" -i
{{cc,c, ignore="Misty Step",character(),'-i' in &ARGS& }}
{{err(f'Fey Touched: {cc} is no longer available today.') if not c.get_cc(cc) and not ignore else '' }}
-f "Fey Touched|You learn the Misty Step spell. You can cast this spell without expending a spell slot once until you finish a long rest. "
{{f'{c.mod_cc(cc,-1)} -f "{cc}|{cc_str(cc)}" ' if not ignore else ''}}
%*%
