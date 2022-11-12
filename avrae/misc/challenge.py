<drac2>
args=&ARGS&
var_name='challenge_players'
players=[int(p) for p in load_yaml(get_svar(var_name,get(var_name,'[]')))]
CR=[float(cr) for cr in args if cr.isdigit()]
CR+=[roll("1000*"+cr)/1000 for cr in args if '/' in cr]
if C:=combat():
	monsters=[c for c in C.combatants if c.controller==ctx.author.id]
	CR += [cr for c in monsters if bool(tuple(c.levels)) and (cr:=(c.levels.get('Monster',default=-1)))>=0]
	players=[p for c in C.combatants if c.controller!=ctx.author.id and c.levels.get('Monster')==0]
	players+=[p.levesl.total_level for p in players]
XPtable={0:10, 0.125:25, 0.25:50, 0.5:100, 1:200, 2:450, 3:700, 4:1100, 5:1800, 6:2300, 7:2900, 8:3900, 9:5000, 10:5900, 11:7200, 12:8400, 13:10000, 14:11500, 15:13000, 16:15000 }
xp=sum(XPtable.get(cr,0) for cr in CR)
adjusted_table={0: 1, 1:1,
			 2: 1.5,
			 3: 2, 4: 2, 5: 2, 6: 2,
			 7: 2.5, 8: 2.5, 9: 2.5, 10: 2.5,
			 11: 3, 12: 3, 13: 3, 14: 3, 15: 3, 16: 3, 17: 3, 18: 3, 19: 3, 20: 3}
mul=adjusted_table.get(len(CR),1)
threshold_table = [
	(10, 20, 30, 50), # HB level 0 dummy
	(25, 50, 75, 100),
	(50, 100, 150, 200),
	(75, 150, 225, 400),
	(125, 250, 375, 500),
	(250, 500, 750, 1100),
	(300, 600, 900, 1400),
	(350, 750, 1100, 1700),
	(450, 900, 1400, 2100),
	(550, 1100, 1600, 2400),
	(600, 1200, 1900, 2800),
	(800, 1600, 2400, 3600),
	(1000, 2000, 3000, 4500),
	(1100, 2200, 3400, 5100),
	(1250, 2500, 3800, 5700),
	(1400, 2800, 4300, 6400),
	(1600, 3200, 4800, 7200),
	(2000, 3900, 5900, 8800),
	(2100, 4200, 6300, 9500),
	(2400, 4900, 7300, 10900),
	(2800, 5700, 8500, 12700)]
# HB extend threshold range
threshold_labels=["easy","medium","hard","deadly","deadlyer","extra deadly","seriously deadly","double deadly", "really deadly", "totally deadly"]
threshold_table=[(e, m, h, d, d+e, d+m, d+h, 2*d, 2*d+e, 2*d+m) for (e, m, h, d) in threshold_table]
thresholds={lbl:sum(threshold_table[lvl][i] for lvl in players) for i,lbl in enumerate(threshold_labels)}
total_xp = round(mul*xp)
challenge=(["Impossible"]+[lbl for lbl,t in thresholds.items() if total_xp>t])[-1]
return f'echo {CR} VS {players}\nthresholds={thresholds}\nxp = {xp} bonus = {mul*100}% total = {total_xp} **{challenge}**\n'
</drac2>