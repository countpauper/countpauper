class Organization(object):
    def __init__(self, domain):
        self.domain = domain
        self.responsibilities = tuple()

class Government(Organization):
        
    def tick(self, time, period):
        for location in self.domain:
            self.rule(location, period)

    def rule(self, location, period):
        for process in self.responsibilities:
            amounts = location.availability(process)
            location.order(process, min(amounts))


