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
    def help(cls):
        return cls.__doc__

class Cover(Action):
    cost = dict(ap=1)
    ability = Mental
    offensive = False

    def __init__(self, actor):
        super(Cover, self).__init__(actor)

    def __call__(self, *args, **kwargs):
        cover, shield = 1, None
        if isinstance(self.actor.off_hand(), Shield):
            shield = self.actor.off_hand()
        self.actor.affect(Effect('cover', cover, dict(defense=dict(cover=1))), True)
        if shield:
            return f"seeks cover behind their {shield}"
        else:
            return f"seeks cover nearby"

class AttackResult(object):
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
            result = AttackResult(attack_roll, defense_roll)
            target.damage(result.damage())
            return result
        else:
            return AttackResult(attack=attack_roll)

