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
    return name.lower().strip(), description.strip()

def convert_generic(input, category ):
    data = dict()
    for row in input:
        name, desc = parse_feature(row['Feature'])
        data[name] = { k.lower():v for k,v in row.items() if k!='Feature' and v}
        data[name].update(dict(description=escape(desc),category=category))
    return data

def convert_subclass(input):
    data = dict()
    for row in input:
        cls, sub = row.get('subclass').split('-')
        cls = cls.strip().lower()
        sub = sub.strip().lower()

        name, desc = parse_feature(row['Foundation'])
        if name is not None:
            data[name] = {'description':escape(desc), 'category':'subclass', 'mastery':1, 'class':cls, 'subclass':sub, 'level':1}

        name, desc = parse_feature(row['Specialization'])
        if name is not None:
            data[name] = {'description':escape(desc), 'category':'subclass', 'mastery':2, 'class':cls, 'subclass':sub, 'level':5}

        name, desc = parse_feature(row['Mastery'])
        if name is not None:
            data[name] = {'description':escape(desc), 'category':'subclass', 'mastery':3, 'class':cls, 'subclass':sub, 'level':8}
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
    for row in input:
        row_str='\n'.join(row)
        name, domain, cost, desc = row_str.split('\n',maxsplit=3)
        card = dict(description=escape(desc),category='domain')
        card.update(parse_cost(cost))
        card.update(parse_domain(domain))
        data[name.lower()] = card
    return data

if __name__ == "__main__":
    data = dict()
    with open('daggerheart/domain cards.csv', newline='') as csvfile:
        reader = csv.reader(csvfile)
        data.update(convert_cards(reader))

    with open('daggerheart/7a5a3d45-7ad0-4db9-9c9a-dc37294a216e.gvar',mode='w',newline='') as jsonFile:
        jsonFile.write(json.dumps(data, indent='\t'))

    data = dict()    # split in two gvars because it's too muc
    with open('daggerheart/class features.csv', newline='') as csvfile:
        reader = csv.DictReader(csvfile)
        data.update(convert_generic(reader, 'class'))

    with open('daggerheart/subclass features.csv', newline='') as csvfile:
        reader = csv.DictReader(csvfile)
        data.update(convert_subclass(reader))

    with open('daggerheart/ancestry features.csv', newline='') as csvfile:
        reader = csv.DictReader(csvfile)
        data.update(convert_generic(reader, 'ancestry'))

    with open('daggerheart/community features.csv', newline='') as csvfile:
        reader = csv.DictReader(csvfile)
        data.update(convert_generic(reader, 'community'))

    with open('daggerheart/1eeba3a1-1fd9-4b72-8713-d6475ccfc204.gvar',mode='w',newline='') as jsonFile:
        jsonFile.write(json.dumps(data, indent='\t'))

