class Item(object):
    def __init__(self):
        self.name = 'thing'

    def weight(self):
        return 0

    def hands(self):
        return 0

    def __str__(self):
        return self.name


class Armor(Item):
    def __init__(self, rating=1):
        self.rating = rating
        self.enchantment = 0
        self.name = ['Shirt', 'Gambeson', 'Chainmail', 'Brigandine', 'Cuirass', 'Plate'][self.rating]

    def defense(self):
        return self.rating + self.enchantment

    def weight(self):
        return self.rating


class Weapon(Item):
    def __init__(self, heavy=False):
        self.heavy = heavy
        self.ability = 'physical'
        self.name = 'Claymore' if heavy else 'Sword'
        self.enchantment = 0

    def range(self):
        return 0

    def hands(self):
        return 1+self.heavy

    def weight(self):
        return self.heavy

    def bonus(self):
        if self.heavy:
            return [4, self.enchantment]
        else:
            return [self.enchantment]


class RangedWeapon(Weapon):
    def __init(self, heavy=False):
        super(self).__init(heavy)
        self.name = 'Crossbow' if self.heay else 'Bow'

    def hands(self):
        # NB no one handed ranged weapons like sling, because no trade off for shield, anyway need to load
        return 2

    def range(self):
        # TODO: not sure what the unit is. Units of theater of the mind here.
        if self.heavy:
            return 3
        else:
            return 2


class Shield(Item):
    def __init__(self):
        self.name = 'Shield'

    def hands(self):
        return -1

    def weight(self):
        return 1

