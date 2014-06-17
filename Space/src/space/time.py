_ticks_per_minute = 8
_ticks_per_hour = 64 * _ticks_per_minute
_ticks_per_day = 16 * _ticks_per_hour
_ticks_per_cycle = 8 * _ticks_per_day
_ticks_per_year = 64 * _ticks_per_cycle

class Time(object):
    def __init__(self, tick=0):
        if (tick<0):
            raise ValueError("Negative time")
        self._tick = int(tick)

    def __add__(self,period):
        return Time(self._tick + period._ticks)

    def __sub__(self, period):
        if period._ticsk>self._ticks:
            raise ValueError("Negative time")
        return Time(self._tick - period._ticks)

    def __str__(self):
        year = self._tick // _ticks_per_year
        cycle = (self._tick % _ticks_per_year) // _ticks_per_cycle
        day = (self._tick % _ticks_per_cycle) // _ticks_per_day
        hour = (self._tick % _ticks_per_day) // _ticks_per_day
        minute = (self._tick % _ticks_per_hour ) // _ticks_per_minute
        second = 8 * (self._tick % ticks_per_minute)
        return "{%04d} - {%02d} - {%d} {%02d} : {%02d} : {%02d}".format(year,cycle,day,hour,minute,second)

class Period(object):
    def __init__(self, ticks=0):
        self._ticks = int(ticks)

    def __add__(self,period):
        return Period(self._ticks + period._ticks)

    def __sub__(self, period):
        return Period(self._ticks - period._ticks)     

    def __mul__(self, factor):
        return Period(self.ticks * factor)

    def __mul__(self, divisor):
        return Period(self.ticks // divisor)

    def __str__(self):
        return self._ticks

class Minute(Period):
    def __init__(self, minutes):
        super(Minute, self).__init__(minutes*_ticks_per_minute)

class Hour(Period):
    def __init__(self, hours):
        super(Minute, self).__init__(hours * _ticks_per_hour )

class Hour(Period):
    def __init__(self, hours):
        super(Minute, self).__init__(hours * _ticks_per_hour )

class Day(Period):
    def __init__(self, hours):
        super(Minute, self).__init__(hours * _ticks_per_day )

class Cycle(Period):
    def __init__(self, hours):
        super(Minute, self).__init__(hours * _ticks_per_cycle )

class Year(Period):
    def __init__(self, hours):
        super(Minute, self).__init__(hours * _ticks_per_year )

