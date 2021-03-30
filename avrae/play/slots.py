<drac2>
cvar='Play'
game_name='bet'
state=load_json(get(cvar,'{}'))
bet=state.get(game_name)
base_cmd=f'{ctx.prefix}{ctx.alias}'
args="&*&"
syntax=f'{base_cmd} slots [payout|<amount>]'
# definition
icon=[':black_small_square:',':cherries:',':lemon:',':melon:',':bell:',':coin:',':gem:']
wheel=[
	[1,2,3,5,1,2,3,6,1,2,3,4,1,2,3,5,1,2,3,6],
	[2,4,1,4,1,4,5,1,4,1,4,1,3,4,1,4,6,4,1,4],
	[2,1,2,3,2,4,2,1,2,3,2,5,2,1,2,3,2,6,2,3]
]
payout={
	'1':2, '11':5,'111':10,'222':10,'226':10, '333':15,'336':15,'444':20,'446':20,'555':25,'666':100
}
numbers=' 123456'
nl='\n'
thumb='-thumb https://cdn.discordapp.com/attachments/755790258154635286/826340323331407912/Slot_Machine.png'
footer=f'-footer "{syntax}, Collect pay out with {base_cmd} collect"'
if args.lower().startswith('pay'):
	desc=[]
	for k,v in payout.items():
		k=f'{k:<3}'
		state_str="|".join(icon[numbers.index(c)] for c in k)
		desc.append(f'|{state_str}| **{v}**')
	return f'embed -desc "{nl.join(desc)}" {footer} {thumb}'

if args:
	if not args.isdigit():
		return f'echo {syntax}'
	amount=int(args)
else:
	amount =1
if amount>50:
	return f'echo You can\'t bet more than 50 at once.'

if bet is not None:
	bet=int(bet)
else:
	return f'echo Before you play slots you have to bet using `{base_cmd} bet <amount>`'

if bet<amount:
	return f'echo You are out. Before you play slots you have to bet more using `{base_cmd} bet <amount>`'

bet-=amount

#compute
display=[]
profit = 0
for _ in range(amount):
	result=[w[randint(len(w))] for w in wheel]
	display_str =f'|{"|".join(icon[r] for r in result)}|'
	# compute profit
	payout_label=''.join(numbers[r] for r in result)
	pay=0
	for k,v in payout.items():	# in order, get highest match
		if payout_label.startswith(k):
			pay=v
	if pay:
		display_str+=f' **+{pay}**'
	profit+=pay
	display.append(display_str)

# persist
bet+=profit
state[game_name]=bet
character().set_cvar(cvar,dump_json(state))
if profit:
	profit=f'**(+{profit})**'
else:
	profit=''


return f'embed -title "Slots" -desc "{nl.join(display)}" -f "Coins|{bet}{profit}" {footer} {thumb} '
</drac2>


