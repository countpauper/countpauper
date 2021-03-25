!alias renc <drac2>
args=argparse("&*&")
if args.get('help'):
	return f'echo `{ctx.prefix}{ctx.alias} [-t <treshold>][...] [-rr <repeat>]`'

tresholds=[int(t) for t in args.get('t')] or [10]
tresholds *= int(args.last('rr',1))
rolls=[(i,vroll('1d20'),t) for i,t in enumerate(tresholds)]
scaries=[':scream:',':spider:',':grimacing:',':pleading_face:',':face_with_symbols_over_mouth:',':dragon_face:',':skull_crossbones:',':confounded:',':skull:',';:ghost:',':zombie:',':scream_cat:']
encounters=[f'**{i+1}** ||{r}|| {scaries[randint(len(scaries))]} ||{vroll("1d12+1d8") if r.total>rt else "`none"+" "*17+"`" }||' for i,r,rt in rolls]
sep='\n'
return f'embed -title "Random Encounter" -desc "{sep.join(encounters)}"'
</drac2>