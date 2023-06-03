from items import *
from dice import Dice
from stats import *
from effect import Effect
from errors import GameError

class Character(object):
    id = Identifier("id")
    name = Identifier("name")
    color = Property("color")
    portrait = Property('portrait')
    level = Stat("level", minimum=-3)
    physical = Ability("physical")
    mental = Ability("mental")
    social = Ability("social")
    hp = CounterStat('hp')
    sp = CounterStat('sp')
    mp = CounterStat('mp')
    ap = CounterStat('ap')
    defense = Stat("defense")
    default_ap = 3
    default_hp = 4

    def __init__(self, **kwargs):
        self.effects = list()
        self.worn = list()
        self.stats = dict(id=None,
                          name='Nemo',
                          color=None,
                          portrait=None,
                          level=1,
                          physical=1,
                          mental=1,
                          social=1,
                          defense=0
        )
        self.stats = {stat:kwargs.get(stat, value) for stat, value in self.stats.items()}
        self.stats['hp'] = Counter(self.max_hp())
        self.hp = kwargs.get('hp', self.hp)
        self.stats['sp'] = Counter(self.max_sp())
        self.sp = kwargs.get('sp', self.sp)
        self.stats['mp'] = Counter(self.max_mp())
        self.mp = kwargs.get('mp', self.mp)
        self.stats['ap'] = Counter(self.default_ap)
        self.ap = kwargs.get('ap', self.ap)

        self.skill = kwargs.get('skills', [])
        self.inventory = kwargs.get('inventory',[])
        # TODO: find a way to put equipped status in a record, but still allow duplicate items and duplicate location
        # list of tuples? or dict with locations, then list of items there.
        self.held = dict()
        if self.inventory:
            self.auto_equip()

    def __getitem__(self, key):
        return self.stats[key]

    def __setitem__(self, key, value):
        raise NotImplementedError("Not allowed (yet)")

    def __delitem__(self, key):
        raise NotImplementedError("Not allowed (yet)")

    def max_hp(self):
        return self.default_hp + self.level + sum(self.get_boni('hp').values())

    def max_sp(self):
        return self.mental + sum(self.get_boni('sp').values())

    def max_mp(self):
        return self.social + sum(self.get_boni('mp').values())

    def max_ap(self):
        return self.default_ap + sum(self.get_boni('ap').values())

    def alive(self):
        return bool(self.hp) and bool(self.level)

    def moralized(self):
        return bool(self.mp)

    def active(self):
        return self.alive() and self.moralized()

    def get_max(self, stat):
        if stat=='hp':
            return self.max_hp()
        elif stat=='sp':
            return self.max_sp()
        elif stat=='mp':
            return self.max_mp()
        elif stat=='ap':
            return self.max_ap()
        else:
            raise ValueError(f"Unknown counter {stat}")

    def get_boni(self, stat):
        boni = dict()
        for e in self.effects:
            boni.update(e.get_boni(stat))
        for w in self.worn:
            boni.update(w.get_boni(stat))
        boni={k:v for k,v in boni.items() if v is not None}
        return boni

    def defense_dice(self):
        return Dice.for_ability(self.physical)+Dice(bonus=self.get_boni('defense').values())

    def turn(self):
        self['ap'].reset()
        self.effects=[e for e in self.effects if e.turn()]

    def attack(self, enemy=None, number=0, bonus=None):
        self._act()

        attack_roll = self.attack_dice(number, bonus).roll()
        if enemy is not None:
            defense_roll = enemy.defense_dice().roll()
            damage = attack_roll.total - defense_roll.total
            if damage < 0:
                return f"{attack_roll} VS {defense_roll} misses {enemy.name}"
            else:
                enemy.damage(damage)
                return f"{attack_roll} VS {defense_roll} hits {enemy.name} ({enemy['hp']})[-{damage}]"
        else:
            return f"{attack_roll}"

    def cover(self):
        if self.find_effect('cover'):
            raise GameError("You are already covered.")
        self._act()
        cover, shield = 1, None
        if isinstance(self.off_hand(), Shield):
            shield = self.off_hand()
        self.effects.append(Effect('cover', cover, dict(defense=dict(cover=1))))
        return shield

    def find_effect(self, name):
        return [e for e in self.effects if e.name == name]

    def damage(self, dmg):
        hp = self.hp - int(dmg)
        self.hp = hp

    def _act(self, ap=1):
        if not self.ap:
            raise GameError("Not enough action points")
        self.ap -= 1

    def attack_dice(self, nr=0, bonus=None):
        result = Dice.for_ability(self.physical)
        if nr == 1:   # check for dual wielding
            if weapon:=self.off_hand():
                result += weapon.bonus()
            elif nr:
                result += nr*-2
        else:
            if weapon:=self.main_hand():
                result += weapon.bonus()
            if nr:
                result += nr*-2
        if bonus is not None:
            result+=bonus
        return result

    def bodyweight(self):
        return self.size.weight

    def weight(self):
        return self.bodweight() + self.carried()

    def carried(self):
        return sum(i.weight() for i in self.inventory)

    def capacity(self):
        return self.physical

    def obtain(self, *items):
        items = [ItemFactory(item) for item in items]
        item_weight = sum(i.weight() for i in items)
        if self.carried()+item_weight>self.capacity():
            raise GameError(f"{', '.join(str(i) for i in items)} is too heavy to carry.")   # TODO: can be overloaded
        self.inventory += items
        return items

    def _find_item(self, item):
        if isinstance(item, Item):
            if item in self.inventory:
                return item
            else:
                return None
        if isinstance(item,str):
            if matches := [i for i in self.inventory if i.name.lower()==item.lower()]:
                return matches[-1]
            else:
                return None
        elif isinstance(item, type):
            if matches := [i for i in self.inventory if isinstance(i, item)]:
                return matches[-1]
            else:
                return None
        else:
            raise TypeError(f"Can't find items by {type(item)}")

    def lose(self, *items):
        def _error(argument):
            raise GameError(argument)
        items = [self._find_item(item) or _error(f"No item {item} in the inventory") for item in items]
        for item in items:
            self.inventory.remove(item)
            if self.main_hand() == item:
                self.held['main'] = None
            if self.off_hand() == item:
                self.held['off'] = None
            if item in self.worn:
                self.worn.remove(item)
        return items

    def main_hand(self):
        return self.held.get('main')

    def off_hand(self):
        return self.held.get('off')

    def item(self, name):
        return [i for i in self.inventory if i.name==name]

    def auto_equip(self):
        self.held=dict()
        self.worn=[]
        for i in self.inventory:
            if not self.main_hand() and i.hands() >= 1:
                self.held['main'] = i
            elif not self.off_hand() and \
                (not self.main_hand() or self.main_hand().hands() < 2) and \
                abs(i.hands())==1:
                self.held['off'] = i
            elif not self.worn and isinstance(i, Armor):
                self.worn = [i]
        return self

    def __str__(self):
        return f"""{self.name}: Level {self.level}
    {self.physical} Physical: {self['hp']} HP
    {self.mental} Mental: {self['sp']} SP
    {self.social} Social: {self['mp']} MP
    Defense {self.defense_dice()} Attack {self.attack_dice()}
    Inventory[{self.carried()}/{self.capacity()}] {" ".join(str(i) for i in self.inventory) or "Empty"} 
    {" ".join(str(s) for s in self.skill)}"""



