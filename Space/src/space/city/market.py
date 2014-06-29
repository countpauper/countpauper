import sys
from collections import defaultdict
from utility.format_exception import FormatException
from space.item import Good

class _MarketException(FormatException):
    pass

class TradeException(_MarketException):
    """Market does not trade in that product"""
    pass

class PriceException(_MarketException):
    """Not enough supply for that price"""
    pass

class Offer(object):
    def __init__(self, vendor, item, asking_price):
        self.item = item
        self.asking_price = asking_price
        # self.time = time
        self.vendor = vendor
       
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

class _Market(object):
    def __init__(self):
        self.offers = defaultdict(lambda:list())
 
    def sell(self, vendor, item, asking_price):
        """List an item on the market, creating an offer record for it under it's product specification"""
        if not isinstance(item, self.type):
            raise TradeException("Market {0} doesn't carry item {1}", self, item)
        offer = Offer(vendor, item, asking_price)
        self.offers[item.specification].append(offer)
        return offer

    def shop(self, item_type):
        """ Retrieve a product: amount dictionary for all offers for which the product's type is a subtype of item_type
        eg if item_type is food, return a dict of meal products of different qualities and their quantities"""
        if not issubclass(item_type, self.type):
            raise TradeException("Market {0} doesn't carry item {1}", self, item_type)
        return dict([(product, sum(offer.item.amount for offer in offers)) for product, offers in self.offers.iteritems() if issubclass(product.type, item_type)])

    def offer(self, product, amount, price):
        if not issubclass(product.type, self.type):
            raise TradeException("Market {0} doesn't carry item {1}", self, item)
        offers = self.offers[product]
        result = Deal(amount, price)
        remaining_amount = amount
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



