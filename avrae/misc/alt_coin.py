!servalias coins <drac2>
coins=load_json(get_svar('coins','{}'))
coinRates=coins.get('rates',{"cp":100,"sp":10,"ep":2,"gp":1,"pp":0.1})
args='''&*&'''
if sum(args.count(c) for c in coinRates)>1:
	return f'echo Use: `!qb {args}`'

coinWeight=coins.get('weight',0.02)
coinPouchName=coins.get('pouch',"Coin Pouch")
coinTypes=coins.get('types',list(coinRates.keys()))
defaultCoin=coins.get('default',[k for (k,v) in coinRates.items() if v==1][0])
return f'''tembed
{'{'*2}args=&ARGS&{'}'*2}
{'{'*2}coinTypes={coinTypes}{'}'*2}
{'{'*2}coinRates={dump_json(coinRates)} {'}'*2}
{'{'*2}defaultCoin="{defaultCoin}"{'}'*2}
{'{'*2}coinPouchName="{coinPouchName}"{'}'*2}
{get_gvar("912e8eac-adcb-4d29-bcb6-23acc3624211")}'''
</drac2>