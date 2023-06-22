from language import camel_to_words
from ability import Physical, Mental, Social
from items import Shield
from effect import Effect
from errors import TargetError
from result import *

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
        if hasattr(cls, '_verb'):
            return cls._verb
        else:
            return cls.name()+"s"

    @classmethod
    def help(cls):
        return cls.__doc__

class Cover(Action):
    cost = dict(ap=1)
    ability = Mental
    offensive = False
    _verb = "seeks cover"

    def __init__(self, actor):
        super(Cover, self).__init__(actor)

    def __call__(self, *args, **kwargs):
        cover, shield = 1, None
        if isinstance(self.actor.off_hand(), Shield):
            shield = self.actor.off_hand()
        if shield:
            desc = f"protected by their {shield}"
        else:
            desc = f"covered"
        result = EffectResult(Effect("covered", boni=dict(defense=dict(cover=cover)), description=desc))
        result.target(self.actor)
        result.apply()
        return result

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

