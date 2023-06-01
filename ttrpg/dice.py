from d20 import roll

ability_dice = [
    [1],
    [2],
    [4],
    [6],
    [8],
    [10],
    [6, 4],
    [6, 6],
    [8, 6],
    [8, 8],
    [10, 8],
    [10, 10],
    [12, 10],
    [12, 12],
    [8, 8, 8],
    [10, 8, 8],
    [10, 10, 8],
    [10, 10, 10],
    [12, 10, 10],
    [12, 12, 10],
    [12, 12, 12]]


class Dice(object):
    def __init__(self, *dice, bonus=[]):
        self.dice = list(d for d in dice if abs(d) > 1)
        self.flat = list(bonus) + list(d for d in dice if abs(d) <= 1)

    def __add__(self, other):
        if type(other) == Dice:
            return Dice(*self.dice, *other.dice, bonus=self.flat + other.flat)
        else:
            return Dice(*self.dice, bonus=self.flat+[other])

    def __sub__(self, other):
        if type(other) == Dice:
            return Dice(*self.dice, *[-d for d in other.dice], bonus=self.flat+[-b for b in other.flat])
        else:
            return Dice(*self.dice, bonus=self.flat+[-other])

    def __str__(self):
        dice = self.dice
        parts = []
        while dice:
            die = dice[0]
            if die <= 1:
                parts.append(str(dice.count(die) * die))
            else:
                parts.append(f'{dice.count(die)}d{die}')
            dice = [d for d in dice if d is not die]
        parts += [str(b) for b in self.flat]
        if parts:
            return '+'.join(parts).replace('+-', '-')
        else:
            return '0'

    def roll(self):
        return roll(str(self))

    def minimum(self):
        return len([d for d in self.dice])+sum(self.flat)

    def maximum(self):
        return sum(d for d in self.dice)+sum(self.flat)

    def __eq__(self,other):
        if isinstance(other, Dice):
            return self.dice == other.dice and self.flat == other.flat
        else:
            return False

    @staticmethod
    def for_ability(score):
        return Dice(*ability_dice[score])
