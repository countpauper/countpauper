config=load_json(get('march','{}'))
definition=dict(
	verbose=false,
	diagonal=None
)
args=argparse('''&*&''')
new_config={ var:args.last(var) for var in definition }
