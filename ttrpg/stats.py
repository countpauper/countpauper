class Property(object):
    def __init__(self, name):
        self.name =  name

    def __get__(self, instance, owner):
        return instance.stats.get(self.name)

    def __set__(self, instance, value):
        instance.stats[self.name] = value

    def __delete__(self, instance):
        instance.stats.pop(self.name)

class Stat(Property):
    def __init__(self, name):
        super(Stat,self).__init__(name)

    def __get__(self, instance, owner):
        boni = instance.get_boni(self.name)
        return max(0, instance.stats.get(self.name) + sum(boni.values()))

    def __set__(self, instance, value):
        instance.stats[self.name] = value

    def __delete__(self, instance):
        instance.stats.pop(self.name)

class Identifier(Property):
    def __init__(self, name):
        super(Identifier, self).__init__(name)

class Ability(Stat):
    def __init__(self, name):
        super(Ability,self).__init__(name)

class Counter(object):
    def __init__(self, max):
        self.max = max
        self.value = self.max

    def __add__(self, increase):
        result = Counter(self.max).value = self.value + increase
        result.value = self.value + increase
        return result

    def __sub__(self, decrease):
        result = Counter(self.max)
        result.value = self.value - decrease
        return result

    def __str__(self):
        return f'{self.value}/{self.max}'

    def __conform__(self, protocol):
        return self.value

    def __bool__(self):
        return self.value > 0

    def __eq__(self, other):
        if type(other) == Counter:
            return self.value == other.value and self.max == other.max
        else:
            return self.value == other

class CounterStat(Stat):
    def __init__(self, name):
        super(CounterStat, self).__init__(name)


    def __get__(self, instance, owner):
        stat = instance.stats.get(self.name)
        boni = instance.get_boni(self.name)
        return min(max(0, stat.value + sum(boni.values())), instance.get_max(self.name))


    def __set__(self, instance, value):
        stat = instance.stats.get(self.name)
        stat.value = value

