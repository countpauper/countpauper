import argparse 
import ijson 
import json

def match(record, **kwargs):
    # print(f"Match {record} {kwargs}")
    return all(record[k] == v for k,v in kwargs.items())

def coord_tuple(coords):
    return tuple(coords.get(axis) for axis in "xyz")

def inside(record, bounding_box):
    coords = coord_tuple(record.get("coords"))
    return all(
        v>=limit[0] and v<=limit[1] 
        for v,limit in zip(coords, bounding_box)
    
    )
def parse_map(parser, columns=None, prefix=''):
    result = dict()
    key = None
    for field, data_type, value in parser:
        if data_type=='end_map':
            return result 
        elif data_type=='map_key':
            key = value 
        elif data_type=='start_map':
            result[key] = parse_map(parser, None, f'{prefix}.{key}') # recursive TODO sub columns?
            key = None
        else: 
            assert(key is not None)
            # assert field == f'{prefix}.{key}'
            if columns is None or key in columns:
                result[key] = value 
            key = None 

def query_systems_old(filename, query, action, columns=None):
    array_prefix='item'
    with open(filename, 'r') as f:
        parser = ijson.parse(f, use_float=True)
        prefix=""
        for field, data_type, value in parser:
            if data_type=='start_array':
                prefix += array_prefix
                rows = 0
                matches = 0
            elif data_type == 'end_array':
                assert prefix.endswith(array_prefix)
                prefix = prefix[:-len(array_prefix)]
            elif data_type == 'start_map':
                record = parse_map(parser, columns, prefix)
                # print(f"[{row}] = {record['name']}")
                rows += 1 
                if query(record):
                    matches += 1
                    result = action(record)
                    if result:
                        return result
                #if rows % 100000 == 0:
                #    print(f"{matches}/{rows}")
            else:
                assert(False)    # unexpected type 
    return None  


def query_systems(filename, query, action, columns=None):
    array_prefix='item'
    rows, matches = 0,0
    with open(filename, 'r') as f:
        for record in ijson.items(f, array_prefix):
            rows += 1 
            if query(record):
                matches += 1
                result = action(record)
                if result:
                    return result
            #if rows % 100000 == 0:
            #    print(f"{matches}/{rows}")
    return None  

def early_out(record, _):
    return record

def find_system(filename, query, columns=None):
    return query_systems(filename, query, lambda x: early_out(x), columns)

def collect(record, result):
    result.append(record)
    return None # len(result)>=1000
    
def flatten(record):
    coord = coord_tuple(record.pop('coords'))
    record['x'] = coord[0]
    record['y'] = coord[1]
    record['z'] = coord[2]
    return record
    
def print_csv(record):
    print(",".join(str(record.get(k)) for k in ["id64","x","y","z","name"]))
    
def find_systems(filename, query, columns=None):
    result = []
    query_systems(filename, query, lambda x: collect(x, result), columns)
    return result

def print_systems(filename, query, columns=None):
    query_systems(filename, query, lambda x: print_csv(flatten(x)), columns)

def is_coordinate_string(s):
    return s.count(",") == 2

def sanitize_column(col, val):
    if col=="coords":
        return dict(x=val.get("x"), y=val.get("y"), z=val.get("z"))
    else:
        return val
    
def santize_row(r):
    return {k:sanitize_column(k, v) for k,v in r.items()}

def sanitize(records):
    return [santize_row(r) for r in records]

if __name__=="__main__":
    parser = argparse.ArgumentParser(
                        prog='crop_systems',
                        description='Crop a json system dump to a bounding box',
                        epilog='countpauper@gmail.com')

    parser.add_argument('filename', help='json file containing systems') # json containing the 
    parser.add_argument('center', default='0,0,0', nargs='?', help='system name')   # TODO or comma separatedcoordinate?
    parser.add_argument('range', default=1000, nargs='?', help='manhatten distance range from center in ly') 
    # TODO radius to fine tune further. if used range will be set to radius
    args = parser.parse_args()

    print(f"finding sytems within {args.range}ly of {args.center}")

    if is_coordinate_string(args.center):
        center = (float(v) for v in args.center.split(","))
    # TODO: if args.center is decimal, find system by id64?
    elif system := find_system(args.filename, lambda row: match(row, name=args.center), ["name", "coords"]):
        print(system)
        center = coord_tuple(system.get("coords"))
    else:
        print(f"System {args.center} not found")

    bounding_box = tuple((v - args.range, v + args.range) for v in center)
    box = print_systems(args.filename, lambda x: inside(x, bounding_box))
    #print(f"{len(box)} systems in {bounding_box}")
    #box = sanitize(box)
    
    #with open("clipped_systems.json", 'w') as of:  
    #    json.dump(box, of)
