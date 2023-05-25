from ability import abilities, dice
from sizes import sizes
import random
from items import *
from d20 import roll

class Counter(object):

    def __init__(self, a, b=None):
        if b is None:
            self.max = self.value = a
        else:
            self.value = a
            self.max = b

    def __str__(self):
        return f'{self.value}/{self.max}'

    def __add__(self, value):
        return Counter(self.value + value, self.max)

    def __sub__(self, value):
        return Counter(self.value - value, self.max)

    def __bool__(self):
        return self.value>0

def dice_expr(dice):
    while 0 in dice:
        dice.remove(0)
    if not dice:
        return '0'
    parts = []
    while dice:
        die = dice[0]
        if die <= 1:
            parts.append(str(dice.count(die)*die))
        else:
            parts.append(f'{dice.count(die)}d{die}')
        dice = [d for d in dice if d is not die]
    return '+'.join(parts).replace('+-','-')


class Character(object):

    def __init__(self, **kwargs):
        self.name = kwargs.get('name','Nemo')
        self.level = kwargs.get('level', 1)
        self.size = kwargs.get('size', sizes['m'])
        if type(self.size) == str:
            self.size = sizes.get(self.size)
        self.physical = kwargs.get('physical',self.size['physical'])
        self.mental = kwargs.get('mental',2)
        self.social = kwargs.get('social',2)
        self.hp = Counter(self.max_hp())
        self.sp = Counter(self.max_sp())
        self.mp = Counter(self.max_mp())
        self.skills = []
        self.inventory = []
        self.held = dict(main=None, off=None) # TODO: less hands for paws, more hands for weird creatures
        self.worn = None

    def max_hp(self):
        return self.level + self.physical

    def max_sp(self):
        return self.mental

    def max_mp(self):
        return self.social

    def natural_armor(self):
        return self.size['physical']

    def defense(self):
        if not self.worn:
            return self.natural_armor()
        else:
            return self.natural_armor() + self.worn.defense()

    def attack(self, enemy, bonus=0):
        attack_roll = roll(dice_expr(self.attack_dice()+[bonus]))
        damage = attack_roll.total - enemy.defense()
        if damage < 0:
            return f"{self.name} misses {enemy.name} ({attack_roll})"
        else:
            enemy.damage(damage)
            return f'{self.name} attacks ({attack_roll}) {enemy.name} ({enemy.hp})[-{damage}]'

    def damage(self, dmg):
        self.hp -= dmg

    def attack_dice(self):
        if not (weapon:=self.held['main']):
            return [4]
        else:
            return dice[self.physical] + weapon.bonus()

    def bodyweight(self):
        return self.size.weight

    def weight(self):
        return self.bodweight() + self.carried()

    def carried(self):
        return sum(i.weight() for i in self.inventory)

    def capacity(self):
        return self.physical

    def __str__(self):
        return f"""{self.name}: Level {self.level}
    Defense {self.defense()}. Attack {dice_expr(self.attack_dice())}
    {self.physical} Physical: {self.hp} HP
    {self.mental} Mental: {self.sp} SP
    {self.social} Social: MP {self.mp} MP
    [{" ".join(str(i) for i in self.inventory)}] {self.carried()}/{self.capacity()}"""

    def alive(self):
        return bool(self.hp)

    def moralized(self):
        return bool(self.mp)

    def active(self):
        return self.alive() and self.moralized()

    @staticmethod
    def random_pc(level=1):
        assert level == 1 # not yet implemented
        size = random.randint(1, 3)
        if size == 1:
            return Character(name='Pixie', size='xs', social=3, mental=3)
        elif size == 2:
            r = random.randint(0, 1)
            return Character(name='Gnome', size='s', social=2+r, mental=2+(1-r))
        elif size == 3:
            return Character(name='Human', size='m')
        else:
             return Character(name='Default')

    @staticmethod
    def random_monster(level):
        total_ability = 6+level
        assert total_ability>=3 # minium ability. Lowest level would be -3
        remaining_ability = total_ability-2 # reserve two for 1 each social and mental
        size = random.choice([s for s in sizes.values() if s['physical']<remaining_ability])
        remaining_ability = total_ability - size['physical']
        social_bonus = random.randint(0, remaining_ability)
        mental_bonus = remaining_ability - social_bonus
        return Character(name=f'{size["name"]} Monster', size=size, social=1+social_bonus, mental=1+mental_bonus)

    def obtain(self, items):
        self.inventory+=items

    def auto_equip(self):
        for i in self.inventory:
            if not self.held['main'] and i.hands() >= 1:
                self.held['main'] = i
                # TODO: find best main weapon

        if not self.held['main'] or self.held['main'].hands()<2:
            for i in self.inventory:
                if (not self.held['off'] and
                    abs(i.hands())==1):
                    self.held['off'] = i
                    # TODO: find best off hand item

        if not self.worn:
            for i in self.inventory:
                if type(i) == Armor:
                    self.worn = i
                    break

    def random_equipment(self):
        capacity = self.physical - self.size['physical']
        if capacity < 2 :
            weapon = random.choice([Weapon(), RangedWeapon()])
        else:
            weapon = random.choice([Weapon(), Weapon(True), RangedWeapon(), RangedWeapon(True)])
        capacity -= weapon.weight()
        if weapon.hands() == 1:
            offhand = random.choice([Weapon(), Shield() if capacity else None, None])
            capacity -= offhand.weight() if offhand else 0
        else:
            offhand = None
        if capacity and (armor_rating := random.randint(0, min(capacity, 3))):
            armor = Armor(armor_rating)
            capacity -= armor.weight()
        else:
            armor = None
        return [i for i in (weapon, offhand, armor) if i]


