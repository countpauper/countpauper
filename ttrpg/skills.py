from skill import Skill
from items import *
from effect import Effect
from character import Character
from ability import Physical, Mental, Social
from errors import GameError, TargetError
from language import indefinite_article, possessive, list_off




class CrossCut(Skill):
    """Attack with two weapons simultaneously. Their attack dice are added."""
    def __init__(self, actor):
        super(CrossCut, self).__init__(actor)
        self.cost = dict(ap=1, pp=1)
        self.ability = Physical
        self.offensive = True

    def __call__(self, *args, **kwargs):
        args = list(args)
        if isinstance(self.actor.main_hand(), MeleeWeapon) and isinstance(self.actor.off_hand(), MeleeWeapon):
            attack_dice = self.actor.attack_dice(0) + self.actor.attack_dice(1)
            if args:
                target = args.pop(0)
                result = self.actor.attack(target, attack_dice)
                return f"crosscuts {target}: {result} [{target.hp}]"
            else:
                return f"crosscuts {attack_dice.roll()}"
        else:   # TODO: specific exception types for skill preconditions
            raise GameError("You must be wielding two melee weapons.")


class Parry(Skill):
    """Protect yourself against melee attacks with your weapons. """
    def __init__(self, actor):
        super(Parry, self).__init__(actor)
        self.cost = dict(ap=1)
        self.ability = Physical
        self.offensive = False

    def __call__(self, *args, **kwargs):
        weapon = self.actor.main_hand()
        if isinstance(weapon, MeleeWeapon):
            self.actor.affect(Effect('parry', 1, dict(defense=dict(parry=1))), True)
            return f"parries with {indefinite_article(weapon)} {weapon}"
        else:
            raise GameError("You must be wielding a melee weapon")


class Riposte(Skill):
    """Prepare to hit with a counter attack, at full strength, after a missed attac."""
    def __init__(self, actor):
        super(Riposte, self).__init__(actor)
        self.cost = dict(ap=1)
        self.ability = Physical
        self.offensive = True

class Backstab(Skill):
    """Attack with a 1d6 bonus against a character who is not engaged or covered."""
    def __init__(self, actor):
        super(Backstab, self).__init__(actor)
        self.cost = dict(ap=1)
        self.ability = Physical
        self.offensive = True

class Flank(Skill):
    """Attack with a bonus against an enemy that is engaged by someone else."""
    def __init__(self, actor):
        super(Flank, self).__init__(actor)
        self.cost = dict(ap=1)
        self.ability = Physical
        self.offensive = True

class Disarm(Skill):
    """Ready a reaction to make someone drop their weapon after they miss you."""
    def __init__(self, actor):
        super(Disarm, self).__init__(actor)
        self.cost = dict(ap=1)
        self.ability = Physical
        self.offensive = True

class Explosion(Skill):
    """Hit everyone in the same location with an elemental attack."""
    def __init__(self, actor):
        super(Explosion, self).__init__(actor)
        self.cost = dict(ap=1, pp=3)
        self.ability = Mental
        self.offensive = True

    def __call__(self, *args, **kwargs):
        damage_roll = self.actor.ability_dice(Mental).roll()
        target_result=[]
        if targets:=args:
            for target in targets:
                save_roll = target.ability_dice(Physical).roll()
                if (damage := damage_roll.total-save_roll.total) > 0:
                    target.damage(damage)
                    target_result.append(f"{target} [{target.hp}] hit ({save_roll}) for {damage} damage")
                else:
                    target_result.append(f"{target} evades ({save_roll})")
        return "\n  ".join([f"explodes ({damage_roll})"] + target_result)


class Frighten(Skill):
    """Scare someone into losing morale points. They can overcome with their mental strength."""
    def __init__(self, actor):
        super(Frighten, self).__init__(actor)
        self.cost = dict(ap=1, pp=1)
        self.ability = Social
        self.offensive = True

    def __call__(self, *args, **kwargs):
        damage_roll = self.actor.ability_dice(Social).roll()
        if args:
            target = args[0]    # TODO: more args is more pp?
            save_roll = target.ability_dice(Mental).roll()
            if (damage := damage_roll.total-save_roll.total) > 0:
                target.mp -= damage
                return f"frightens {target}: {damage_roll} VS {save_roll} for {damage} morale [{target.mp}]"
            else:
                return f"attempts to frighten {target}: {damage_roll} VS {save_roll}"
        else:
            return f"frightens ({damage_roll.roll()})"

class Heal(Skill):
    """Heal someone, restoring their health by your mental ability dice."""
    def __init__(self, actor):
        super(Heal, self).__init__(actor)
        self.cost = dict(ap=1, pp=1)
        self.ability = Mental
        self.offensive = False

    def __call__(self, *args, **kwargs):
        heal_roll = self.actor.ability_dice(Mental).roll()
        if args:    # TODO: more args is ? spread the heal? more pp?
            target = args[0]
            target.hp += heal_roll.total
            return f"heals {target}: for {heal_roll} health [{target.hp}]"
        else:
            return f"heals ({heal_roll.roll()})"

# TODO: if Familiar gets this for free, free action and at range, why would a player take it? To upcast?
# for now it's not in all skills
class Encourage(Skill):
    """Encourage your ally, to give them a bonus to their next dice roll."""
    def __init__(self, actor):
        super(Encourage, self).__init__(actor)
        self.cost = dict(ap=1, mp=1)
        self.ability = Social
        self.offensive = False

    def __call__(self, *args, **kwargs):
        targets = args
        if not targets:
            raise TargetError("You need to encourage at least one ally.")
        if self.actor in targets:
            raise TargetError("You cannot encourage yourself.")
        for target in targets:
            target.affect(Effect("encouraged", 1, dict(roll=dict(encouraged=1))))
        return f"encourages {list_off(targets)}"

class Familiar(Skill):
    """Call your familiar to your side.

    Your familiar can have any form, although your GM may limit the capabilties its form implies.
    It has 0 physical, 0 mental and 3 social ability. This means it can not attack or speak or carry
    anything with a weight.
    Your familiar also has only one hitpoint,  will stay with you until the next time you rest.
    The familiar has one ability: Encourage"""
    def __init__(self, actor):
        super(Familiar, self).__init__(actor)
        self.cost = dict(ap=1, mp=1)
        self.ability = Social
        self.offensive = False

    def __call__(self, *args, **kwargs):
        familiar_name = f"{possessive(self.actor)} familiar"
        if self.actor.ally(familiar_name):
            raise GameError(f"{self.actor} already has a familiar.")
        else:
            familiar = Character(name=familiar_name, level=-3, physical=0, mental=0, social=3, skill=[Encourage])
            self.actor.summon(familiar)
            return "summons a familiar"

class Harmony(Skill):
    """By instilling harmony in all your nearby allies, they work together as one. This will increase their defense by 1"""
    def __init__(self, actor):
        super(Harmony, self).__init__(actor)
        self.cost = dict(ap=1, mp=1)
        self.ability = Social
        self.offensive = False

    def __call__(self, *args, **kwargs):
        targets = args
        if len(targets) > self.actor.social:
            raise TargetError("You can only harmonize as many allies as your social score.")
        for target in targets:
            target.affect(Effect("harmony", 1, dict(defense=dict(harmony=1))))
        return f"harmonizes {list_off(targets)}"

Skill.all = [CrossCut, Parry, Riposte, Backstab, Flank, Disarm, Explosion, Frighten, Heal, Familiar, Encourage]

