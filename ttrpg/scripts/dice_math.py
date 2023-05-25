from ability import dice as ability_dice

def pmf(die):
    return [1/die]*die

def pmf_max(dice):
    max_pmf = pmf(dice[0])
    for die in dice[1:]:
        next_die_pmf = pmf(die)+[0]*(len(max_pmf)-die)
        max_pmf=[p*sum(next_die_pmf[:v+1]) + next_die_pmf[v]*sum(max_pmf[:v]) for v, p in enumerate(max_pmf)]
    return max_pmf

def pmf_sum(dice):
    sum_pmf = [1]
    for die in dice:
        die_pmf = [0]+pmf(die)
        next_pmf=[0]*(len(sum_pmf)+len(die_pmf))
        for i0,p0 in enumerate(sum_pmf):
            for i1,p1 in enumerate(die_pmf):
                next_pmf[i0+i1]+=p0*p1
        sum_pmf=next_pmf
    return sum_pmf[1:-len(dice)]

def mean_pmf(pmf):
    return sum(p*idx for idx,p in enumerate(pmf))

def var_pmf(pmf):
    mp = mean_pmf(pmf)
    return sum(p * (idx**2) for idx, p in enumerate(pmf)) - (mp**2)

if __name__ == "__main__":
    for ability ,ad in enumerate(ability_dice):
        _pmf =  pmf_sum(ad)
        weighted = [p*(idx+1) for idx,p in enumerate(_pmf)]
        pmf_str=[str(p) for p in _pmf]+[" "]*(36-len(_pmf))
        ad_str=[str(d) for d in ad]+[" "]*(3-len(ad))
        dpr=[sum(p*max(0, atk-defense) for atk, p in enumerate(_pmf)) for defense in range(20) ]
        dpr_str=[str(d) for d in dpr]
        print(f"{ability+1}, {','.join(ad_str)}, {','.join(pmf_str)}, {mean_pmf([0]+_pmf)}, {var_pmf([0]+_pmf)}, dpr, {','.join(dpr_str)}")
