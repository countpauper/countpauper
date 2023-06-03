from items import *
from effect import Effect
from errors import GameError
from language import camel_to_words, indefinite_article


class Skill(object):
    all = []

    def __init__(self):
        self.cost = dict(ap=1)  # could be hp,sp,mp,ap or even gp

    @staticmethod
    def name(cls):
        return camel_to_words(cls.__name__)

    @staticmethod
    def create(name):
        if match := [s for s in Skill.all if Skill.name(s) == name]:
            return match[0]
        else:
            return None


class CrossCut(Skill):
    """Attack with two weapons simultaneously. Their attack dice are added."""
    def __init__(self):
        self.cost = dict(ap=2, sp=1)

    def __call__(self, *args, **kwargs):
        args = list(args)
        actor = args.pop(0)
        if isinstance(actor.main_hand(), MeleeWeapon) and isinstance(actor.off_hand(), MeleeWeapon):
            attack_dice = actor.attack_dice(0) + actor.attack_dice(1)
            if args:
                target = args.pop(0)
                result = actor.attack(target, attack_dice)
                return f"crosscuts {target.Name()}: {result}."
            else:
                return f"crosscuts {attack_dice.roll()}"
        else:   # TODO: specific exception types for skill preconditions
            raise GameError("You must be wielding two melee weapons.")

class Parry(Skill):
    """Protect yourself against melee attacks with your weapons. """
    def __init__(self):
        self.cost = dict(ap=1)

    def __call__(self, *args, **kwargs):
        actor = args[0]
        weapon = actor.main_hand()
        if isinstance(weapon, MeleeWeapon):
            actor.affect(Effect('parry', 1, dict(defense=dict(parry=1))), True)
            return f"parries with {indefinite_article(weapon)} {weapon}"
        else:
            raise GameError("You must be wielding a melee weapon")


class Riposte(Skill):
    """After a missed attack while parrying, react with a counter attack."""
    pass

class Backstab(Skill):
    """Roll an extra 1d6 damage against a character who can't see you."""
    pass

class Flank(Skill):
    """Attack with a bonus against an enemy that is engaged by someone else."""
    pass

class Disarm(Skill):
    """Ready a reaction to make someone drop their weapon after they miss you."""
    pass

Skill.all = [CrossCut, Parry, Riposte, Backstab, Flank, Disarm]

