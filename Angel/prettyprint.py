import gdb 
import traceback

ns_prefix = 'Angel::Logic::'

class ExpressionPrinter:
    def val2str(self, v):
        v.fetch_lazy()
        idx = int(str(v["_M_index"]).split(" ")[0])
        if idx not in range(0, 32): # TODO count variants 
            return "<Uninitialized>"
        print(f"idx {idx}")
        if v.is_optimized_out:
            return "<Optimized out>"
        try:
            obj_addr = int(v.address)
            if obj_addr==0:
                return "<nullptr>"
            else:
                result = gdb.parse_and_eval(f"{ns_prefix}to_string(*({ns_prefix}Expression*){obj_addr})")
                return str(result).strip("\"")
        except Exception as e:
            print(f"Expression to string conversion failed: {e}")
            return f"<Failed>"

    def __init__(self, val):
        self.val = val
        self.s = self.val2str(val)

    def to_string(self):
        return self.s

def unicode_unescape(s):
    return s.replace('\\x','\\u') # .encode('latin1').decode('unicode_escape')

class OperatorPrinter:
    def __init__(self, val):
        self.val = val['op']['sw']  
        # print(f"OperatorPrinter {self.val}")

    def to_string(self):
        code = str(self.val['unicode']).split(' ')[-1][2:-1]
        code = unicode_unescape(code)
        operands = str(self.val['operands']).split(' ')[0]
        return f"{code}/{operands}"  
      
    def display_hint(self):
        return None # "string"


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
    try: 
        type_str = str(val.type)
        #print(f"type {type_str}")
        if type_str.startswith("const "):
            type_str = type_str[6:]
        if type_str.startswith(ns_prefix):
            logic_type = type_str[len(ns_prefix):]
            if logic_type == "Expression": 
                return None # ExpressionPrinter(val)
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
                return SimplePrinter(val.type, val["value"])    
            #elif logic_type == "Tuple":
            #S    return SimplePrinter(val.type, val["name"])  
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
