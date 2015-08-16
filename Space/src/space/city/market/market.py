import sys
import weakref
from collections import defaultdict
from specification import specify, Specification, Resource
from exception import SupplyException, TradeException

class Offer(object):
    def __init__(self, market, owner, resource, amount, price):
        self.market = weakref.ref(market)
        self.owner = owner
        self.resource = resource
        self.requested_amount = amount 
        self.price = price
        # self.time = time
        # self.expiration = time + period
        self.total_money = 0

    # TODO: extend and complete in rent/hire offers only?
    def extend(self, duration):
        extra_money = duration.days() * self.price
        self.owner.money -= extra_money
        self.resource.money += extra_money
        self.total_money += extra_money 
        
    def finalize(self):
        # TODO: only works for jobs
        self.resource.fire(self.requested_amount)

    def add(self, resource):
        if isinstance(resource,Item):
            self.resource.stack(resource)

    def __lt__(self, offer):
        if self.price < offer.price:
            return True
        if self.price == offer.price:
            return self.amount() > offer.amount()
        return False

    def amount(self):
        """Return difference between target and current
        NB negative for supply, positive for demand"""
        return self.requested_amount - self.resource.amount
           
    def ready(self):
        return self.amount()==0

    def claim(self):
        self.market().cancel(self)
        return self.resource

class Supply(Offer):
    """A single resource on sale"""
    def __init__(self, market, vendor, resource, price):
        super(Supply,self).__init__(market, vendor, resource, 0, price)

    def adjust(self, new_price):
        self.resource = self.market()._immediate_sell(self.owner, self.resource, new_price)
        self.price = new_price
         
class Demand(Offer):
    """A single resource being bought"""
    def __init__(self, market, vendor, resource, amount, price):
        super(Demand,self).__init__(market, vendor, resource, amount - resource.amount, price)

    def adjust(self, new_price):
        bought_resource = self.market()._immediate_buy(self.owner, self.resource.specification, self.amount(), new_price)
        self.resource.stack(bought_resource)
        self.price = new_price

    def total_price(self):
        return self.requested_amount * self.price

class Batch(object):
    """Multiple resources being bought"""
    def __init__(self, owner, materials, location):
        self.offers=list()
        for material, amount in materials.iteritems():
            specification = specify(type=material)
            try:
                quote = location.quote(specification, amount)
                price = quote // amount  # TODO: problems with average price
            except SupplyException:
                price = 1 # TODO what to do if no quote, what to do if no history even?
            offer = location.buy(owner, specification, amount, price)   
            self.offers.append(offer)

    def ready(self):
        return all([offer.ready() for offer in self.offers])

    def claim():
        result = [offer.claim() for offer in self.offers]
        self.offers = None
        return result

    def total_price(self):
        return sum([offer.total_money for offer in self.offers])

class History(object):
    def __init__(self, amount, price):
        self.time = 0
        self.amount = amount
        self.price = price

class Listing(object):
    def __init__(self):
        self.supply = []
        self.demand = []
        self.history = []

class Market(object):
    def __init__(self):
        # TODO: instead of default dict, initialize all sold products with empty lists
        self.products = defaultdict(lambda:Listing())

    def sell(self, vendor, resource, price):
        """List a resource on the market, creating an offer record for it under it's product specification"""
        if not isinstance(resource, self.type):
            raise TradeException("Market {0} doesn't carry resource {1}", self, resource)

        remaining_resource = self._immediate_sell(vendor, resource, price)
        if remaining_resource.amount>0:
            offer = Supply(self, vendor, remaining_resource, price)
            self.products[resource.specification].supply.append(offer)
            self.products[resource.specification].supply.sort() # TODO optimize by adding sorted
            return offer

    def record(self, resource, amount, price):
        self.products[resource.specification].history.append(History(amount, price))
        print "Sold {1} / {0} @ ${2}".format(resource, amount, price)

    def _immediate_sell(self, vendor, resource, price):
        """Split resource and sell to existing demand, 
        return remaining unsellable resource
        only completely fullfills offers whose amount and price is met
        fullfills order from highest to lowest price"""
        demand = self.demand(resource.specification, price)  
        total_amount = sum([offer.resource.amount for offer in demand])

        for offer in demand:
            if offer.ready():
                continue  
            if resource.amount>offer.amount():
                sold_amount = offer.amount()
            else:
                sold_amount = resource.amount
            resource = resource.purchase(sold_amount)
            total_price= offer.price * sold_amount
            offer.owner.money -= total_price
            vendor.money += total_price
            self.record(sold_resource, sold_amount, total_price)
