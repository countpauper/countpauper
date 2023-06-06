from items import *
from effect import Effect
from ability import Physical, Mental, Social
from errors import GameError
from language import camel_to_words, indefinite_article, list_off


class Skill(object):
    all = []

    def __init__(self):
        self.cost = dict(ap=1)  # could be hp,pp,mp,ap or even gp

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
        self.cost = dict(ap=1, pp=1)
        self.ability = Physical
        self.offensive = True

    def __call__(self, *args, **kwargs):
        args = list(args)
        actor = args.pop(0)
        if isinstance(actor.main_hand(), MeleeWeapon) and isinstance(actor.off_hand(), MeleeWeapon):
            attack_dice = actor.attack_dice(0) + actor.attack_dice(1)
            if args:
                target = args.pop(0)
                result = actor.attack(target, attack_dice)
                return f"crosscuts {target.Name()}: {result} [{target.hp}]"
            else:
                return f"crosscuts {attack_dice.roll()}"
        else:   # TODO: specific exception types for skill preconditions
            raise GameError("You must be wielding two melee weapons.")


class Parry(Skill):
    """Protect yourself against melee attacks with your weapons. """
    def __init__(self):
        self.cost = dict(ap=1)
        self.ability = Physical
        self.offensive = False

    def __call__(self, *args, **kwargs):
        actor = args[0]
        weapon = actor.main_hand()
        if isinstance(weapon, MeleeWeapon):
            actor.affect(Effect('parry', 1, dict(defense=dict(parry=1))), True)
            return f"parries with {indefinite_article(weapon)} {weapon}"
        else:
            raise GameError("You must be wielding a melee weapon")


class Riposte(Skill):
    """Prepare to hit with a counter attack, at full strength, after a missed attac."""
    def __init__(self):
        self.cost = dict(ap=1)
        self.ability = Physical
        self.offensive = True

class Backstab(Skill):
    """Attack with a 1d6 bonus against a character who is not engaged or covered."""
    def __init__(self):
        self.cost = dict(ap=1)
        self.ability = Physical
        self.offensive = True

class Flank(Skill):
    """Attack with a bonus against an enemy that is engaged by someone else."""
    def __init__(self):
        self.cost = dict(ap=1)
        self.ability = Physical
        self.offensive = True

class Disarm(Skill):
    """Ready a reaction to make someone drop their weapon after they miss you."""
    def __init__(self):
        self.cost = dict(ap=1)
        self.ability = Physical
        self.offensive = True

class Explosion(Skill):
    """Hit everyone in the same location with an elemental attack."""
    def __init__(self):
        self.cost = dict(ap=1, pp=3)
        self.ability = Mental
        self.offensive = True

    def __call__(self, *args, **kwargs):
        args = list(args)
        actor = args.pop(0)
        damage_roll = actor.ability_dice(Mental).roll()
        target_result=[]
        if targets:=args:
            for target in targets:
                save_roll = target.ability_dice(Physical).roll()
                if (damage := damage_roll.total-save_roll.total) > 0:
                    target.damage(damage)
                    target_result.append(f"{target.Name()} [{target.hp}] hit ({save_roll}) for {damage} damage")
                else:
                    target_result.append(f"{target.Name()} evades ({save_roll})")
        return "\n  ".join([f"explodes ({damage_roll})"] + target_result)


class Frighten(Skill):
    """Scare someone into losing morale points. They can overcome with their mental strength."""
    def __init__(self):
        self.cost = dict(ap=1, pp=1)
        self.ability = Social
        self.offensive = True

    def __call__(self, *args, **kwargs):
        args = list(args)
        actor = args.pop(0)
        damage_roll = actor.ability_dice(Social).roll()
        if args:
            target = args.pop(0)
            save_roll = target.ability_dice(Mental).roll()
            if (damage := damage_roll.total-save_roll.total) > 0:
                target.mp -= damage
                return f"frightens {target.Name()}: {damage_roll} VS {save_roll} for {damage} morale [{target.mp}]"
            else:
                return f"attempts to frighten {target.Name()}: {damage_roll} VS {save_roll}"
        else:
            return f"frightens ({damage_roll.roll()})"

class Heal(Skill):
    """Heal someone, restoring their health by your mental ability dice."""
    def __init__(self):
        self.cost = dict(ap=1, pp=1)
        self.ability = Mental
        self.offensive = False

    def __call__(self, *args, **kwargs):
        args = list(args)
        actor = args.pop(0)
        heal_roll = actor.ability_dice(Mental).roll()
        if args:
            target = args.pop(0)
            target.hp += heal_roll.total
            return f"heals {target.Name()}: for {heal_roll} health [{target.hp}]"
        else:
            return f"heals ({heal_roll.roll()})"

Skill.all = [CrossCut, Parry, Riposte, Backstab, Flank, Disarm, Explosion, Frighten, Heal]

