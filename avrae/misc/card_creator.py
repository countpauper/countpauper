import csv
import json
import sys


def convert_item(data, item):
	if type(item) is str:
		for k,v in data.items():
			array_tag='{'+ k+'[]}'
			## TODO: only supports one array tag or it will get KeyErrors
			if array_tag in item:
				if v:
					values = v.split('\n')
					item=item.replace(array_tag,'{'+k+'}')
					return tuple(item.format(**{**data, **{k:split_v}}) for split_v in values)
				else:
					return None
		if all(v or not '{'+k+'}' in item for k,v in data.items()):
			return item.format(**data)
		else:
			return None
	elif type(item) is list:
		result=list()
		for i in item:
			v = convert_item(data, i)
			if type(v) is tuple and v:
				result+=list(v)
			elif v is not None:
				result.append(v)
		return result
	else:
		return item

def apply_template(data, template):
	result=dict()
	for key, value in template.items():
		value= convert_item(data, value)
		if value is not None:
			result[key] = value
	return result

def convert(csv, template):
	data=[]
	for row in reader:
		row_data={k:e for k,e	 in row.items() if k}
		if not row_data.get('skip'):
			data.append(apply_template(row_data,template))
	return data

template = dict(
	npc=dict(
		count=1,
		title="{name}",
		tags=["{tag}"],	# TODO find a way to indicate splitting a column? not by comma, it's already csv
		title_size="15",	# TODO auto scale to name length
		color="",
		icon="{icon}",
		background_image="{image}",
		contents=[
			"section|Description",
			"text|{description[]}",
			"property|Speech|{speech}",
			"property|Character|{character}",
			"property|Mannerism|{manner}",
			"property|Quote|{quote}",
			"property|Skills|{skill}",
			"section|background",	# TODO conditional
			"property|faction|{faction}",
			"text|{background[]}",
			"section|Goals",
			"bullet|{goal[]}",
			"section|Notes",
			"text|{notes[]}",
			"fill"
		]
	)
)

def parse_args(args):
	if len(args)>=2:
		template_name=args.pop(0)
		input_fn=args.pop(0)
		if args:
			return template_name, input_fn, args.pop(0)
		else:
			if input_fn.lower().endswith('.csv'):
				return template_name, input_fn, input_fn.lower().replace('.csv','.json')
			elif input_fn.lower().endswith('.txt'):
					return template_name, input_fn, input_fn.lower().replace('.txt', '.json')
			else:
				return template_name, input_fn, input_fn+ '.json'
	else:
		raise exception("provide input csv")

if __name__ == "__main__":
	template_name, input_fn, output_fn=parse_args(sys.argv[1:4])
	with open(input_fn, newline='') as csvfile:
		delimiter='\t' if input_fn.endswith('.txt') else ','
		reader = csv.DictReader(csvfile, delimiter=delimiter)
		data = convert(reader, template[template_name])
		with open(output_fn,mode='w',newline='') as jsonFile:
			jsonFile.write(json.dumps(data,indent='  '))
		print('created '+output_fn)