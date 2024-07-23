import json

if __name__ == "__main__":
    # spells
    with open('.\\spells.json', newline='') as spellfile:
        spells = json.load(spellfile)

    tables=[]
    for level in range(1,6):
        level_spells = [k for k,v in spells.items() if v.get('level', 0) == level]
        factor = max(1.0, 100.0 / len(level_spells))
        l=[""]*100
        for r, s in enumerate(level_spells):
            l[int(r*factor)] = s.capitalize()
        tables.append(l)
    for r in range(0,100):
        print(f'{r+1}, '+', '.join(f'{l[r]}' for l in tables))
