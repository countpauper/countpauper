import csv
import json
import random

def read_csv(file):
    reader = csv.DictReader(file)
    return [row for row in reader]

def group_materials(table):
    result = dict()
    for material in  "any", "cloth", "leather", "metal", "wood":
        result[material] = [row for row in table if row.get('Material').lower() == material]
    return result

translate_table = dict(
    blunt="blunt resistance",
    sharp="sharp resistance",
    poison="poison resistance",
    fire="fire resistance",
    cold="cold resistance",
    lightning="lightning resistance"
)

def translate_bonus_key(key):
    return translate_table.get(key.lower(), key.lower())

def bonify(table, columns, custom_columns=dict()):
    """Convert and filter rows of to bonus dicts"""
    return [{translate_bonus_key(col):val for col in columns if (val := row.get(col))} |
            {translate_bonus_key(row.get(col)):val for col, val_col in custom_columns.items() if (val := row.get(val_col))}
            for row in table]

def bonify_all(table, columns, custom_columns):
    """Convert the rows of all materials to bonus dicts"""
    return {material: bonify(rows, columns, custom_columns) for material, rows in table.items()}

armor = bonify(read_csv(open("armor.csv")), ("Name", "Rarity", "Price", "Hands", "Material", "Weight", "Sharp", "Blunt", "Fire", "Poison", "Lightning", "Cold"))
armor_material = bonify_all(group_materials(read_csv(open("armor material.csv"))), ("Prefix", "Rarity", "Price", "Weight","Sharp", "Blunt", "Fire", "Poison", "Lightning", "Cold"), {"B1":"V1"})
armor_bonus = bonify_all(group_materials(read_csv(open("armor bonus.csv"))),("Prefix", "Postfix", "Rarity", "Price", "Weight"), {"B1": "V1", "B2": "V2", "B3":"V3"})



def select_random(table):
    rarities = [int(row.get("rarity", 0)) for row in table]
    defaultRarity = 100 - sum(rarities)
    return random.choices([None] + table, [defaultRarity] + rarities)[0]

def select(table, **filter):
    return [row for row in table if all(row.get(k).lower() == v.lower() for k,v in filter.items())]


def combine(*stats):
    result = dict()
    name = ""
    rarity = 1.0
    for stat_block in stats:
        if stat_block is None:
            continue
        for k, v in stat_block.items():
            if k == "name":
                name = v
            elif k == "prefix":
                if v[0] != "*":
                    name = v + " " + name
            elif k == "postfix":
                if v[0] != "*":
                    name+= " " + v
            elif k == "material":
                pass # TODO: could just recursively add to stats here, if k specified which category
            elif k == "rarity":
                rarity *= float(v)/100
            else:
                result[k] = result.get(k, 0) + int(v)
    return dict(name=name, rarity=rarity) | result

def generate_random_item(items, materials, boni):
    item = select_random(items)
    applicable_materials = materials.get('any',[]) + materials.get(item.get("material").lower(), [])
    material = select_random(applicable_materials)
    applicable_boni = boni.get('any',[]) + boni.get(item.get("material").lower(), [])
    bonus = select_random(applicable_boni)
    return combine(item, material, bonus)

def generate_all_items(items, materials, boni):
    result = []
    for item in items:
        for material in materials.get('any',[]) + materials.get(item.get("material").lower(), []):
            for bonus in [None] +boni.get('any',[]) + boni.get(item.get("material").lower(), []):
                result.append(combine(item, material, bonus))
    return result

nl='\n'
def format_rows(rows):
    return nl.join(str(row) for row in rows)

def format_materials(mats):
    return nl.join(f"{mat}\n{format_rows(rows)}" for mat, rows in mats.items())

def format_item(item):
    return ", ".join(f"{k}: {v}" for k,v in item.items())

if __name__ == "__main__":
    all_items = generate_all_items(armor, armor_material, armor_bonus)
    for item in all_items:
        print(format_item(item))
    print(f"Item of the day (out of {len(all_items)}): " + format_item(generate_random_item(armor, armor_material, armor_bonus)))

    export = dict(armor=armor, armor_material=armor_material, armor_bonus = armor_bonus)

    with open('items.json', 'w') as f:
            json.dump(export, f)
