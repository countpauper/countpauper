import sys
from item import Good

class _MarketException(Exception):
    pass

class ItemCarriedException(_MarketException):
    pass

class _Market(object):
    def __init__(self):
        self.offers = {}
 
    def sell(self, item, asking_price):
        if not isinstance(item, self.type):
            raise ItemCarriedException("Market {0} doesn't carry item {1}", self, item)
        self.offers[item] = asking_price

    def offer(self, product, amount, price):
        for item, asking_price in self.offers.iteritems():
            # TODO: gather multiple offers if not enough amount, find lowest price
            if item.specification == product and asking_price<=price:
                return { item: amount }

    def buy(self, offer):
        # remove sold out items
        for item, amount in offer.iteritems():
            if item.amount >= amount:
                self.offers.pop(item)
        
    def cancel(self, offer):
        pass    # whatever, no reservation as long as offer is within a tick

class GoodsMarket(_Market):
    def __init__(self):
        super(self,GoodsMarket).__init__()
        self.type = Good

class JobsMarket(_Market):
    pass

class ServiceMarket(_Market):
    pass



