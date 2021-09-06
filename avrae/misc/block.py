!alias block <drac2>
C=combat()
if not C:
	return f'echo Block initiative only works after `!init begin`.'
args=argparse('''&*&''')
# TODO: schemes:
#  -dm (id=current/map} DM vs rest
#  -b (number) per assume 5 maximum >=30,25-29,20-24 etc to -5 to 0, <-5
#  combined. all players in a block then dms in that block (but whoever is first)
# !block lock (-d minutes>=10) start taking your turn, warns others that want to lock based on time(). Unlock on !block next or
# priority dms: argument
dms=args.get('dm')
# 2nd attempt: map or dm combatant's controller
if not dms:
	dm_names=['dm','map']
	dms=[c.controller for c in C.combatants if c.name in dm_names]
# 3d: caller
if not dms:
	dms = [ctx.author.id]

# TODO: add extra conditions to split blocks
# initiatve tresholds
# maximum number of members/groups
# don't split groups down the middle

blocks=[]
dm_block=None
current_block=None
for c in C.combatants:
	c_is_dm=c.controller in dms
	if dm_block is c_is_dm:
		block.append(c)
	else:
		block=[c]
		blocks.append(block)
		dm_block = c_is_dm
	if C.current.name==c.name:
		current_block=blocks[-1]
descs=[]
for b in blocks:
	blockdesc=', '.join(c.name for c in b)
	if b is current_block:
		descs.append(f'**{blockdesc}**')
	else:
		descs.append(blockdesc)
desc='\n'.join(descs)

# TODO: if nexting, check (use metadata block_init) if all are done, then move to next block top. then n n n for all members
# reinitialize the list of done turns in metadata for the new block
# then
nexts=None
if current_block:
	nexts=([idx for idx,c in enumerate(current_block) if C.current.name==c.name]+[None])[0]
return f'echo {desc} nexts {nexts}'
</drac2>