from skill import Skill
from result import *
from items import MeleeWeapon
from effect import Effect
from character import Character
from ability import Physical, Mental, Social
from errors import GameError, TargetError
from language import indefinite_article, possessive, list_off


class CrossCut(Skill):
    """Attack with two weapons simultaneously. Their attack dice are added."""

    cost = dict(ap=1, pp=1)
    ability = Physical
    offensive = True
    _verb = "cuts across"

    def __init__(self, actor):
        super(CrossCut, self).__init__(actor)

    def __call__(self, *args, **kwargs):
        args = list(args)
        if isinstance(self.actor.main_hand(), MeleeWeapon) and isinstance(self.actor.off_hand(), MeleeWeapon):
            attack_dice = self.actor.attack_dice(0) + self.actor.attack_dice(1)
            result = RollResult(attack_dice.roll())
            if args:
                target = args.pop(0)
                defense_roll = target.defense_dice().roll()
                result.versus(target, defense_roll)
            result.apply()
            return result
        else:   # TODO: specific exception types for skill preconditions
            raise GameError("You must be wielding two melee weapons.")


class Parry(Skill):
    """Protect yourself against melee attacks with your weapons. """

    cost = dict(ap=1)
    ability = Physical
    offensive = False
    _verb = "parries"

    def __init__(self, actor):
        super(Parry, self).__init__(actor)

    def __call__(self, *args, **kwargs):
        weapon = self.actor.main_hand()
        if isinstance(weapon, MeleeWeapon):
            result = EffectResult(Effect('parrying', boni = dict(defense=dict(parry=1)), description=f"parrying with {indefinite_article(weapon)} {weapon}"))
            result.target(self.actor)
            result.apply()
            return result
        else:
            raise GameError("You must be wielding a melee weapon")


class Riposte(Skill):
    """Prepare to hit with a counter attack, at full strength, after a missed attac."""

    cost = dict(ap=1)
    ability = Physical
    offensive = True

    def __init__(self, actor):
        super(Riposte, self).__init__(actor)


class Backstab(Skill):
    """Attack with a 1d6 bonus against a character who is not engaged or covered."""

    cost = dict(ap=1)
    ability = Physical
    offensive = True

    def __init__(self, actor):
        super(Backstab, self).__init__(actor)


class Flank(Skill):
    """Attack with a bonus against an enemy that is engaged by someone else."""
    cost = dict(ap=1)
    ability = Physical
    offensive = True

    def __init__(self, actor):
        super(Flank, self).__init__(actor)


class Disarm(Skill):
    """Ready a reaction to make someone drop their weapon after they miss you."""

    cost = dict(ap=1)
    ability = Physical
    offensive = True

    def __init__(self, actor):
        super(Disarm, self).__init__(actor)


class Explosion(Skill):
    """Hit everyone in the same location with an elemental attack."""

    cost = dict(ap=1, pp=3)
    ability = Mental
    offensive = True

    def __init__(self, actor):
        super(Explosion, self).__init__(actor)

    def __call__(self, *args, **kwargs):
        result = RollResult(self.actor.ability_dice(Mental).roll(), fail="evades")
        if targets:=args:
            for target in targets:
                result.versus(target, target.ability_dice(Physical).roll())
        result.apply()
        return result


class Frighten(Skill):
    """Scare someone into losing morale points. They can overcome with their mental strength."""

    cost = dict(ap=1, pp=1)
    ability = Social
    offensive = True

    def __init__(self, actor):
        super(Frighten, self).__init__(actor)

    def __call__(self, *args, **kwargs):
        result = RollResult(self.actor.ability_dice(Social).roll(), "morale")
        if args:
            target = args[0]    # TODO: more args is more pp?
            result.versus(target, target.ability_dice(Mental).roll())
        result.apply()
        return result


class Heal(Skill):
    """Heal someone, restoring their health by your mental ability dice."""

    cost = dict(ap=1, pp=1)
    ability = Mental
    offensive = False

    def __init__(self, actor):
        super(Heal, self).__init__(actor)

    def __call__(self, *args, **kwargs):
        result = RollResult(self.actor.ability_dice(Mental).roll(), fail="superfluous", negative=False)
        if args:    # TODO: more args is ? spread the heal? more pp?
            target = args[0]
            result.versus(target)
        result.apply()
        return result


# TODO: if Familiar gets this for free, free action and at range, why would a player take it? To upcast?
# for now it's not in all skills
class Encourage(Skill):
    """Encourage your ally, to give them a bonus to their next dice roll."""

    cost = dict(ap=1, mp=1)
    ability = Social
    offensive = False

    def __init__(self, actor):
        super(Encourage, self).__init__(actor)

    def __call__(self, *args, **kwargs):
        targets = args
        if not targets:
            raise TargetError("You need to encourage at least one ally.")
        if self.actor in targets:
            raise TargetError("You cannot encourage yourself.")
        result = EffectResult(Effect("encouraged", boni=dict(roll=dict(encouraged=1))))
        result.target(*targets)
        result.apply()
        return result


class Familiar(Skill):
    """Call your familiar to your side.

    Your familiar can have any form, although your GM may limit the capabilties its form implies.
    It has 0 physical, 0 mental and 3 social ability. This means it can not attack or speak or carry
    anything with a weight.
    Your familiar also has only one hitpoint,  will stay with you until the next time you rest.
    The familiar has one ability: Encourage"""

    cost = dict(ap=1, mp=1)
    ability = Social
    offensive = False
    _verb = "summons a familiar"

    def __init__(self, actor):
        super(Familiar, self).__init__(actor)


    def __call__(self, *args, **kwargs):
        familiar_name = f"{possessive(self.actor)} familiar"
        if self.actor.ally(familiar_name):
            raise GameError(f"{self.actor} already has a familiar.")
        else:
            result = SummonResult(self.actor, duration='rest')
            result.summon(Character(name=familiar_name, level=-3, physical=0, mental=0, social=3, skill=[Encourage]))
            result.apply()
            return result

class Harmony(Skill):
    """By instilling harmony in all your nearby allies, they work together as one. This will increase their defense by 1"""
    cost = dict(ap=1, mp=1)
    ability = Social
    offensive = False

    def __init__(self, actor):
        super(Harmony, self).__init__(actor)

    def __call__(self, *args, **kwargs):
        targets = args
        if len(targets) > self.actor.social:
            raise TargetError("You can only harmonize as many allies as your social score.")
        result = EffectResult(Effect("harmonized", boni=dict(defense=dict(harmony=1))))
        result.target(*targets)
        result.apply()
        return result


Skill.all = [CrossCut, Parry, Riposte, Backstab, Flank, Disarm, Explosion, Frighten, Heal, Familiar, Encourage]

