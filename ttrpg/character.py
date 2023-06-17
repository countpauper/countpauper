from items import *
from dice import Dice
from stats import *
from skill import Skill
from ability import Physical, Mental, Social
from effect import Effect
from errors import GameError
from language import plural, list_off
from actions import *


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
        self.stats['hp'] = Counter(kwargs.get('hp'), lambda: self.max_hp(), lambda: sum(self.get_boni('hp').values()))
        self.stats['pp'] = Counter(kwargs.get('pp'), lambda: self.max_pp(), lambda: sum(self.get_boni('pp').values()))
        self.stats['mp'] = Counter(kwargs.get('mp'), lambda: self.max_mp(), lambda: sum(self.get_boni('mp').values()))
        self.stats['ap'] = Counter(kwargs.get('ap'), lambda: self.default_ap, lambda: sum(self.get_boni('ap').values()))

        self.skill = [Skill.create(self, s) for s in kwargs.get('skill', [])]
        self.inventory = [ItemFactory(i) for i in kwargs.get('inventory',[])]
        self.allies = kwargs.get('allies', [])
        self.held = dict()
        if self.inventory:
            self.auto_equip()

    def __getitem__(self, key):
        return self.stats[stat_aliases.get(key, key)]

    def __setitem__(self, key, value):
        key = stat_aliases.get(key, key)
        if (stat := self.stats.get(key)) is None:
            raise KeyError(f"Unknown statistic {key}")
        if isinstance(stat, Counter):
            if isinstance(value, Counter):
                self.stats[key] = value
            else:
                stat.set(value)
        else:
            self.stats[key] = max(0, value) # TODO assume minimum is 0 (for abilities) but it should be more configurable, also maximum


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
        if stat[0] == 'h':
            return self.max_hp()
        elif stat[0] == 'p':
            return self.max_pp()
        elif stat[0] == 'm':
            return self.max_mp()
        elif stat[0] == 'a':
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
        return self.ability_dice(Physical)+self.get_boni('defense')

    def turn(self):
        self['ap'].reset()
        self.effects=[e for e in self.effects if e.turn()]

    def attack_dice(self, nr=0):
        result = self.ability_dice(Physical)
        # TODO: add personal attack bonus (or penalty for subsequent attacks)
        if nr == 1:   # check for dual wielding
            if weapon := self.off_hand():
                result += weapon.bonus()
            elif nr:
                result += nr*-2
        else:
            if weapon := self.main_hand():
                result += weapon.bonus()
            if nr:
                result += nr*-2
        return result

    def ability(self, ability):
        if ability == Physical or ability == Physical.name:
            return self.physical
        if ability == Mental or ability == Mental.name:
            return self.mental
        if ability == Social or ability == Social.name:
            return self.social
        raise ValueError(f"Unknown ability {ability}")


    def ability_dice(self, ability):
        return Dice.for_ability(self.ability(ability)) + self.get_boni("roll")

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

    def execute(self, action, *targets, **kwargs):
        if isinstance(action, Action):
            pass
        elif isinstance(action, type):
            action = action(self)
        elif skill := self._find_skill(action):
            action = skill
        else:
            raise GameError(f"{action} is not known by {self}.")

        self._check_cost(action.cost)  # NB : check beforehand to make it transactional
        if not (ability_score:=self.ability(action.ability)):
            raise GameError(f"Your {action.ability.name} is too low ({ability_score}) to {action}.")
        response = action(*targets, **kwargs)
        response.action = action
        self._cost(action.cost)
        return response

    def affect(self, effect, unique=False):
        if unique and (duplicates := self.affected(effect.name)):
            raise GameError(f"You are already affected by {duplicates[0]}.")
        # TODO create Effect from args? what if there are Effect types?
        self.effects.append(effect)

    def affected(self, name):
        return [e for e in self.effects if e.name == name]

    def _detect_cost_errors(self, cost):
        errors = []
        if (ap:=cost.get('ap')) and self.ap < ap:
            errors.append(f"you need {ap} {plural(ap, 'action')}, but you have {self.ap}")
        if (pp:=cost.get('pp')) and self.pp < pp:
            errors.append(f"you need {pp} power, but you have {self.pp}")
        if (mp:=cost.get('mp')) and self.mp < mp:
            errors.append(f"you need {mp} morale, but you have {self.mp}")
        # TODO: allow this? is going to 0 then allowed?
        if (hp:=cost.get('hp')) and self.hp < hp:
            errors.append(f"you need {hp} health, but you have {self.hp}")
        return errors

    @staticmethod
    def _report_cost_errors(errors):
        if errors:
            raise GameError(list_off(errors).capitalize()+".")

    def _check_cost(self, cost):
        if approval:=self._detect_cost_errors(cost):    # NB : check beforehand to make it transactional
            self._report_cost_errors(approval)

    def approve_cost(self, cost):
        return len(self._detect_cost_errors(cost)) == 0

    def _cost(self, cost):
        if ap:=cost.get('ap'):
            self.ap -= ap
        if pp:=cost.get('pp'):
            self.pp -= pp
        if mp:=cost.get('mp'):
            self.mp -= mp
        if hp:=cost.get('hp'):
            self.hp -= hp

    def carried(self):
        """Weight carried."""
        return sum(i.weight() for i in self.inventory)

    def capacity(self):
        """Maximum weight that can be carried without encumberance."""
        return self.physical

    def memory(self):
        """Maximum number of skilsl known."""
        return self.level

    def level_up(self, ability):
        # TODO: apply as a level bonus effect so it can be removed on level drain, also the bonus hp and the max is just ... 4?
        self.level += 1
        self.hp += 1
        if ability == Physical or ability == Physical.name:
            self.physical += 1
        elif ability == Mental or ability == Mental.name:
            self.mental += 1
        elif ability == Social or ability == Social.name:
            self.social += 1
        else:
            raise GameError(f"Unknown ability {ability} for leveling up.")
        return self.level

    def learn(self, *new_skills):
        if len(self.skill) + len(new_skills) > self.memory():
            raise GameError(f"{self} already knows {len(self.skill)}/{self.memory()} skills.")
        new_skills = [Skill.create(self, skill) for skill in new_skills]
        if known_skills:=[skill for skill in new_skills if any(isinstance(known_skill, type(skill)) for known_skill in self.skill)]:
            raise GameError(f"{self} already knows {list_off(known_skills)}.")
        self.skill += new_skills
        return tuple(new_skills)

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


    def ally(self, name):
        if match := [ally for ally in self.allies if ally.name.lower()==name.lower()]:
            return match[0]
        else:
            return None

    def summon(self, ally):
        self.allies.append(ally)

    def __str__(self):
        return self.name.capitalize()

    def __repr__(self):
        return f"""{self.name}: Level {self.level}
{self.physical} Physical: {self.hp} HP
{self.mental} Mental: {self.pp} PP
{self.social} Social: {self.mp} MP
Defense {self.defense_dice()} Attack {self.attack_dice()}
Inventory [{self.carried()}/{self.capacity()}]: {list_off(self.inventory) or "Empty"} 
Skills [{len(self.skill)}/{self.memory()}]: {list_off(self.skill) or "none"}
Allies: {list_off(self.allies) or "none"}"""



