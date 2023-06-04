from dice import Dice
import random

armor_names={0:'shirt', 1:'gambeson', 2:'chainmail', 3:'brigandine', 4:'cuirass', 5:'plate'}
heavy_melee_weapon_names=['claymore', 'katana', 'spear', 'quarterstaff']
light_melee_weapon_names=['sword', 'battle axe', 'mace', 'dagger']
heavy_ranged_weapon_names=['crossbow','longbow', 'arquebus']
light_ranged_weapon_names=['bow', 'sling']


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

    def get_boni(self, stat):
        return dict()


class Armor(Item):
    def __init__(self, **props):
        self.rating = props.get('rating', 1)
        self.enchantment = props.get('enchantment', None)
        self.name = props.get('name', armor_names[self.rating])

    def weight(self):
        return self.rating

    def properties(self):
        return dict(name=self.name,
                    rating=self.rating,
                    enchantment=self.enchantment)

    def get_boni(self, stat):
        boni = super(Armor, self).get_boni(stat)
        if stat == 'defense':
            boni['rating'] = self.rating
            if self.enchantment:
                boni['enchantment'] = self.enchantment
        return boni


class Weapon(Item):
    def __init__(self, **props):
        self.heavy = props.get('heavy', False)
        self.ability = props.get('ability','physical')
        self.enchantment = props.get('enchantment',0)

    def properties(self):
        return dict(name=self.name,
                    heavy=self.heavy,
                    ability=self.ability,
                    enchantment=self.enchantment)

    def weight(self):
        return 1+self.heavy

    def get_boni(self, stat):
        boni = super(Weapon, self).get_boni(stat)
        if stat == 'attack':
            if self.heavy:
                boni['heavy'] = Dice(4)
            if self.enchantment:
                boni['enchantment'] = self.enchantment
        return boni

    def bonus(self):
        result = Dice()
        for bonus in self.get_boni("attack").values():
            result += bonus
        return result


class MeleeWeapon(Weapon):
    def __init__(self, **props):
        super(MeleeWeapon, self).__init__(**props)
        self.name = props.get('name',random.choice(heavy_melee_weapon_names) if self.heavy else \
            random.choice(light_melee_weapon_names))

    def reach(self):
        return 0

    def hands(self):
        return 1+self.heavy


class RangedWeapon(Weapon):
    def __init__(self, **props):
        super(RangedWeapon, self).__init__(**props)
        self.name = props.get('name', random.choice(heavy_ranged_weapon_names) if self.heavy \
            else random.choice(light_ranged_weapon_names))

    def hands(self):
        # NB no one handed ranged weapons like sling, because no trade off for shield, anyway need to load
        return 2

    def reach(self):
        if self.heavy:
            return 3
        else:
            return 2


class Shield(Item):
    def __init__(self, **props):
        self.name = props.get('name', 'shield')

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


def ItemFactory(item, props=None):
    if props is None:
        props = dict()
    if isinstance(item, Item):
        return item
    elif isinstance(item, type) and issubclass(item, Item):
        return item(**props)
    elif isinstance(item, str):
        props['name'] = props.get('name', item.lower())
        item = item.lower()
        if item == "shield":
            return Shield(**props)
        if item in light_melee_weapon_names or item == 'meleeweapon':
            return MeleeWeapon(**props)
        elif item in heavy_melee_weapon_names:
            return MeleeWeapon(heavy=True, **props)
        elif item in light_ranged_weapon_names or item == 'rangedweapon':
            return RangedWeapon(**props)
        elif item in heavy_ranged_weapon_names:
            return RangedWeapon(heavy=True, **props)
        elif item == 'equipment':
            return Equipment(**props)
        elif item == 'armor':
            return Armor(**props)
        else:
            try:
                armor_rating = list(armor_names.values()).index(item)
                return Armor(name=item, rating=armor_rating)
            except ValueError:
                return Equipment(name=item)
    else:
        raise TypeError(f"Can not create an item by {type(item)}")
