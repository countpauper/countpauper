import sys
from collections import defaultdict
from utility.format_exception import FormatException
from space.item import Good
from specification import Specification

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
    def __init__(self, owner, amount, price):
        self.price = price
        # self.time = time
        # self.expiration = time + period
        self.owner = owner
        self.amount = amount 

    def total_price(self):
        return self.amount * self.price

    def __eq__(self, offer):
        return self.amount == offer.amount and self.price == offer.price

    def __lt__(self, offer):
        if self.price < offer.price:
            return true
        if self.price == offer.price:
            return self.item.amount > offer.item.amount
        return false
       
class Supply(Offer):
    def __init__(self, vendor, item, price):
        self.item = item
        super(Supply,self).__init__(vendor, self.item.amount, price)

class Demand(Offer):
    def __init__(self, vendor, product, amount, price):
        super(Demand,self).__init__(vendor, amount, price)

class Deal(object):
    def __init__(self, amount, price):
        self.amount = amount
        self.price = price
        self.offers = list()

    def total_price(self):
        return self.amount * self.price

    def accept(self, buyer):
        buyer.money -= self.total_price()
        # gather items
        remaining_amount = self.amount
        total_item = None
        for offer in self.offers:
            if offer.item.amount<=remaining_amount:
                sold_item = offer.item.split(offer.item.amount)
                self._remove_offer(offer)   # sold out
                remaining_amount -= sold_item.amount
            else:
                sold_item = offer.item.split(remaining_amount)
                remaining_amount = 0
            # pay the vendor
            offer.vendor.money += self.price * sold_item.amount
            if not total_item:
                total_item = sold_item
            else:
                total_item.stack(sold_item)
        return total_item

    def decline(self):
        pass

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
            offer = Supply(vendor, remaining_item, price)
            self.products[item.specification].supply.append(offer)
            self.products[item.specification].supply.sort() # TODO optimize by adding sorted
            return offer

    def _immediate_sell(self, vendor, item, price):
        """Split item and sell to existing demand, 
        return remaining unsellable item
        only completely fullfills offers whose amount and price is met
        fullfills order from highest to lowest price"""
        demand = self.demand(item.specification, price)  
        total_amount = sum([offer.item.amount for offer in demand])

        for offer in reversed(demand):   # reverse: highest price first
            if item.amount>=offer.amount:
                sold_item = item.split(offer.amount)
                offer.owner.money -= offer.total_price()
                vendor.money += offer.total_price()
                offer.item = sold_item  # for buyer to pick up
                offer.owner.give(sold_item)  # TODO: signal buyer at end of offer
                self._remove_offer(offer)
        return item
 
    def supply(self, product, price=sys.maxint):
        """return list of supply offers for at most the given price"""
        return [offer for offer in self.products[product].supply if offer.price<=price]

    def demand(self, product, price=-sys.maxint):
        """return list of demand offers for at least the given price"""
        return [offer for offer in self.products[product].demand if offer.price>=price]

    def buy(self, buyer, product, amount, price):
        if not issubclass(product.type, self.type):
            raise TradeException("Market {0} doesn't carry item {1}", self, product)
        try:
            return self._immediate_buy(buyer, product, amount, price)
        except SupplyException:
            offer = Demand(buyer, product, amount, price)
            self.products[product].demand.append(offer)
            self.products[product].demand.sort() # TODO optimize 
            return offer

    def _immediate_buy(self, buyer, product, amount, price):
        """Return an item made up of offered items whose price is less
        raises a SupplyException if the complete amount can't be fullfilled"""
        supply = self.supply(product, price)    # supply offers for that price
        total_amount = sum([offer.item.amount for offer in supply])
        if total_amount<amount:
            raise CostException("No {1} x {0} on sale for {2}", product, amount, price)
        item = None
        for offer in supply:
            if offer.item.amount>=amount:
                sold_item = offer.item.split(amount)
            else:
                sold_item = offer.item
            total_price = sold_item.amount * offer.price
            buyer.money -= total_price
            offer.owner.money += total_price
            if item is None:
                item = sold_item
            else:
                item.stack(sold_item)
        return item

    def trades(self, product):
        """Return True if this market trades in this specification or type"""
        if isinstance(product, Specification):
            return product in self.products
        elif issubclass(product, self.type):
            return True
        return False

    def quote(self, specification, amount):
        if not issubclass(specification.type, self.type):
            raise TradeException("Market {0} doesn't carry item {1}", self, specification)
        offers = self.supply(specification)
        total_price = 0
        for offer in offers:
            item = offer.item
            if item.amount>amount:
                total_price += offer.asking_price * amount
                return total_price
            else:
                total_price += item.amount * offer.asking_price
                amount -= item.amount
        raise StockException("Less than {1} {0} on sale", specification, amount)

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
            self.products[offer.product].demand.remove(offer)

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



