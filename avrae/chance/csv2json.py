import csv
import json

def deduck(v):
	if type(v) is not str:
		return v
	if v.lower()=='y':
		return True
	if v.lower()=='n':
		return False
	if v.isdigit():
		return int(v)
	return v

def convert(csv, key, fields):
	data={}
	for row in reader:
		row={k:deduck(e) for k,e in row.items() if k and e}
		data[row[key].strip().lower()]={k.lower():row[k] for k in fields if k in row}
	return data

if __name__ == "__main__":
	with open('.\\spells.csv', newline='') as csvfile:
		reader = csv.DictReader(csvfile)
		data = convert(reader, 'Name', ['Level','Attack','Save'])
		with open('.\\spells.json',mode='w',newline='') as jsonFile:
			jsonFile.write(json.dumps(data,indent='\t'))
