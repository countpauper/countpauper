from market import Market
from space.city.agent import Agent

class JobMarket(Market):
    def __init__(self):
        super(JobMarket,self).__init__()
        self.type = Agent

