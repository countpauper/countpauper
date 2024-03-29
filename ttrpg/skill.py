from actions import Action

class Skill(Action):
    all = []

    def __init__(self, actor):
        super(Skill, self).__init__(actor)

    @staticmethod
    def create(actor, skill):
        if type(skill) == str:
            if match := [s for s in Skill.all if s.name() == skill]:
                return Skill.create(actor, match[0])
            else:
                raise ValueError(f"Unknown skill {skill}")
        elif isinstance(skill, type) and issubclass(skill, Skill):
            return skill(actor)
        elif isinstance(skill, Skill):
            assert skill.actor == actor # TODO: copy?
            return skill
        else:
            raise TypeError(f"Cannot create skill by {type(skill)}.")

    def __str__(self):
        return type(self).name()
