import sys
import weakref
from collections import defaultdict
from utility.format_exception import FormatException
from space.item import Good
from specification import specify, Specification, Resource

class _MarketException(FormatException):
    pass

class TradeException(_MarketException):
    """Market does not trade in that product"""
    pass

class SupplyException(_MarketException):
    """Not enough supply"""
    pass

class CostException(SupplyException):
    """Not enough supply for that price"""
    pass

class DemandException(_MarketException):
    """Not enough supply"""
    pass

class PriceException(DemandException):
    """Not enough demand for that price"""
    pass


class Offer(object):
    def __init__(self, market, owner, item, amount, price):
        self.market = weakref.ref(market)
        self.owner = owner
        self.item = item
        self.requested_amount = amount 
        self.price = price
        # self.time = time
        # self.expiration = time + period
        self.total_money = 0

    def total_price(self):
        return self.amount * self.price

    def __lt__(self, offer):
        if self.price < offer.price:
            return True
        if self.price == offer.price:
            return self.amount() > offer.amount()
        return False

    def amount(self):
        """Return difference between target and current
        NB negative for supply, positive for demand"""
        return self.requested_amount - self.item.amount
           
    def ready(self):
        return self.amount()==0

    def claim(self):
        self.market().cancel(self)
        return self.item

class Supply(Offer):
    """A single item on sale"""
    def __init__(self, market, vendor, item, price):
        super(Supply,self).__init__(market, vendor, item, 0, price)

    def adjust(self, new_price):
        self.item = self.market()._immediate_sell(self.owner, self.item, new_price)
        self.price = new_price
         
class Demand(Offer):
    """A single item being bought"""
    def __init__(self, market, vendor, item, amount, price):
        super(Demand,self).__init__(market, vendor, item, amount - item.amount, price)

    def adjust(self, new_price):
        bought_item = self.market()._immediate_buy(self.owner, self.item.specification, self.amount(), new_price)
        self.item.stack(bought_item)
        self.price = new_price

class Batch(object):
    """Multiple items being bought"""
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

class _Market(object):
    def __init__(self):
        # TODO: instead of default dict, initialize all sold products with empty lists
        self.products = defaultdict(lambda:Listing())

    def sell(self, vendor, item, price):
        """List an item on the market, creating an offer record for it under it's product specification"""
        if not isinstance(item, self.type):
            raise TradeException("Market {0} doesn't carry item {1}", self, item)

        remaining_item = self._immediate_sell(vendor, item, price)
        if remaining_item.amount>0:
            offer = Supply(self, vendor, remaining_item, price)
            self.products[item.specification].supply.append(offer)
            self.products[item.specification].supply.sort() # TODO optimize by adding sorted
            return offer

    def record(self, item, price):
        self.products[item.specification].history.append(History(item.amount, price))
        print "Sold {0} @ ${1}".format(item, price)

    def _immediate_sell(self, vendor, item, price):
        """Split item and sell to existing demand, 
        return remaining unsellable item
        only completely fullfills offers whose amount and price is met
        fullfills order from highest to lowest price"""
        demand = self.demand(item.specification, price)  
        total_amount = sum([offer.item.amount for offer in demand])

        for offer in demand:
            if offer.ready():
                continue  
            if item.amount>offer.amount():
                sold_item = item.split(offer.amount())
            else:
                sold_item = item
            total_price= offer.price * sold_item.amount
            offer.owner.money -= total_price
            vendor.money += total_price
            self.record(sold_item, total_price)
            offer.item.stack(item)
            offer.total_money += total_price
            if item.amount==0:
                break
        return item
 
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
            raise TradeException("Market {0} doesn't carry item {1}", self, product)
        item = self._immediate_buy(buyer, product, amount, price)
        offer = Demand(self, buyer, item, amount - item.amount, price)
        self.products[product].demand.append(offer)
        self.products[product].demand.sort() # TODO optimize 
        return offer

    def _immediate_buy(self, buyer, product, amount, price):
        """Return an item made up of offered items whose price is less"""
        supply = self.supply(product, price)    # supply offers for that price
        item = product.type(specification=product, amount=0)
        for offer in supply:
            if offer.ready():
                continue
            if offer.item.amount>=amount:
                sold_item = offer.item.split(amount)
            else:
                sold_item = offer.item
            total_price = sold_item.amount * offer.price
            buyer.money -= total_price
            offer.owner.money += total_price
            self.record(sold_item, total_price)
            amount -= sold_item.amount
            item.stack(sold_item)
            offer.total_money += total_price
            if amount==0:
                break
        return item

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
            raise exc_type("Less than {1} {0} offered", offer.item, amount)
        return total_price

    def quote(self, specification, amount):
        if not issubclass(specification.type, self.type):
            raise TradeException("Market {0} doesn't carry item {1}", self, specification)
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
            return { product: sum([offer.item.amount for offer in self.supply(product)]) }
        elif issubclass(product, self.type):  # product is a class, return all specifications of that class
            return dict([(specification, sum([offer.item.amount for offer in listing.supply])) for specification, listing in self.products.iteritems()
                    if issubclass(specification.type, product)])

    def _remove_offer(self, offer):
        if isinstance(offer, Supply):
            self.products[offer.item.specification].supply.remove(offer)
        elif isinstance(offer, Demand):
            self.products[offer.product].demand1.remove(offer)

    def cancel(self, offer):
        self._remove_offer(offer)

class GoodsMarket(_Market):
    def __init__(self):
        super(GoodsMarket,self).__init__()
        self.type = Good

class JobsMarket(_Market):
    def __init__(self):
        super(GoodsMarket,self).__init__()
        self.type = Profession

class ServiceMarket(_Market):
    pass    # TODO: add buildings AND services



