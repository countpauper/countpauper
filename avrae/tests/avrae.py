### Avrae built in emulation for unit testing gvars
import math

def typeof(v):
    if type(v)==list:
        return 'SafeList'
    elif type(v)==dict:
        return 'SafeDict'
    else:
        return type(v).__name__ 
    
def err(msg):
    raise RuntimeError(msg)

sqrt = math.sqrt
ceil = math.ceil
floor = math.floor