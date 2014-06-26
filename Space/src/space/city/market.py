import sys
from collections import defaultdict
from space.item import Good

class _MarketException(Exception):
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
        self.offers = dict()

    def price(self):
        return self.amount * self.price

class _Market(object):
    def __init__(self):
        self.offers = defaultdict(lambda:list())
 
    def sell(self, vendor, item, asking_price):
        if not isinstance(item, self.type):
            raise ItemCarriedException("Market {0} doesn't carry item {1}", self, item)
        self.offers[item.product].append(Offer(vendor, item, asking_price))

    def offer(self, product, amount, price):
        if not issubclass(product.type, self.type):
            raise ItemCarriedException("Market {0} doesn't carry item {1}", self, item)
        offers = self.offers[product]
        result = Deal(amount, price)
        remaining_amount = amount
        for offer in offers:
            item = offer.item
            # TODO: find lowest prices
            if offer.asking_price<=price and item.amount>=remaining_amount:
                remaining_amount -= item.amount
                result.append(offer)
        if remaining_amount > 0:
            raise PriceException("Market {0} does not carry {2} x {1} at {3}", self, product, amount, price)
        return result

    def _remove_offer(self, offer):
        self.offers[offer.item.specification].remove(offer)

    def buy(self, buyer, deal):
        # pay
        buyer.money -= deal.price()
        # gather items
        remaining_amount = deal.amount
        total_item = None
        for offer in offer.iteritems():
            if offer.item.amount<=remaining_amount:
                sold_item = offer.item.split(offer.item.amount)
                self._remove_offer(offer)   # sold out
                remaining_amount -= sold_item.amount
            else:
                sold_item = offer.item.split(remaining_amount)
                remaining_amount = 0
            # pay the vendor
            offer.vendor.money += deal.price * sold_item.amount
            if not total_item:
                total_item = sold_item
            else:
                total_item.stack(sold_item)
        return total_item

    def cancel(self, deal):
        pass    # whatever, no reservation as long as offer is within a tick

class GoodsMarket(_Market):
    def __init__(self):
        super(GoodsMarket,self).__init__()
        self.type = Good

class JobsMarket(_Market):
    pass

class ServiceMarket(_Market):
    pass



