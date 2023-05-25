# real python
import sys
import random
from chance import math
import json

def rand_key(dictionary):
    return list(dictionary.keys())[random.randint(0,len(dictionary)-1)]

add_space = lambda s: s + " " if s else ""

def money_description(money):
    gp = int(money)
    sp = int((money-gp)*10)
    cp = int((money-(gp+0.1*sp))*100)
    ret = None
    if gp:
        ret = add_space(ret) + str(gp)+"gp"
    if sp:
        ret = add_space(ret)  + str(sp)+"sp"
    if cp:
        ret = add_space(ret)  + str(cp)+"cp"
    return ret

def loot_description(gems):
    ret = None
    for gem, (amount,price) in gems.items():
        ret = add_space(ret) + str(amount)+"x "+gem+" ("+money_description((price))+")"
    return ret

def random_gems(total):
    cost = 0
    ret = {}
    with open("gems.json","r") as file:
        data = json.load(file)
    gems = data["gems"]
    qualities = data["qualities"]
    sizes = data["sizes"]
    while cost<min(total):
        left = max(total)-cost
        gem = rand_key(gems)
        size = rand_key(sizes)
        if random.randint(0,4)==0:
            quality = rand_key(qualities)
        else:
            quality = "{}"

        value = gems[gem] * qualities[quality] * sizes[size]
        max_amount = math.floor(left / value)
        if max_amount > 0 and max_amount < 20:
            amount = random.randint(1,max_amount)
            cost = cost + amount * value
            description = size.format(quality.format(gem))
            if description in ret:
                ret[description] = ret[description][0] + amount, value
            else:
                ret[description] = amount, value
    return ret

def easy_gems(gp):
    ret = {}
    with open("gems.json","r") as file:
        data = json.load(file)
    gems, qualities, sizes = data["gems"], data["qualities"], data["sizes"]
    tries = 0
    limit = gp//10
    while gp>limit and tries<4:
        tries +=1
        gem = rand_key(gems)
        size = rand_key(sizes)
        if random.randint(0,4)==0:
            quality = rand_key(qualities)
        else:
            quality = "{}"

        value = gems[gem] * qualities[quality] * sizes[size]
        max_amount = math.floor(gp / value)
        if max_amount>0 and max_amount<20:
            amount = random.randint(1,max_amount)
            gp -= amount * value
            description = size.format(quality.format(gem))
            if description in ret:
                ret[description] = ret[description][0] + amount, value
            else:
                ret[description] = amount, value
        tries = 0
    return ret

if __name__ == "__main__":
    # execute only if run as a script
    if len(sys.argv)>2:
        gold = (float(sys.argv[1]), float(sys.argv[2])+1)
    elif len(sys.argv)==2:
        max_gold = float(sys.argv[1])
        gold = (max_gold / 2,max_gold+1)
    else:
        max_gold = random.randint(1, int(math.pow(5, random.randint(1, 8))))
        gold = (max_gold / 2, max_gold+1)
    print(money_description(min(gold))+" to "+money_description(max(gold)-1))
    # gems = random_gems(gold)
    gems = easy_gems(max(gold))
    print(loot_description(gems))

