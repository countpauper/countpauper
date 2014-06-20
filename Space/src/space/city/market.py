import sys

class _Market(object):
    def __init__(self):
        self.offers = {}

    def sell(self, item, price):
        self.offers[item] = price

    def buy(self, product):
        lowest_price = sys.intmax
        result = None
        for item, price in self.offers.iteritems():
            if item.specification == product and price < lowest_price:
                result = item
                lowest_price = price
        return result, price

class GoodsMarket(_Market):
    pass


