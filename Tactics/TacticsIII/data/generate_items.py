import csv
import json
import random

def read_csv(file):
    reader = csv.DictReader(file)
    return [row for row in reader]


translate_table = dict(
)

match_columns="material","type","damage type"

def translate_bonus_key(key):
    return translate_table.get(key.lower(), key.lower())

def bonify(table, columns, custom_columns=dict()):
    """Convert and filter rows of to bonus dicts"""
    return [{translate_bonus_key(col):val for col in columns if (val := row.get(col))} |
            {translate_bonus_key(row.get(col)):val for col, val_col in custom_columns.items() if (val := row.get(val_col))}
            for row in table]

def column_correct(column, value, item):
    if column=="rarity":
        return float(value) * 0.01
    if value.isdigit():
        return int(value)
    elif value[0]=='-' and value[1:].isdigit():
        return int(value)
    elif not item and column in match_columns:
        if value.lower()=="any":
            return []
        else:
            return [v.lower().strip() for v in value.split("|")]
    else:
        return value.lower().strip()

def type_correct(table, item=False):
    result=list()
    for row in table:
        result.append({k: column_correct(k,v, item) for k,v in row.items()})
    return result


armors = type_correct(bonify(read_csv(open("armor.csv")), ("Name", "Rarity", "Price", "Hands", "Material", "Weight", "Sharp", "Blunt", "Fire", "Poison", "Lightning", "Cold")), True)
armor_material = type_correct(bonify(read_csv(open("armor material.csv")), ("Prefix", "Rarity", "Price", "Material", "Weight","Sharp", "Blunt", "Fire", "Poison", "Lightning", "Cold"), {"B1":"V1"}))
armor_bonus = type_correct(bonify(read_csv(open("armor bonus.csv")),("Prefix", "Postfix", "Rarity", "Price", "Material", "Weight"), {"B1": "V1", "B2": "V2", "B3":"V3"}))

weapons = type_correct(bonify(read_csv(open("weapons.csv")), ("Name", "Rarity", "Price", "Hands", "Reach", "Range", "Load", "Material", "Type", "Weight", "Damage Type", "Stat", "Damage")), True)
weapon_material = type_correct(bonify(read_csv(open("weapon material.csv")), ("Prefix", "Rarity", "Price", "Material", "Weight","Sharp Damage", "Blunt Damage", "Fire Damage", "Poison Damage", "Lightning Damage", "Cold"), {"B1":"V1"}))
weapon_bonus = type_correct(bonify(read_csv(open("weapon bonus.csv")),("Prefix", "Postfix", "Rarity", "Price", "Attument", "Material", "Damage Type", "Type",   "Weight"), {"B1": "V1", "B2": "V2", "Penalty":"V3"}))

def select_random(table):
    rarities = [row.get("rarity", 0) for row in table]
    defaultRarity = 1.0 - sum(rarities)
    return random.choices([None] + table, [defaultRarity] + rarities)[0]

def select(table, **filter):
    return [row for row in table if all(row.get(k).lower() == v.lower() for k,v in filter.items())]

def match(item, bonus, columns):
    return all(not bonus.get(col, []) or
               item.get(col,"") in bonus.get(col,[]) for col in columns)

def specify_damage(item):
    if (damage_type:=item.get("damage type")) and (damage:=item.get("damage")):
        dt_name=f'{damage_type} damage'
        item[dt_name] = item.get(dt_name, 0) + damage
        del item["damage"]
    return item

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
            elif k in match_columns:
                result[k] = result.get(k, v)
            elif k == "rarity":
                rarity *= float(v)
            elif k == 'stat':
                result[k] = v
            else:
                result[k] = result.get(k, 0) + v
    result = specify_damage(result)
    return dict(name=name, rarity=rarity) | result

def generate_random_item(items, materials, boni):
    item = select_random(items)
    applicable_materials = [material for material in materials if match(item, material, match_columns)]
    material = select_random(applicable_materials)
    applicable_boni = [bonus for bonus in boni if match(item, bonus, match_columns)]
    bonus = select_random(applicable_boni)
    return combine(item, material, bonus)


def generate_all_items(items, materials, boni):
    result = []
    for item in items:
        for material in materials:
            if match(item, material, match_columns):
                result.append(combine(item, material))
                for bonus in boni:
                    if match(item, bonus, match_columns):
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
    all_armors = generate_all_items(armors, armor_material, armor_bonus)
    for item in all_armors:
        print(format_item(item))
    all_weapons = generate_all_items(weapons, weapon_material, weapon_bonus)
    for item in all_weapons:
        print(format_item(item))

    print(f"Armor of the day (out of {len(all_armors)}): " + format_item(generate_random_item(armors, armor_material, armor_bonus)))
    print(f"Weapon of the day (out of {len(all_weapons)}): " + format_item(generate_random_item(weapons, weapon_material, weapon_bonus)))

    export = dict(armor=armors, armor_material=armor_material, armor_bonus = armor_bonus,
                  weapon=weapons, weapon_material=weapon_material, weapon_bonus = weapon_bonus)

    with open('items.json', 'w') as f:
            json.dump(export, f)
