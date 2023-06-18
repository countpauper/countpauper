from language import list_off

class Result(object):
    def __init__(self):
        pass

    def details(self):
        return dict()

class StrResult(Result):    # TODO: perhaps an effect result where the description is the description of the effect
    def __init__(self, description):
        self.description = description

    def __str__(self):
        return self.description

class EffectResult(Result):
    def __init__(self, effect):
        self.effect = effect
        self.targets = list()

    def add(self, *targets):
        self.targets += targets
        return self

    def __str__(self):
        if self.targets:
            return f"{list_off(self.targets)} {'is' if len(self.targets)==1 else 'are'} {self.effect}"
        else:
            return f"no one is {self.effect}"

    def apply(self):
        for target in self.targets:
            target.affect(self.effect, True)
        return self

class RollResult(Result):
    def __init__(self, roll, points="health", fail="miss", negative=True):
        self.roll = roll
        self.vs = dict()
        self.points = points
        self.fail = fail
        self.negative = negative
        self.applied = False

    def versus(self, target, roll=None):
        self.vs[target] = roll
        return self

    def delta(self, target=None):
        if target is None:
            return sum(self.delta(target) for target in self.vs)
        elif target in self.vs:
            if self.negative:
                if roll := self.vs[target]:
                    return -max(0, self.roll.total - roll.total)
                else:
                    return -self.roll.total
            else:
                vs_roll = max(0, roll.total if (roll := self.vs[target]) else 0)
                return self.roll.total - vs_roll
        else:
            return None

    def apply(self, *args):   # TODO apply to mp hp. Know which one, also for _effect
        for target in list(args or self.vs):
            target[self.points] += self.delta(target)
        self.applied = True

    def hit(self, target=None):
        if target is None:
            return sum(self.hit(target) for target in self.vs if target is not None)
        else:
            return self.delta(target) or 0

    def _effect(self, target):
        if delta := self.delta(target):
            if self.applied:
                status = target[self.points]
            else:
                status = target[self.points] + delta
            if delta <= 0:
                return f"{-delta} {self.points} damage [{status}]"
            else:
                return f"{delta} {self.points} recovered [{status}]"
        else:
            return self.fail

    def __str__(self):
        if not self.vs:
            return str(self.roll)
        elif len(self.vs) == 1:
            target, roll = next(iter(self.vs.items()))
            return f"{self.roll} VS {target} {roll if roll else ''}: {self._effect(target)}"
        else:
            return "\n  ".join([f"{self.roll} VS"]+[f"{target} {roll if roll else ''}: {self._effect(target)}" for target, roll in self.vs.items()])

class SummonResult(Result):
    def __init__(self, summoner):
        self.summoner = summoner
        self.summons = []

    def summon(self, *characters):
        self.summons += characters

    def apply(self):
        self.summoner.summon(*self.summons)

    def __str__(self):
        if self.summons:
            return list_off(self.summons)
        else:
            return f"no one"

    def details(self):
        return {str(summon):repr(summon) for summon in self.summons}
