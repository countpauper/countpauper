!alias join <drac2>
ccn='Gift of Alacrity'
ch=character()
c=combat()
if not ch.cc_exists(ccn):
	return 'init join'

if ch.get_cc(ccn):
	return f'init join'

if c and c.me:
	bonus=roll('1d8')
	return f'init opt "{name}" -p +{bonus}'
else:
	return f'init join -b 1d8 -phrase {ccn}'
</drac2>