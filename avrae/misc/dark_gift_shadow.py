!snippet shadow <drac2>
cc="Shadow Strike"
c=character()
if not c.cc_exists(cc):
	return f'-f "Shadow Strike|You do not have a living shadow.\nUse `!cc create \\"Shadow Strike\\" -min 0 -max proficiencyBonus -type bubble -reset long`" '
if c.get_cc(cc)<=0:
	return f'-f "Shadow Strike|{c.cc_str(cc)} - Your living shadow is tired. You can use this feature a number of times equal to your proficiency bonus, and you regain all expended uses when you finish a long rest." '
c.mod_cc(cc,-1)
return f'-f "Shadow Strike|{c.cc_str(cc)} - When you make a melee attack roll, you can *increase your reach for that attack by 10 feet*. Your shadow stretches and delivers the attack as if it were you." '
</drac2>