from items import *
from effect import Effect
from errors import GameError
from language import camel_to_words


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
    """Attack with two weapons simultaneously."""
    def __init__(self):
        self.cost = dict(ap=1, sp=1)

    def __call__(self, *args, **kwargs):
        actor = args[0]
        target = args[1]
        if type(actor.main()) == MeleeWeapon and type(actor.off())==MeleeWeapon:
            # TODO: should be current subsquence attack penalty and increase by one, tripple crosscuts is possible but at minus a lot
            first_attack = actor.attack(target, 0)
            second_attack = actor.attack(target, 1)
            return f"{first_attack} and {second_attack}."
        else:   # TODO: specific exception types for skill execution
            raise GameError("You must be wielding two melee weapons")

class Parry(Skill):
    """Protect yourself against melee attacks with your weapons."""
    def __init__(self):
        self.cost = dict(ap=1)

    def __call__(self, *args, **kwargs):
        actor = args[0]
        if type(actor.main()) == MeleeWeapon:
            self.affect(Effect('parry', 1, dict(defense=dict(cover=1)), True))
            return f"{actor.name.capitalize()} parries."
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

