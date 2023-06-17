from language import camel_to_words
from ability import Physical, Mental, Social
from items import Shield
from effect import Effect
from errors import TargetError

class Action(object):
    cost = dict()
    ability = None
    offensive = None

    def __init__(self, actor):
        self.actor = actor

    @classmethod
    def name(cls):
        return camel_to_words(cls.__name__)

    @classmethod
    def verb(cls):
        return cls.name()+"s"

    @classmethod
    def help(cls):
        return cls.__doc__

class Result(object):
    def __init__(self):
        pass

class StrResult(object):    # TODO: perhaps an effect result where the description is the description of the effect
    def __init__(self, description):
        self.description = description

    def __str__(self):
        return self.description


class Cover(Action):
    cost = dict(ap=1)
    ability = Mental
    offensive = False

    @classmethod
    def verb(cls):
        return "seeks cover"

    def __init__(self, actor):
        super(Cover, self).__init__(actor)

    def __call__(self, *args, **kwargs):
        cover, shield = 1, None
        if isinstance(self.actor.off_hand(), Shield):
            shield = self.actor.off_hand()
        self.actor.affect(Effect('cover', cover, dict(defense=dict(cover=1))), True)
        if shield:
            return StrResult(f"seeks cover behind their {shield}")
        else:
            return StrResult(f"seeks cover nearby")

class RollResult(Result):
    def __init__(self, roll, points="health", fail="miss", negative=True):
        self.roll = roll
        self.vs = dict()
        self.points = points
        self.fail = fail
        self.negative = negative
        self.applied = False

    def versus(self, target, roll=None):
        self.vs[target] = roll
        return self

    def delta(self, target=None):
        if target is None:
            return sum(self.delta(target) for target in self.vs)
        elif target in self.vs:
            if self.negative:
                if roll := self.vs[target]:
                    return -max(0, self.roll.total - roll.total)
                else:
                    return -self.roll.total
            else:
                vs_roll = max(0, roll.total if (roll := self.vs[target]) else 0)
                return self.roll.total - vs_roll
        else:
            return None

    def apply(self, *args):   # TODO apply to mp hp. Know which one, also for _effect
        for target in list(args or self.vs):
            target[self.points] += self.delta(target)
        self.applied = True

    def hit(self, target=None):
        if target is None:
            return sum(self.hit(target) for target in self.vs if target is not None)
        else:
            return self.delta(target) or 0

    def _effect(self, target):
        if delta := self.delta(target):
            if self.applied:
                status = target[self.points]
            else:
                status = target[self.points] + delta
            if delta <= 0:
                return f"{-delta} {self.points} damage [{status}]"
            else:
                return f"{delta} {self.points} recovered [{status}]"
        else:
            return self.fail

    def __str__(self):
        if not self.vs:
            return str(self.roll)
        elif len(self.vs) == 1:
            target, roll = next(iter(self.vs.items()))
            return f"{self.roll} VS {target} {roll if roll else ''}: {self._effect(target)}"
        else:
            return "\n  ".join([f"{self.roll} VS"]+[f"{target} {roll if roll else ''}: {self._effect(target)}" for target, roll in self.vs.items()])

class Attack(Action):
    cost = dict(ap=1)
    ability = Physical
    offensive = True

    def __init__(self, actor):
        super(Attack, self).__init__(actor)

    def __call__(self, *targets, **kwargs):
        if (attack_dice := kwargs.get('attack_dice')) is None:
            attack_dice = self.actor.attack_dice()

        attack_roll = attack_dice.roll()
        if targets:
            if len(targets) > 1:
                raise TargetError("You can only attack one target at a time.")
            target = targets[0]
            defense_roll = target.defense_dice().roll()
            result = RollResult(attack_roll).versus(target, defense_roll)
            result.apply()
            return result
        else:
            return RollResult(attack_roll)

