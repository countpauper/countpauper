from d20 import roll


class Dice(object):
    def __init__(self, *dice):
        self.dice = list(dice)

    def __add__(self, other):
        if type(other) == Dice:
            return Dice(*self.dice, *other.dice)
        else:
            return Dice(*self.dice, other)

    def __str__(self):
        dice = self.dice
        while 0 in dice:
            dice.remove(0)
        if not dice:
            return '0'
        parts = []
        while dice:
            die = dice[0]
            if die <= 1:
                parts.append(str(dice.count(die) * die))
            else:
                parts.append(f'{dice.count(die)}d{die}')
            dice = [d for d in dice if d is not die]
        return '+'.join(parts).replace('+-', '-')

    def roll(self):
        return roll(str(self))

    def minimum(self):
        return len([d for d in self.dice if d>1])+sum(d for d in self.dice if d<=1)

    def maximum(self):
        return sum(d for d in self.dice)

    def __eq__(self,other):
        if isinstance(other, Dice):
            return self.dice == other.dice
        else:
            return False

    @staticmethod
    def for_ability(score):
        dice = [
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
        return Dice(*dice[score-1])
