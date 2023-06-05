from items import *
from dice import Dice
from stats import *
from skills import Skill
from effect import Effect
from errors import GameError
from language import plural, list_off

class Attack(object):
    def __init__(self, attack, defense=None):
        self.attack=attack
        self.defense = defense

    def damage(self):
        if self.defense:
            if self.attack.total>self.defense.total:
                return self.attack.total - self.defense.total
            else:
                return 0
        else:
            return None

    def hit(self):
        return self.damage()

    def __str__(self):
        if self.defense:
            if dmg := self.damage():
                return f"{self.attack} VS {self.defense} hits for {dmg} damage"
            else:
                return f"{self.attack} VS {self.defense} misses"
        else:
            return f"{self.attack}"

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
    pp = CounterStat('pp')
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
        self.user = kwargs.get('user')
        self.stats = {stat:kwargs.get(stat, value) for stat, value in self.stats.items()}
        self.stats['hp'] = Counter(self.max_hp())
        self.hp = kwargs.get('hp', self.hp)
        self.stats['pp'] = Counter(self.max_pp())
        self.pp = kwargs.get('pp', self.pp)
        self.stats['mp'] = Counter(self.max_mp())
        self.mp = kwargs.get('mp', self.mp)
        self.stats['ap'] = Counter(self.default_ap)
        self.ap = kwargs.get('ap', self.ap)

        self.skill = [Skill.create(s) for s in kwargs.get('skill', [])]
        self.inventory = [ItemFactory(i) for i in kwargs.get('inventory',[])]
        # TODO: find a way to put equipped status in a record, but still allow duplicate items and duplicate location
        # list of tuples? or dict with locations, then list of items there.
        self.held = dict()
        if self.inventory:
            self.auto_equip()

    def Name(self):
        return self.name.capitalize()

    def __getitem__(self, key):
        return self.stats[key]

    def __setitem__(self, key, value):
        raise NotImplementedError("Not allowed (yet)")

    def __delitem__(self, key):
        raise NotImplementedError("Not allowed (yet)")

    def max_hp(self):
        return self.default_hp + self.level + sum(self.get_boni('hp').values())

    def max_pp(self):
        return self.mental + sum(self.get_boni('pp').values())

    def max_mp(self):
        return self.social + sum(self.get_boni('mp').values())

    def max_ap(self):
        return self.default_ap + sum(self.get_boni('ap').values())

    def alive(self):
        return bool(self.hp) and bool(self.level)

    def motivated(self):
        return bool(self.mp)

    def powerful(self):
        return bool(self.pp)

    def active(self):
        return self.alive() and self.motivated()

    def get_max(self, stat):
        if stat=='hp':
            return self.max_hp()
        elif stat=='pp':
            return self.max_pp()
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

    def physical_dice(self):
        return Dice.for_ability(self.physical)

    def defense_dice(self):
        return self.physical_dice()+Dice(bonus=self.get_boni('defense').values())

    def turn(self):
        self['ap'].reset()
        self.effects=[e for e in self.effects if e.turn()]

    def attack_dice(self, nr=0):
        result = Dice.for_ability(self.physical)
        # TODO: add personal attack bonus (or penalty for subsequent attacks)
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
        return result

    def mental_dice(self):
        # TODO: refactor getting different attack dice with the right boni
        return  Dice.for_ability(self.mental)

    def attack(self, target=None, attack_dice=None):
        self._act()
        if attack_dice is None:
            attack_dice = self.attack_dice()

        attack_roll = attack_dice.roll()
        if target is not None:
            defense_roll = target.defense_dice().roll()
            result = Attack(attack_roll, defense_roll)
            target.damage(result.damage())
            return result
        else:
            return Attack(attack=attack_roll)

    def cover(self):
        self._check_cost(dict(ap=1))    # NB : check beforehand to make it transactional
        cover, shield = 1, None
        if isinstance(self.off_hand(), Shield):
            shield = self.off_hand()
        self.affect(Effect('cover', cover, dict(defense=dict(cover=1))), True)
        self._act()
        return shield

    def _find_skill(self, skill):
        if type(skill) == str:
            if match := [s for s in self.skill if str(s) == skill]:
                return match[0]
            else:
                return None
        elif isinstance(skill, type) and issubclass(skill, Skill):
            if match := [s for s in self.skill if isinstance(s, skill)]:
                return match[0]
            else:
                return None
        else:
            raise TypeError(f"No skill can be found based on {type(skill)}.")

    def execute(self, skill, *targets):
        if s := self._find_skill(skill):
            self._check_cost(s.cost)
            response = s(self, *targets)
            self._cost(s.cost)
            return response
        else:
            raise GameError(f"{skill} is not known by {self.Name()}.")

    def affect(self, effect, unique=False):
        if unique and (duplicates := self.affected(effect.name)):
            raise GameError(f"You are already affected by {duplicates[0]}.")

        # TODO: check for duplicates/conflicts
        # TODO create Effect from args? what if there are Effect types?
        self.effects.append(effect)

    def affected(self, name):
        return [e for e in self.effects if e.name == name]

    def damage(self, dmg):
        if dmg:
            hp = self.hp - int(dmg)
            self.hp = hp

    def _check_cost(self, cost):
        errors = []
        if (ap:=cost.get('ap')) and self.ap < ap:
            errors.append(f"you need {ap} {plural(ap, 'action')}, but you have {self['ap']}")
        if (pp:=cost.get('pp')) and self.pp < pp:
            errors.append(f"you need {pp} power, but you have {self['pp']}")
        if (mp:=cost.get('mp')) and self.mp < mp:
            errors.append(f"you need {mp} morale, but you have {self['mp']}")
        # TODO: allow this? is going to 0 then allowed?
        if (hp:=cost.get('hp')) and self.hp < hp:
            errors.append(f"you need {hp} health, but you have {self['hp']}")
        if errors:
            raise GameError(list_off(errors).capitalize()+".")
        return cost

    def _cost(self, cost):
        if ap:=cost.get('ap'):
            self.ap -= ap
        if pp:=cost.get('pp'):
            self.pp -= pp
        if mp:=cost.get('mp'):
            self.mp -= mp
        if hp:=cost.get('hp'):
            self.hp -= hp

    def _act(self, ap=1):
        cost = dict(ap=ap)
        self._check_cost(cost)
        self._cost(cost)


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
            raise GameError(f"{list_off(items)} is too heavy to carry.")   # TODO: can be overloaded
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
        items = [self._find_item(item) or _error(f"No {item} found in the inventory of {self.name}.") for item in items]
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
    {self.mental} Mental: {self['pp']} PP
    {self.social} Social: {self['mp']} MP
    Defense {self.defense_dice()} Attack {self.attack_dice()}
    Inventory[{self.carried()}/{self.capacity()}]: {list_off(self.inventory) or "Empty"} 
    Skills: {list_off(self.skill)}"""



