!alias xpzip echo <drac2>
xp_logs = load_json(get('xplog', '{}'))
#xp_logs = load_json(get('xplog', get('xplog_backup','{}')))

# parse the arguments
args=argparse(&ARGS&)
arbitrary_period = {"day": 0, "month": 3, "year": 6}
period = ([period for period in arbitrary_period.keys() if args.last(period)]+["day"])[0]
limit=args.last('h',16, type_=int)

# store the last <limit> items as tail and remove them from the log to be merged
xp_tail={k:v for k,v in xp_logs.items() if k in list(xp_logs.keys())[-limit:]}
xp_logs={k:v for k,v in xp_logs.items() if k not in xp_tail.keys()}

# list to prevent key errors
date_time_list = [(i.split(" ")[0], j) for i, j in xp_logs.items()]
# removing the comments to be able to add and make separate lists off dates and amounts to prevent key errors
xp_dates = []
xp_amounts = []
for d,s in date_time_list:
    idx=min(i for i,c in enumerate(s+"|") if c not in '+- 0123456789')    # split integer part
    temp = int(s[:idx].replace(' ',''))
    xp_dates.append(d[arbitrary_period[period]:])
    xp_amounts.append(temp)

# taking out conflicting keys.
xp_logs_merged = {}
for i in range(len(xp_dates)):
    if xp_dates[i] not in xp_logs_merged.keys():
        xp_logs_merged[xp_dates[i]] = xp_amounts[i]
        continue
    xp_logs_merged[xp_dates[i]] += xp_amounts[i]

# append the tail again
xp_logs_merged.update(xp_tail)

# persist
# character().set_cvar('xplog',dump_json(xp_logs_merged))
return "\n".join(f"{d} - {v}" for d,v in xp_logs_merged.items())
</drac2>