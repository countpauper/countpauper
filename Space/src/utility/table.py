import csv

class Table(object):
    def __init__(self, header):
        self.header = header
        self.data = []
        self.delimiter = '\t'

    def add(self, data):
        self.data.append = data

    def write(self, fn):
        with open(fn, 'wb') as csvfile:
            writer = csv.DictWriter(csvfile, fieldnames=self.header, delimiter = self.delimiter)
            writer.writeheader()
            writer.writerows(self.data)


    def read(self,fn):
        with open(fn, 'rb') as csvfile:
            reader = csv.DictReader(csvfile, delimiter = self.delimiter)
            reader.readrow(self.header)
            reader.readrows(self.data)
