import gdb 
import traceback
import sys

variants=None

def short_str(o, length):
    s = str(o)
    if len(s)<=length:
        return s
    else:
        return f"{s[:length-1]}…"

def find_sub(val, name, index=0, tabs=0):
    t = val.type
    if t.name == name:  # find by type name
        index-=1
        if index<0:
            return val, index
    if t.is_scalar:
        return None, index
    for f in t.fields():
        #print(f"{' '*tabs}Found field {short_str(f.name,40)} of type {short_str(f.type,40)}[{f.type.code}] in {short_str(t, 40)} #{index}")
        if f.name == name:   # find by field name 
            index -= 1
            if index<0:
                return val[f], index
        if f.name is None:
            #print(f"{' '*tabs}Skipping one field of type {f.type} base={f.is_base_class} artificial={f.artificial}")
            continue
        v, index = find_sub(val[f], name, index, tabs+1)
        if v is not None:
            return v, index
    return None, index

class ExpressionPrinter:
    def __init__(self, val):
        index_field,_ = find_sub(val, "_M_index")
        self.index = int(str(index_field).split(" ")[0])
        self.val,_ = find_sub(val, "_M_storage", self.index-1)
        if not self.val:
            #print(f"ExpressionPrinter storage #{self.index} not found")
            pass
        elif self.val.type.name == "std::monostate": 
            #print(f"ExpressionPrinter storage with no value at #{self.index}")
            self.val = None
        else:
            #print(f"ExpressionPrinter storage {self.val} of type {self.val.type}={self.val.type.code}")
            pass
    
    def to_string(self):
        # Access data members through GDB's Python API
        #return gdb.execute(f'print my_object[{self.val}]', to_string=True)
        if self.val:
            return f"{self.val}"
        else:
            return f"{variants[self.index]}(<unknown>)"

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

def expression_variants():
    alternatives=[]
    exp_type = gdb.lookup_type(f"{ns_prefix}ExpressionVariant")
    exp_var_type = exp_type.strip_typedefs()
    # TODO for some reason  exp_var_type.template_argument(n) gives a syntax error so doing it the hard way 
    header = "std::variant<"
    alt_list = exp_var_type.name[len(header):-1]
    alternatives = [alt[len(ns_prefix):] for alt in alt_list.split(", ")]
    alternatives[0] = "Null"    # std::monostate
    return alternatives

def angel_printer(val):
    try: 
        global variants
        variants = expression_variants() 
        type_str = str(val.type)
        if type_str.startswith(ns_prefix):
            logic_type = type_str[len(ns_prefix):]
            if logic_type == "Expression": 
                return ExpressionPrinter(val)
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
            # TODO but gdb seems to have these with value in storage?
            #elif logic_type == "Expression":
            #    print(f"Expression: type = {val.type}")
            # elif logic_type == "Association":
            #     print(f"Association {val}")
            # elif logic_type == "Conjunction":
            #     print(f"Conjunction {val}")

    #        return ExpressionValuePrinter(type_str[len(ns_prefix):], val)
        return None
    except Exception as e:
        stack = traceback.extract_stack()# [:-1]
        print(f"{e} at\n{traceback.format_exc()}") # \n{traceback.format_exc()}")



gdb.pretty_printers.append(angel_printer)
