from market import Market
from space.item import Good
from space.city.agent import Agent

class GoodsMarket(Market):
    def __init__(self):
        super(GoodsMarket,self).__init__()
        self.type = Good