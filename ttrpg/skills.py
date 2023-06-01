from items import *

class Skill(object):
    def __init__(self):
        self.cost = dict(ap=1)  # could be hp,sp,mp,ap or even gp

    def name(self):
        return type(self).__name__ # TODO: split camel case and lower case it

class CrossCut(Skill):
    """Attack with two weapons simultaneously."""
    def __init__(self):
        self.cost = dict(ap=1, sp=1)

    def __call__(self, *args, **kwargs):
        actor=args[0]
        target=args[1]
        if type(actor.main())==Weapon and type(actor.off())==Weapon:
            first_attack = actor.attack(target, 0)
            second_attack = actor.attack(target, 1)
            return f"{first_attack} and {second_attack}."
        else:   # TODO: specific exception types for skill execution
            raise Exception("Precondition: You must be wielding two weapons")

class Parry(Skill):
    """Cover yourself against melee attacks with your weapons."""
    pass

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