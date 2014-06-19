import time
import hardcoded_data
import unittest

class Main(object):
    def __init__(self):
        self.space = hardcoded_data.space
        self.frequency = 2
        speed_up = 16
        self.ticks_per_rl_second = 64*8 * speed_up

    def loop(self):
        while True:
            # TODO: instead of ticking all, on creation all active objects should add themselves to a scheduler 
            # the scheduler should eventually execute as rate monotic
            start = time.clock()
            self.space.tick(self.ticks_per_rl_second/self.frequency)
            duration = time.clock()-start
            if duration<1:
                time.sleep((1.0/self.frequency)-duration)
        
if __name__== '__main__':
    exit(Main().loop())

