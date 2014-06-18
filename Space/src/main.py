import time
import hardcoded_data
import unittest

class Main(object):
    def __init__(self):
        self.space = hardcoded_data.space

    def loop(self):
        while True:
            # TODO: instead of ticking all, on creation all active objects should add themselves to a scheduler 
            # the scheduler should eventually execute as rate monotic
            start = time.clock()
            self.space.tick(1)
            duration = time.clock()-start
            if duration<1:
                time.sleep(0.1-duration)
        
if __name__== '__main__':
    exit(Main().loop())

