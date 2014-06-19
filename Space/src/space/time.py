_seconds_per_tick = 8
_ticks_per_minute = 8
_ticks_per_hour = 64 * _ticks_per_minute
_ticks_per_day = 32 * _ticks_per_hour
_ticks_per_cycle = 8 * _ticks_per_day
_ticks_per_year = 32 * _ticks_per_cycle

class Time(object):
    def __init__(self, tick=0, minute=0, hour=0,day=0,cycle=0,year=0):
        self._tick = int(tick + 
                         minute * _ticks_per_minute +
                         hour * _ticks_per_hour +
                         day * _ticks_per_day +
                         cycle * _ticks_per_cycle +
                         year * _ticks_per_year)
                         
        if (self._tick<0):
            raise ValueError("Negative time")
 
    def __add__(self, period):
        return Time(self._tick + period._ticks)

    def __sub__(self, other):
        if isinstance(other, Period):
            if period._ticsk>self._ticks:
                raise ValueError("Negative time")
            return Time(self._tick - other._ticks)
        elif isinstance(other, Time):
            return Period(self.tick - other._tick)
        else:
            raise TypeError("Can only subtract Time or Period from Time")

    def __lt__(self, other):
        return self._tick < other._tick

    def __le__(self, other):
        return self._tick <= other._tick

    def __eq__(self, other):
        return self._tick == other._tick

    def __repr__(self):
        year = self._tick // _ticks_per_year
        cycle = (self._tick % _ticks_per_year) // _ticks_per_cycle
        day = (self._tick % _ticks_per_cycle) // _ticks_per_day
        hour = (self._tick % _ticks_per_day) // _ticks_per_hour
        minute = (self._tick % _ticks_per_hour ) // _ticks_per_minute
        second = _seconds_per_tick * (self._tick % _ticks_per_minute)
        return "{:04d}-{:02d}-{:d} {:02d}:{:02d}:{:02d}".format(year,cycle,day,hour,minute,second)

class Period(object):
    def __init__(self, ticks=0):
        self._ticks = int(ticks)

    def seconds(self):
        return self._ticks * _seconds_per_tick

    def hours(self):
        return self._ticks / float(_ticks_per_hour)

    def days(self):
        return self._ticks / float(_ticks_per_day)

    def cycles(self):
        return self._ticks / float(_ticks_per_cycle)

    def __add__(self,period):
        return Period(self._ticks + period._ticks)

    def __sub__(self, period):
        return Period(self._ticks - period._ticks)     

    def __mul__(self, factor):
        return Period(self.ticks * factor)

    def __mul__(self, divisor):
        return Period(self.ticks // divisor)

    def __repr__(self):
        return str(self._ticks)

    def __lt__(self, other):
        return self._ticks < other._ticks

    def __eq__(self, other):
        return self._ticks == other._ticks

class Minute(Period):
    def __init__(self, minutes):
        super(Minute, self).__init__(minutes*_ticks_per_minute)

class Hour(Period):
    def __init__(self, hours):
        super(Hour, self).__init__(hours * _ticks_per_hour )

class Day(Period):
    def __init__(self, hours):
        super(Day, self).__init__(hours * _ticks_per_day )

class Cycle(Period):
    def __init__(self, hours):
        super(Cycle, self).__init__(hours * _ticks_per_cycle )

class Year(Period):
    def __init__(self, hours):
        super(Year, self).__init__(hours * _ticks_per_year )

