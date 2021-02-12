import csv
import json

def convert(csv, key, fields):
	data={}
	for row in reader:
		data[row[key].strip().lower()]={k.lower():row[k] for k in fields if k in row}
	return data

if __name__ == "__main__":
	with open('c:\\temp\\spells.csv', newline='') as csvfile:
		reader = csv.DictReader(csvfile)
		data = convert(reader, 'Name', ['Level','Attack','Save'])
		with open('.\\spells.json',mode='w',newline='') as jsonFile:
			jsonFile.write(json.dumps(data,indent='\t'))
