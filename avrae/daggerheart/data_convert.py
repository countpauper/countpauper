import csv
import json

def escape(s):
    if not s:
        return s
    return ''.join('\\"' if c == '"' else c for c in s)

def parse_feature(feature):
    if not feature:
        return None, None
    name, description = feature.split(':', maxsplit=1)
    name = name.replace("’","'").lower().strip()
    return name, description.strip()

integer_keys = 'hope', 'stress', 'armor', 'recall', 'level', 'mastery'
def generic_convert(key, value):
    if value is None:
        return None
    if key == "Feature":
        return None # This is split 
    elif key.lower() in integer_keys:
        if not value:
            return None 
        else:
            return int(value)
    else:
        return str(value).strip()

def convert_generic(input, category ):
    data = dict()
    for row in input:
        name, desc = parse_feature(row['Feature'])
        data[name] = dict(description=escape(desc), category=category, type='feature', source='SRD')
        data[name].update({ k.lower():cv for k, v in row.items() if (cv:=generic_convert(k, v)) is not None})
    return data

def convert_subclass(input):
    data = dict()
    for row in input:
        cls, sub = row.get('subclass').split('-')
        cls = cls.strip().lower()
        sub = sub.strip().lower()

        name, desc = parse_feature(row['Foundation'])
        if name is not None:
            data[name] = {'description':escape(desc), 'category':'subclass', 'mastery':1, 'class':cls, 'subclass':sub, 'type':'feature', 'source':'SRD'}

        name, desc = parse_feature(row['Specialization'])
        if name is not None:
            data[name] = {'description':escape(desc), 'category':'subclass', 'mastery':2, 'class':cls, 'subclass':sub, 'type':'feature', 'source':'SRD'}

        name, desc = parse_feature(row['Mastery'])
        if name is not None:
            data[name] = {'description':escape(desc), 'category':'subclass', 'mastery':3, 'class':cls, 'subclass':sub, 'type':'feature', 'source':'SRD'}
    return data

def parse_cost(cost):
    result=dict()
    recall_header = "recall cost:"
    if cost.lower().startswith(recall_header):
        result['recall'] = int(cost[len(recall_header):])
    else:
        raise NotImplementedError(f"Unsupported card cost format: '{cost}'")
    return result 

def parse_domain(domain):
    parts = domain.lower().split(' ')
    if parts[0]!='level':
        raise NotImplementedError(f"Unsupported card domain format: '{domain}'")
    return dict(
        level=int(parts[1]),
        domain=parts[2],
        type=parts[3]
    )

def convert_cards(input):
    data = dict()
    for line, row in enumerate(input):
        row_str='\n'.join(row)
        name, domain, cost, desc = row_str.split('\n',maxsplit=3)
        card = dict(description=escape(desc),category='domain', source='SRD')
        card.update(parse_cost(cost))
        card.update(parse_domain(domain))
        data[name.lower()] = card
        # print(f"Complete {line} {name}")
    return data

max_sub_classes=2
def convert_classes(input):
    result = dict()
    for row in input:
        name=row.get('class').lower().strip()
        result[name]=dict(domains=[row.get('primary domain').lower().strip(), row.get('secondary domain').lower().strip()], evasion = int(row.get('evasion',0)), hp=int(row.get('hp',0)))
        subs=dict()
        for n in range(0,max_sub_classes):
            if subname:=row.get(f'sub{n}'):
                subname = subname.strip()
                subdata=dict()
                if trait:=row.get(f'trait{n}'):
                    subdata['cast']=trait.lower().strip()
                subs[subname.lower()]=subdata
        result[name]['subclasses'] = subs
    return result

if __name__ == "__main__":
    data = dict()
    with open('daggerheart/domain cards.csv', newline='', encoding='utf-8-sig', errors='replace') as csvfile:
        reader = csv.reader(csvfile)
        data.update(convert_cards(reader))

    with open('daggerheart/7a5a3d45-7ad0-4db9-9c9a-dc37294a216e.gvar',mode='w',newline='') as jsonFile:
        jsonFile.write(json.dumps(data, indent='\t'))

    data = dict()    # split in two gvars because it's too muc
    with open('daggerheart/class features.csv', newline='', encoding='utf-8-sig', errors='replace') as csvfile:
        reader = csv.DictReader(csvfile)
        data.update(convert_generic(reader, 'class'))

    with open('daggerheart/subclass features.csv', newline='', encoding='utf-8-sig', errors='replace') as csvfile:
        reader = csv.DictReader(csvfile)
        data.update(convert_subclass(reader))

    with open('daggerheart/ancestry features.csv', newline='', encoding='utf-8-sig', errors='replace') as csvfile:
        reader = csv.DictReader(csvfile)
        data.update(convert_generic(reader, 'ancestry'))

    with open('daggerheart/community features.csv', newline='', encoding='utf-8-sig', errors='replace') as csvfile:
        reader = csv.DictReader(csvfile)
        data.update(convert_generic(reader, 'community'))

    with open('daggerheart/transformation features.csv', newline='', encoding='utf-8-sig', errors='replace') as csvfile:
        reader = csv.DictReader(csvfile)
        data.update(convert_generic(reader, 'transformation'))

    with open('daggerheart/1eeba3a1-1fd9-4b72-8713-d6475ccfc204.gvar',mode='w',newline='', encoding='utf-8-sig', errors='replace') as jsonFile:
        jsonFile.write(json.dumps(data, indent='\t'))

    with open('daggerheart/class info.csv', newline='') as csvfile:
        reader = csv.DictReader(csvfile)
        class_info = convert_classes(reader)

    with open('daggerheart/0342d22f-af8d-44d0-811a-5f1ec8049248.gvar',mode='w',newline='') as jsonFile:
        jsonFile.write(json.dumps(class_info, indent='\t'))

