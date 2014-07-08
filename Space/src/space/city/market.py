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

class PriceException(_MarketException):
    """Not enough supply for that price"""
    pass

class StockException(_MarketException):
    """Not enough offered"""
    pass

class Offer(object):
    def __init__(self, vendor, item, asking_price):
        self.item = item
        self.asking_price = asking_price
        # self.time = time
        self.vendor = vendor

    def __eq__(self, offer):
        return self.item == offer.item and self.asking_price == offer.asking_price

    def __lt__(self, offer):
        if self.asking_price < offer.asking_price:
            return true
        if self.asking_price == offer.asking_price:
            return self.item.amount > offer.item.amount
        return false
       
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

class _Market(object):
    def __init__(self):
        # TODO: instead of default dict, initialize all sold products with empty lists
        self.offers = defaultdict(lambda:list())
        self.history = defaultdict(lambda:list())
         
    def sell(self, vendor, item, asking_price):
        """List an item on the market, creating an offer record for it under it's product specification"""
        if not isinstance(item, self.type):
            raise TradeException("Market {0} doesn't carry item {1}", self, item)
        offer = Offer(vendor, item, asking_price)
        self.offers[item.specification].append(offer)
        self.offers[item.specification].sort() # TODO optimize by adding sorted
        return offer

    def trades(self, product):
        """Return True if this market trades in this specification or type"""
        if isinstance(product, Specification):
            return issubclass(product.type, self.type)
        elif issubclass(product, self.type):
            return True
        return False

    def _offers(self, product):
        """Return a list of offers that match product specification or class type"""

        if isinstance(product, Specification):    # product is a specifation, return that specific listing
            return self.offers[product]
        elif issubclass(product, self.type):  # product is a class, return all specifications of that class
            return [offer for specification, offers in self.offers.iteritems() if issubclass(specification.type, product) for offer in offers]

    def price(self, product):
        # TODO: use history instead of hack
        try:
            return self.quote(product, 1)
        except StockException:
            return 100

    def quote(self, specification, amount):
        if not issubclass(specification.type, self.type):
            raise TradeException("Market {0} doesn't carry item {1}", self, specification)
        offers = self.offers[specification]
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
            return { product: sum([offer.item.amount for offer in self.offers[product]]) }
        elif issubclass(product, self.type):  # product is a class, return all specifications of that class
            return dict([(specification, sum([offer.item.amount for offer in offers])) for specification, offers in self.offers.iteritems()
                    if issubclass(specification.type, product)])

    def offer(self, product, amount, price):
        if not issubclass(product.type, self.type):
            raise TradeException("Market {0} doesn't carry item {1}", self, item)
        offers = self.offers[product]
        result = Deal(amount, price)
        remaining_amount = amount

        # TODO: instead of offer, just buy, not a dutch market, use shop to think
        self.history[product].append(History(amount, price)) # TODO: on accept, also _remove

        for offer in offers:
            item = offer.item
            # TODO: find lowest prices
            if offer.asking_price<=price and item.amount>=remaining_amount:
                remaining_amount -= item.amount
                result.offers.append(offer)
        if remaining_amount > 0:
            raise PriceException("Market {0} does not carry {2} x {1} at {3}", self, product, amount, price)
        return result

    def _remove_offer(self, offer):
        self.offers[offer.item.specification].remove(offer)

class GoodsMarket(_Market):
    def __init__(self):
        super(GoodsMarket,self).__init__()
        self.type = Good

class JobsMarket(_Market):
    pass

class ServiceMarket(_Market):
    pass



