from language import camel_to_words

class Skill(object):
    all = []

    def __init__(self):
        self.cost = dict(ap=1)  # could be hp,pp,mp,ap or even gp

    @classmethod
    def name(cls):
        return camel_to_words(cls.__name__)

    @classmethod
    def help(cls):
        return cls.__doc__

    @staticmethod
    def create(skill):
        if type(skill) == str:
            if match := [s for s in Skill.all if s.name() == skill]:
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
        return type(self).name()
