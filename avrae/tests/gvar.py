from importlib.util import spec_from_loader, module_from_spec
from importlib.machinery import SourceFileLoader 

def import_gvar(file, module):
    spec = spec_from_loader(module, SourceFileLoader(module, file))
    module = module_from_spec(spec)
    spec.loader.exec_module(module)
    return module

