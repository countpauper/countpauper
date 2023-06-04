from items import *
from effect import Effect
from errors import GameError
from language import camel_to_words, indefinite_article, list_off


class Skill(object):
    all = []

    def __init__(self):
        self.cost = dict(ap=1)  # could be hp,sp,mp,ap or even gp

    @staticmethod
    def name(cls):
        return camel_to_words(cls.__name__)

    @staticmethod
    def create(skill):
        if type(skill)==str:
            if match := [s for s in Skill.all if Skill.name(s) == skill]:
                return Skill.create(match[0])
            else:
                raise ValueError(f"Unknown skill {skill}")
        elif isinstance(skill, type) and issubclass(skill, Skill):
            return skill()
        elif isinstance(skill, Skill):
            return skill
        else:
            raise TypeError(f"Cannot create skill by {type(skill)}.")

    def __str__(self):
        return self.name(self.__class__)

class CrossCut(Skill):
    """Attack with two weapons simultaneously. Their attack dice are added."""
    def __init__(self):
        self.cost = dict(ap=1, sp=1)

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

class Explosion(Skill):
    """Hit everyone in the same location with an elemental attack."""
    def __init__(self):
        self.cost = dict(ap=1, sp=3)

    def __call__(self, *args, **kwargs):
        args = list(args)
        actor = args.pop(0)
        damage_roll = actor.mental_dice().roll()
        target_result=[]
        if targets:=args:
            for target in targets:
                save_roll = target.physical_dice().roll()
                if (damage :=damage_roll.total-save_roll.total) > 0:
                    target.damage(damage)
                    target_result.append(f"{target.Name()}[{target['hp']}] hit ({save_roll}) for {damage} damage")
                else:
                    target_result.append(f"{target.Name()} evades ({save_roll})")
        return f"explodes {damage_roll} {list_off(target_result)}"

Skill.all = [CrossCut, Parry, Riposte, Backstab, Flank, Disarm, Explosion]

