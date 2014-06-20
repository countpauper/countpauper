from utility.obsever import Observer

class Organization(Observer):
    def __init__(self, domain):
        self.domain = domain
        self.responsibilities = tuple()

class Government(Organization):
        
    def tick(self, time, period):
        for location in self.domain:
            self.rule(location, period)

    def rule(self, location, period):
        for recipe in self.responsibilities:
            amounts = location.availability(process)
            amount = min(amounts)
            if amount:
                process=location.order(recipe, amount)
                process.register(self)

    def notify(self, observable, event):
        if isinstance(observable, Process) and event=='done':
            location.sell(process.products)
