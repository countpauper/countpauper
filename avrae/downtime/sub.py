<drac2>
# TODO
#  Sub comamnd sub <gvar>: add to svar after checking it exist, same for downtime, craft and train except for one 'key'
#   add help how to make gvar to sub help for each
#   sub default to sub to default gvars through svar, dict: { table : { "key":["gvar",...] } }
#   sub reset to remove the svar section for the applicable section
#  data https://www.dndbeyond.com/sources/xgte/downtime-revisited#undefined
#   Sub commnd xge to enble, phb for phb downtime nd dmg for dmg downtime stuff set in svar
args=&ARGS&
key="activities"  # recipes # training
content={
	"activities":{
		"default":["b566982b-c7f8-4ea4-af47-4fd82623b335"],
		"test":["d052a3ca-a467-40f1-9a1c-c5582c0ca3eb"]
	},
	"recipes":{
		"default":"6498daf1-6d03-43ac-822a-2badfd533749",
		"test":"c890d965-e9e8-4b6c-91d1-15906457256b"
	},
	"training":{
		"default":"2605a178-bff2-4553-8548-6778ce0ba8e2"
	}
}
if not args:
	return f'echo `!{ctx.alias} sub <gvar>/<content> ... /reset` '

if args[0].lower()=='reset':
	gvars=None
else:	# Collect and check all gvars from the arguments
	gvars=[]
	for a in args:
		a=a.lower()
		# guid detection, to help give the right error
		parts = a.split('-')
		if len(parts)==5 and all([p.isalnum() for p in parts]):
			gvars.append(a)
		elif selection:=content[key].get(a):
			gvars+=selection
		else:
			return f'echo The {key} content `{a}` is not known.'

	for gv in gvars:
		test = get_gvar(gv)
		if test is None:
			return f'echo The gvar `{gv}` does not exist.'
		test_data = load_json(test)
		if typeof(test_data)!='SafeDict':
			return f'echo The gvar `{gv}` is not a valid JSON Object.'

# Apply
sv=load_json(get_svar('downtime','{}'))
if gvars:
	dest = sv.get(key,[])
	for gv in gvars:
		if not gv in dest:
			dest.append(gv)
	sv[key]=dest
elif key in sv:
	sv.pop(key)
return f'svar downtime {dump_json(sv)}'
</drac2>