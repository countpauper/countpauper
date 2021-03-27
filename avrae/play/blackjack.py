<drac2>
# TODO: double aces at start (or after split) should also reduce
cvar='Play'
game_name='blackjack'
state=load_json(get(cvar,'{}'))
game=state.get(game_name,{})
args="&*&"
target=21
cards=['A',2,3,4,5,6,7,8,9,10,'J','Q','K']
values={'a':1, 'A':11 , 'J':10, 'Q':10, 'K':10}
values.update({nr:nr for nr in range(2,11)})

syntax=f'{ctx.prefix}{ctx.alias} {game_name} [hit|split|stand]'

# start: get 2 cards
if not args:
	game = {'hand':[cards[randint(len(cards))],cards[randint(len(cards))]]}
	hand=game.hand
# hit: an extra card
else:
	hand=game.get('hand')
	if not hand:
		return f'echo No hand. Use `{ctx.prefix}{ctx.alias} {game_name}` to start.'
	if args=='hit':
		hand.append(cards[randint(len(cards))])
	elif args=='split':
		if game.get('split'):
			return f'echo You already split.'
		if len(hand)!=2:
			return f'echo You can not split {hand}. Only the first two cards can be split.'
		if hand==['a','A']:
			hand[0]='A'
		if values[hand[0]]!=values[hand[1]]:
			return f'echo You can not split {hand}. The cards need to have the same value.'
		game['split']=[hand[1] ,cards[randint(len(cards))]]
		game['hand']=[hand[0] ,cards[randint(len(cards))]]
		hand=game.hand
	elif args=='stand':
		game['done']=game.get('done',[])+[hand]
		if split:=game.get('split'):
			game['hand']=split
			hand=game.hand
			game.pop('split')
		else:
			game['hand']=None
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

# persist game state
state[game_name]=game
character().set_cvar(cvar,dump_json(state))

# output
fields=[]
if hand:=game.get('hand'):
	score=sum(values[card] for card in hand)
	fields.append(f'-f "{name}\'s Hand|`{" ".join(str(c) for c in hand)}` = `{score}`"')
if split:=game.get('split'):
	score=sum(values[card] for card in split)
	fields.append(f'-f "Split|`{" ".join(str(c) for c in split)}` = `{score}`"')
for done_hand in game.get('done',[]):
	score=sum(values[card] for card in done_hand)
	fields.append(f'-f "{"Bust at " if score>target else "Stand at "}{score}|`{" ".join(str(c) for c in done_hand)}`"')
return f'embed -title {game_name.title()} {" ".join(fields)} -footer "{syntax}"'
</drac2>