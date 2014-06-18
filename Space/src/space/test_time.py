import unittest
from space.time import Time, Period, Year, Cycle, Day, Hour, Minute

class Test_Time(unittest.TestCase):
    def test_string(self):
        t=Time(0)
        s=str(t)
        self.assertEqual(s,"0000-00-0 00:00:00")
        t=Time(hour=3,minute=2,tick=1)
        self.assertEqual(str(t),"0000-00-0 03:02:08")
        t=Time(year=1, cycle=2, day=3)
        self.assertEqual(str(t),"0001-02-3 00:00:00")

    def test_compare(self):
        t = Time(0),Time(1)
        self.assertNotEqual(t[0], t[1])
        self.assertGreaterEqual(t[1], t[0])
        self.assertLess(t[0], t[1])
        self.assertTrue(t[0]!=t[1])

class Test_Period(unittest.TestCase):
    def test_string(self):
        self.fail("Not implemented")

    def test_compare(self):
        self.assertGreater(Time(1), Time(0))
        year = Year(1)
        cycle = Cycle(1)
        day = Day(1)
        hour = Hour(1)
        minute = Minute(1)
        tick = Period(1)
        self.assertGreater(year,cycle)
        self.assertGreater(cycle,day)
        self.assertGreater(day,hour)
        self.assertGreater(hour,minute)
        self.assertGreater(minute,tick)

    def test_add(self):
        t = 1,2
        p = [Period(tick) for tick in t]
        self.assertEqual(p[0] + p[1] , Period(t[0]+t[1]))
        self.assertEqual(sum(p), Period(sum(t)))
        p01 = p[0]
        p01+= p[1]
        self.assertEqual(p01, Period(t[0]+t[1]))

    def test_sub(self):
        t = 3, 2
        p = [Period(tick) for tick in t]
        self.assertEqual(Period(t0)-Period(t1), Period(t[0]-t[1]))
        self.assertEqual(Period(t1)-Period(t0), Period(t[1]-t[0]))
        p01 = p[0]
        p01 -= p[1]
        self.assertEqual(p01, Period(t0-t1))

if __name__ == '__main__':
    unittest.main()
