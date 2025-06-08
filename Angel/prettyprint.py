import gdb 


def short_str(o, length):
    s = str(o)
    if len(s)<=length:
        return s
    else:
        return f"{s[:length-1]}…"

def find_sub(val, name, tabs=0, index=0):
    t = val.type
    if t.name == name:  # find by type name
        index-=1
        if index<0:
            return val, index
    if t.is_scalar:
        return None, index
    for f in t.fields():
        print(f"{' '*tabs}Found field {short_str(f.name,40)} of type {short_str(f.type,40)}[{f.type.code}] in {short_str(t, 40)} #{index}")
        if f.name == name:   # find by field name 
            index -= 1
            if index<0:
                return val[f], index
        if f.name is None:
            print(f"{' '*tabs}Skipping one field of type {f.type} base={f.is_base_class} artificial={f.artificial}")
            continue
        v, index = find_sub(val[f], name, tabs+1, index)
        if v is not None:
            return v, index
    return None, index

class ExpressionPrinter:
    def __init__(self, val):
        try:
            index_field,_ = find_sub(val, "_M_index")
            index = int(str(index_field).split(" ")[0])
            storage,_ = find_sub(val, "_M_storage", index-1)
            if storage:
                print(f"ExpressionPrinter storage {storage} of type {storage.type}={storage.type.code}")
            else:
                print(f"ExpressionPrinter storage #{index} not found")
            self.val = storage  # gdb.Value object representing your C++ instance
        except Exception as e:
            print(f"ExpressionPrinter failed {e}")
    
    def to_string(self):
        # Access data members through GDB's Python API
        #return gdb.execute(f'print my_object[{self.val}]', to_string=True)
        return f"{self.val}"

class OperatorPrinter:
    def __init__(self, val):
        self.val = val['op']['sw']  
        # print(f"OperatorPrinter {self.val}")

    def to_string(self):
        unicode = str(self.val['unicode']).split(' ')[-1][2:-1]
        operands = str(self.val['operands']).split(' ')[0]
        return f"{unicode}/{operands}"  
      
    def display_hint(self):
        return None # "string"

ns_prefix = 'Angel::Logic::'


class SimplePrinter:
    def __init__(self, typ, value):
        # print(f"SimplePrinter {typ} for {value}")
        self.val = value
        self.type = str(typ)[len(ns_prefix):]  

    def to_string(self):
        return f"{self.type}({self.val})"
      
    def display_hint(self):
        return None     

def angel_printer(val):
    type_str = str(val.type)
    if type_str.startswith(ns_prefix):
        logic_type = type_str[len(ns_prefix):]
        print(f"logic_type {logic_type}")
        if logic_type == "Expression": 
            return None # cant access all alternatives. Above 5 they are anonymous unions ExpressionPrinter(val)
        elif logic_type == "Operator":
            return OperatorPrinter(val)
        elif logic_type == "Id":
            return SimplePrinter(val.type, val["name"])
        elif logic_type == "Integer":
            return SimplePrinter(val.type, val["value"])
        elif logic_type == "Boolean":
            return SimplePrinter(val.type, val["truth"])
        elif logic_type == "Variable":
            return SimplePrinter(val.type, val["name"])    
        elif logic_type == "String":
            return SimplePrinter(val.type, val["name"])    
        elif logic_type == "Tuple":
            return SimplePrinter(val.type, val["name"])  
        #elif logic_type == "Expression":
        #    print(f"Expression: type = {val.type}")
        # elif logic_type == "Association":
        #     print(f"Association {val}")
        # elif logic_type == "Conjunction":
        #     print(f"Conjunction {val}")

#        return ExpressionValuePrinter(type_str[len(ns_prefix):], val)
    return None

gdb.pretty_printers.append(angel_printer)
