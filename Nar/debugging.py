import sys

def is_debugging():
	gettrace = getattr(sys, 'gettrace', None)
	if gettrace is None:
		raise EnvironmentError("No sys.gettrace")
	return bool(gettrace())
