import json
import sys
from d20 import roll


def parse_range(s):
    values = s.split('-')
    if len(values) == 1:
        return range(int(values[0]), int(values[0]) + 1)
    else:
        return range(int(values[0]), int(values[1]) + 1)

def parse_table(table):
    parsed=dict()
    for k, v in table.items():
        for d in parse_range(k):
            parsed[d] = v
    return f'1d{max(parsed.keys())}', parsed


def resolve(item):
    if item is None:
        return ""
    elif type(item) == dict:
        die, results = parse_table(item)
        r = roll(die)
        result = results.get(r.total)
        return resolve(result)
    elif type(item) == list:
        return "|".join(s for i in item if (s:=resolve(i)))
    else:
        return str(item)

def generate(size, tables):
    height = size[1]
    width = size[0]
    rows=[]
    for y in range(height):
        rows.append([resolve(tables) for _ in range(width - ((y+1) % 2))])
    return rows

if __name__ == "__main__":
    with open("hex.json","r") as file:
        tables = json.load(file)
    size = [int(d) for d in sys.argv[1].split("x")]
    print("\n".join(", ".join(str(col) for col in row) for row in generate(size, tables)))

