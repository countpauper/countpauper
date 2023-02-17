import csv
import json

def deduck(v):
	if type(v) is not str:
		return v
	if v.lower()=='y':
		return True
	if v.lower()=='n':
		return False
	# range specific
	if v.lower()=='touch':
		return 5
	if v.lower()=='self':
		return 0
	if v.lower().startswith('self ('):
		v=v[6:-1].strip()
	if v.lower().startswith('touch ('):
		v=v[7:-1].strip()
	if v.lower()=='unlimited':
		return 2^31
	if v.lower()=='sight':	# sight range outdoors is ~2 miles
		return 2000
	if v.lower().endswith('mile'):
		return 1056
	if v.lower().endswith(' miles'):
		return int(v[:-6])*1056
	if 'ft' in v.lower():
		v=v[:v.lower().index('ft')].strip()	# also cuts of radius range
	# generic again
	v=v.replace(',','')	# 1,000
	if v.isdecimal():
		return int(v)
	return v

def convert(csv, key, fields):
	data={}
	for row in reader:
		row={k:e for k,e in row.items() if k and e}
		data[row[key].strip().lower()]={k.lower():deduck(row[k]) for k in fields if k in row}
	return data

def convert_speed(speed):
	if type(speed) is int:
		return dict(walk=speed)
	output = dict()
	for speedStr in speed.split(' '):
		if speedStr.isdecimal():
			output['walk']=int(speedStr)
		else:
			speedTypes=dict(F='fly', C='climb', S='swim', B='burrow', H='hover')
			for postfix, speedType in speedTypes.items():
				if speedStr.endswith(postfix):
					output[speedType]=int(speedStr[:-len(postfix)])
					break
			else:
				raise RuntimeError(f'''Unrecognized speed string "{speedStr}"''')
	return output

def convert_beasts(input):
	output={k:v for k,v in input.items()}
	for key, value in output.items():
		value['speed']=convert_speed(value['speed'])
		value['type']=value['type'].lower()
	return output

if __name__ == "__main__":
	# spells
	with open('.\\spells.csv', newline='') as csvfile:
		reader = csv.DictReader(csvfile)
		data = convert(reader, 'Name', ['Level', 'Attack', 'Save', 'Range'])
		with open('.\\spells.json',mode='w',newline='') as jsonFile:
			jsonFile.write(json.dumps(data,indent='\t'))

	# beasts (todo arguments)
	with open('..\\misc\\beasts.csv', newline='') as csvfile:
		reader = csv.DictReader(csvfile)
		data = convert(reader, 'Name', ['Type', 'CR', 'AC', 'HP', 'HD', 'Speed'])
		data = convert_beasts(data)
		with open('..\\misc\\beasts.json',mode='w',newline='') as jsonFile:
			jsonFile.write(json.dumps(data,indent='\t'))
