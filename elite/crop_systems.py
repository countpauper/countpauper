import argparse 
import orjson
import gzip
from aabb import AABB


def match(record, **kwargs):
    return all(record[k] == v for k,v in kwargs.items())

def coord_tuple(coords):
    return tuple(coords.get(axis) for axis in "xyz")

def query_file_lines(file, query, action, **kwargs):    
    rows, matches = 0,0
    result = None
    for line in file: 
        line = line.strip(" \n\t,")
        if line[0:1] in "[]":
            continue
        # record = partial_json(line, ["id64", "name", "coords.x", "coords.y", "coords.z"])
        record = orjson.loads(line)
        rows += 1 
        if query(record):
            matches += 1
            if result := action(record, **kwargs):
                return result
        if rows % 100000 == 0:
            print(f"{matches}/{rows}")
    return result   

def query_systems(file, query, action, **kwargs):
    if type(file)==str:
        if file.endswith(".json.gz"):
            with gzip.open(file, 'rt') as text_file:
                return query_file_lines(text_file, query, action, **kwargs)
        elif file.endswith(".json"):
            with open(file, "r") as text_file:
                return query_file_lines(text_file, query, action, **kwargs)
        else:
            raise RuntimeError(f"Unsupported input file type {file}. only .json and .json.gz are supported")
    return query_file_lines(file, query, action, columns=None)

def early_out(record, _):
    return record

def find_system(filename, query):
    return query_systems(filename, query, lambda x: early_out(x))

def collect(record, result):
    result.append(record)
    return None # len(result)>=1000 TODO chunking?
    
def find_systems(filename, query):
    result = []
    query_systems(filename, query, lambda x: collect(x, result))
    return result

def flatten(record, prefix="" ):
    result = dict()
    for k, v in record.items():
        key = f"{prefix}.{k}" if prefix else k
        if type(v) is dict: 
            sub_record = flatten(v, key)
            result.update(sub_record)
        else:
            result[key] = v
    return result

def print_csv(outfile, record, columns):
    outfile.write(",".join(str(record.get(k)) for k in columns)+'\n')
    
def print_json(outfile, record):
    if outfile.tell()>3:
        outfile.write(b",\n")
    outfile.write(b"\t")
    outfile.write(orjson.dumps(record))

def crop_columns(record, columns):
    if columns is None:
        return record 
    else:
        return {k: v for k,v in record.items() if k in columns}
    
def print_systems(filename, outfile, columns, query):
    if outfile.endswith(".csv"):
        if not columns:
            raise RuntimeError("Column selection is required for CSV output")
        with open(args.outfile, 'w') as f:
            f.write(",".join(columns))
            f.write("\n")
            query_systems(filename, query, lambda x: print_csv(f, flatten(x), columns))
    elif outfile.endswith(".json"):
        with open(args.outfile, 'wb') as f:
            f.write(b"[\n")
            query_systems(filename, query, lambda x: print_json(f, crop_columns(x, columns)))
            f.write(b"\n]\n")
    else:
        raise RuntimeError(f"Unsupported output format {outfile}")

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

def flatten(l):
    """flatten a lists of lists to a list"""
    return [item for sub_list in l for item in sub_list]

if __name__=="__main__":
    parser = argparse.ArgumentParser(
                        prog='crop_systems',
                        description='Crop a json system dump to a bounding box',
                        epilog='countpauper@gmail.com')

    parser.add_argument('filename', help='(gzipped) json file containing systems')  # TODO support .json.gz
    parser.add_argument('-o', '--outfile', default='cropped_systems.json', nargs='?', help='File to write cropped systems to. JSON and CSV is supported') # TODO: support json as well
    parser.add_argument('-c', '--center', default='0,0,0', nargs='?', help='system name or coordinates in ly') # TODO or system id   
    parser.add_argument('-r', '--range', default=1000, nargs='?', help='Distance range from center in ly') 
    parser.add_argument('-col', '--column', action='append', help="Add columns to select. By default all are. Can be passed multiple times and values can be comma separated.")
    # TODO radius to fine tune further. if used range will be set to radius as well
    args = parser.parse_args()

    print(f"finding sytems within {args.range}ly of {args.center}")

    if is_coordinate_string(args.center):
        center = (float(v) for v in args.center.split(","))
    # TODO: if args.center is decimal, find system by id64?
    elif system := find_system(args.filename, lambda row: match(row, name=args.center)):
        print(system)
        center = coord_tuple(system.get("coords"))
    else:
        print(f"System {args.center} not found")

    columns = flatten([col.split(",") for col in args.column]) if args.column else None
    bounding_box = AABB( ((v - args.range, v + args.range) for v in center))
    print_systems(args.filename, args.outfile, columns, lambda record: bounding_box.inside(coord_tuple(record.get("coords"))))
    