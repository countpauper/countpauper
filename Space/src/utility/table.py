import csv

# TODO: test
# TODO: add line by line while open

class Table(object):
    def __init__(self, header):
        self.header = header
        self._data = []
        self.delimiter = '\t'
        self.file = None
        self.writer = None

    def write(self, fn):
        with open(fn, 'wb') as csvfile:
            self._write(self._create_writer(csvfile))

    def _create_writer(self, file=None):
        if not file:
            file = self.file
        return csv.DictWriter(file, fieldnames=self.header, delimiter = self.delimiter)

    def _write(self, writer=None):
        if not writer:
            writer = self.writer
        writer.writeheader()
        writer.writerows(self._data)
           
    def open(self,fn):
        self.file = open(fn,"wb",1)
        self.writer=self._create_writer()
        self._write()   # write header and data until now
        
    def append(self, data):
        self._data.append(data)
        if self.file:
            self.writer.writerow(data)

    def close():
        self.file.close()
        self.file = None

    def read(self,fn):
        with open(fn, 'rb') as csvfile:
            reader = csv.DictReader(csvfile, delimiter = self.delimiter)
            reader.readrow(self.header)
            reader.readrows(self._data)