#TODO can't stack agents, collect contracts dictionary instead, so they can be paid, 
#contract items when consumed fire the worker, something like that
            offer.resource.stack(resource)
            offer.total_money += total_price
            if resource.amount==0:
                break
        return resource
 
    def supply(self, product, price=sys.maxint):
        """return list of supply offers for at most the given price
        sorted from lowest to highest price"""
        return [offer for offer in self.products[product].supply if offer.price<=price]

    def demand(self, product, price=-sys.maxint):
        """return list of demand offers for at least the given price
        sorted from highest to lowest price"""
        return reversed([offer for offer in self.products[product].demand if offer.price>=price])

    def buy(self, buyer, product, amount, price):
        """Place an order for the given product x amount to be fullfilled at that price"""
        if not issubclass(product.type, self.type):
            raise TradeException("Market {0} doesn't carry resource {1}", self, product)
        resource = self._immediate_buy(buyer, product, amount, price)
        offer = Demand(self, buyer, resource, amount - resource.amount, price)
        self.products[product].demand.append(offer)
        self.products[product].demand.sort() # TODO optimize 
        return offer

    def _immediate_buy(self, buyer, product, amount, price):
        """Return an resource made up of offered resources whose price is less"""
        supply = self.supply(product, price)    # supply offers for that price
        resource = product.type(specification=product, amount=0)
        for offer in supply:
            if offer.ready():
                continue
            if offer.resource.amount>=amount:
                sold_amount = amount
            else:
                sold_amount = offer.resource.amount
            sold_resource = offer.resource.purchase(sold_amount)
            total_price = sold_amount * offer.price
            buyer.money -= total_price
            offer.owner.money += total_price
            self.record(sold_resource, sold_amount, total_price)
            amount -= sold_amount
            resource.stack(sold_resource)
            offer.total_money += total_price
            if amount==0:
                break
        return resource

    def trades(self, product):
        """Return True if this market trades in this specification or type"""
        if isinstance(product, Specification):
            return issubclass(product.type, self.type) # TODO product in self.products
        elif isinstance(product, Resource):
            return isinstance(product.type, self.type)
        elif issubclass(product, self.type):
            return True
        return False

    def _compute_price(self, offers, amount, exc_type):
        total_price = 0
        for offer in offers:
            if offer.amount()>=amount:
                total_price += offer.asking_price * offer.amount
                return total_price
            else:
                total_price += offer.amount() * offer.price
                amount -= offer.amount()
            raise exc_type("Less than {1} {0} offered", offer.resource, amount)
        return total_price

    def quote(self, specification, amount):
        if not issubclass(specification.type, self.type):
            raise TradeException("Market {0} doesn't carry resource {1}", self, specification)
        offers = self.supply(specification)
        if not offers:
            raise SupplyException("No {1} {0} offered", specification, amount)
        return self._compute_price(offers, amount, SupplyException)

    def turnover(self, specification, amount):
        offers = self.demand(specification)
        return self._compute_price(offers, amount, DemandException)

    def shop(self, product):
        """ Retrieve a dictionary of specification: total_amount"""
        if isinstance(product, Specification):    # product is a specifation, return that specific listing
            return { product: sum([offer.resource.amount for offer in self.supply(product)]) }
        elif issubclass(product, self.type):  # product is a class, return all specifications of that class
            return dict([(specification, sum([offer.resource.amount for offer in listing.supply])) for specification, listing in self.products.iteritems()
                    if issubclass(specification.type, product)])

    def _remove_offer(self, offer):
        if isinstance(offer, Supply):
            self.products[offer.resource.specification].supply.remove(offer)
        elif isinstance(offer, Demand):
            self.products[offer.product].demand1.remove(offer)

    def cancel(self, offer):
        self._remove_offer(offer)


