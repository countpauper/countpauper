!alias chaos <drac2>
r = roll('d20'), roll('d20')
d = [10 - r for r in r]
if abs(d[0]) > abs(d[1]):
    win = 0
elif abs(d[1]) > abs(d[0]):
    win = 1
elif d[0] < d[1]:
    win = 1
else:
    win = 0
n='chaos_table'
t=load_json(get_svar(n,get_uvar(n, "[]")))
e=(t[r[win]-1:]+["?"])[0]
return f'echo {r[win]}({r[not win]}) : {e}'
</drac2>
