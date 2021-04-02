<drac2>
cvar='Play'
game_name='blackjack'
state=load_json(get(cvar,'{}'))
game=state.get(game_name,{})
bet=state.get('bet',{})
args="&*&".lower()
target=21
cards=['A',2,3,4,5,6,7,8,9,10,'J','Q','K']
values={'a':1, 'A':11 , 'J':10, 'Q':10, 'K':10}
symbol={'A':':a:','a':':a:',2:':two:',3:':three:',4:':four:',5:':five:',6:':six:',7:':seven:', 8:':eight:',9:':nine:',10:':keycap_ten:',
		'J':':regional_indicator_j:','Q':':regional_indicator_q:','K':':regional_indicator_k:'}


values.update({nr:nr for nr in range(2,11)})

base_cmd=f'{ctx.prefix}{ctx.alias}'
game_cmd=f'{base_cmd} {game_name}'
syntax=f'{game_cmd} [hit|split|stand]'

base_instructions=[f'Ask for another card with `{game_cmd} hit`',
					f'Or stand with `{game_cmd} stand`']
# start: get 2 cards
if not args:
	game = {'hand':[cards[randint(len(cards))],cards[randint(len(cards))]]}
	hand=game.hand
	title=f'{name} plays {game_name}.'
	instructions=[f'Place your bet with `{base_cmd} bet [<amount> [<coin>]]`'] + base_instructions
	if values[hand[0]] == values[hand[1]]:
		instructions.append(f'You have equal value cards. You can split with `{game_cmd} split`')
else:
	hand=game.get('hand')
	if not hand:
		return f'echo No hand. Use `{ctx.prefix}{ctx.alias} {game_name}` to start.'
	# hit: an extra card
	if args=='hit' or args=='draw' or args=='card':
		title=f'{name} asks for a card.'
		hand.append(cards[randint(len(cards))])
		instructions = base_instructions
	# split hand
	elif args=='split':
		if game.get('split'):
			return f'echo You already split.'
		if len(hand)!=2:
			return f'echo You can not split {hand}. Only the first two cards can be split.'
		if hand==['a','A']:
			hand[0]='A'
		if values[hand[0]]!=values[hand[1]]:
			return f'echo You can not split {hand}. The cards need to have the same value.'
		title=f'{name} splits {get("their","their")} hand.'
		game['split']=[hand[1] ,cards[randint(len(cards))]]
		game['hand']=[hand[0] ,cards[randint(len(cards))]]
		hand=game.hand
		instructions=base_instructions
	# stay for final score
	elif args=='stand' or args=='stay':
		score = sum(values[card] for card in hand)
		game['done']=game.get('done',[])+[hand]
		if split:=game.get('split'):
			title = f'{name} stands at {score} and continues with their split hand.'
			game['hand']=split
			hand=game.hand
			game.pop('split')
			instructions=base_instructions
		else:
			title = f'{name} stands at {score}.'
			game['hand']=None
			instructions=[f'If you win, double your bet with `{base_cmd} collect {" ".join(f"{q} {c}" for c,q in bet.items())}`',
						  f'If you lose, forfeit your bet with `{base_cmd} out`'] if bet else []
			instructions.append(f'Play again with `{game_cmd}`')
	else:
		return f'echo `{syntax}`'

if hand:
	while 'A' in hand and sum(values[card] for card in hand)>target:	# reduce an ace 11->1
		hand[hand.index('A')]='a'

	if sum(values[card] for card in hand)>target:	# bust
		game['done'] = game.get('done', []) + [hand]
		if split := game.get('split'):
			game['hand'] = split
			game.pop('split')
		else:
			game['hand'] = None
		instructions=[f'You lose your bet, foreit with `{base_cmd} out`'] if bet else []
		instructions.append(f'Play again with `{game_cmd}`')

# persist game state
state[game_name]=game
character().set_cvar(cvar,dump_json(state))

# output
fields=[]
if hand:=game.get('hand'):
	score=sum(values[card] for card in hand)
	fields.append(f'-f "{name}\'s Hand|{" ".join(symbol[c] for c in hand)} = `{score}`"')
if split:=game.get('split'):
	score=sum(values[card] for card in split)
	fields.append(f'-f "Split|{" ".join(symbol[c] for c in split)} = `{score}`"')
for done_hand in game.get('done',[]):
	score=sum(values[card] for card in done_hand)
	fields.append(f'-f "{name+" busted at " if score>target else name+" final hand at "}{score}|{" ".join(symbol[c] for c in done_hand)}"')
thumb='-thumb !embed -thumb https://cdn.discordapp.com/attachments/751105864370356365/827258201502253116/blackjack.png'
nl='\n'
return f'embed -title "{title}" {" ".join(fields)} -f "Instructions|{nl.join(instructions)}"  {thumb}'
</drac2>