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
	if v.lower()=='sight':	# sight range outdoors is ~2 mules
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

if __name__ == "__main__":
	with open('.\\spells.csv', newline='') as csvfile:
		reader = csv.DictReader(csvfile)
		data = convert(reader, 'Name', ['Level','Attack','Save','Range'])
		with open('.\\spells.json',mode='w',newline='') as jsonFile:
			jsonFile.write(json.dumps(data,indent='\t'))
