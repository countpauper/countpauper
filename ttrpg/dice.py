import d20
from d20 import roll, SimpleStringifier

ability_dice = [
    [],
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


class Stringifier(SimpleStringifier):
    def _stringify(self, node):
        if not node.kept:
             return ''
        return super()._stringify(node)

    def _str_expression(self, node):
        if len(node.children) == 1 and isinstance(node.roll, d20.Literal):
            return f"{int(node.total)}"
        else:
            return f"{self._stringify(node.roll)} = {int(node.total)}"

class Dice(object):
    def __init__(self, *dice, bonus=[]):
        self.dice = list(d for d in dice if abs(d) > 1)
        self.flat = list(bonus) + list(d for d in dice if abs(d) <= 1)

    fixed = None
    @staticmethod
    def fix(amount):
        amount, Dice.fixed = Dice.fixed, amount
        return amount

    def __add__(self, other):
        if other is None:
            return self.copy()
        if isinstance(other, Dice):
            return Dice(*self.dice, *other.dice, bonus=self.flat + other.flat)
        elif isinstance(other, dict):
            result = self
            for v in other.values():
                result += v
            return result
        else:
            return Dice(*self.dice, bonus=self.flat+[other])

    def __sub__(self, other):
        if other is None:
            return self.copy()
        if isinstance(other, Dice):
            return Dice(*self.dice, *[-d for d in other.dice], bonus=self.flat+[-b for b in other.flat])
        else:
            return Dice(*self.dice, bonus=self.flat+[-other])

    def copy(self):
        return Dice(*self.dice, bonus=self.flat)

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

    def __neg__(self):
        return Dice(*[-d for d in self.dice], bonus=[-b for b in self.flat])

    def roll(self):
        if Dice.fixed is None:
            return roll(str(self), stringifier=Stringifier())
        else:
            value = Dice.fixed
            if value == 'min':
                return roll(str(self.minimum()), stringifier=Stringifier())
            elif value == 'max':
                return roll(str(self.maximum()), stringifier=Stringifier())
            return roll(f"{value}", stringifier=Stringifier())


    def minimum(self):
        return Dice(bonus=[1 for _ in self.dice]+self.flat)

    def maximum(self):
        return Dice(bonus=[d for d in self.dice]+self.flat)

    def __eq__(self,other):
        if isinstance(other, Dice):
            return self.dice == other.dice and self.flat == other.flat
        else:
            return False

    @staticmethod
    def for_ability(score):
        return Dice(*ability_dice[score])
