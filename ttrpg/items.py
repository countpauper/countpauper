from dice import Dice
import random

class Item(object):
    def __init__(self):
        self.name = 'thing'

    def weight(self):
        return 0

    def hands(self):
        return 0

    def properties(self):
        return None

    def __str__(self):
        return self.name


class Armor(Item):
    def __init__(self, **props):
        self.rating = props.get('rating',1)
        self.enchantment = props.get('enchantment', 0)
        self.name = props.get('name', ['shirt', 'gambeson', 'chainmail', 'brigandine', 'cuirass', 'plate'][self.rating])

    def defense(self):
        return self.rating + self.enchantment

    def weight(self):
        return self.rating

    def properties(self):
        return dict(name=self.name,
                    rating=self.rating,
                    enchantment=self.enchantment)

class Weapon(Item):
    def __init__(self, **props):
        self.heavy = props.get('heavy', False)
        self.ability = props.get('ability','physical')
        self.name = props.get('name',random.choice(['claymore', 'katana', 'spear', 'quarterstaff']) if self.heavy else \
            random.choice(['sword', 'battle axe', 'mace', 'dagger']))
        self.enchantment = props.get('enchantment',0)

    def properties(self):
        return dict(name=self.name,
                    heavy=self.heavy,
                    ability=self.ability,
                    enchantment=self.enchantment)

    def reach(self):
        return 0

    def hands(self):
        return 1+self.heavy

    def weight(self):
        return 1+self.heavy

    def bonus(self):
        d=Dice(4) if self.heavy else Dice()
        if self.enchantment:
            d += self.enchantment
        return d


class RangedWeapon(Weapon):
    def __init__(self, **props):
        heavy = props.get('heavy', False)
        props['name'] = props.get('name', random.choice(['crossbow','longbow', 'arquebus']) if heavy \
            else random.choice(['bow', 'sling']))
        super(RangedWeapon, self).__init__(**props)

    def hands(self):
        # NB no one handed ranged weapons like sling, because no trade off for shield, anyway need to load
        return 2

    def reach(self):
        # TODO: not sure what the unit is. Units of theater of the mind here.
        if self.heavy:
            return 3
        else:
            return 2


class Shield(Item):
    def __init__(self):
        self.name = 'shield'

    def hands(self):
        return -1

    def weight(self):
        return 1


class Equipment(Item):
    def __init__(self, name, **props):
        self.name=name
        self.heavy=props.get('heavy', False)
        self.usable = props.get('use', False)

    def properties(self):
        return dict(name=self.name,
                    heavy=self.heavy)

    def hands(self):
        return self.usable # TODO: for torches and so on

    def weight(self):
        return self.heavy
