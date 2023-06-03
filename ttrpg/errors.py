from discord.ext.commands import CommandError
from language import plural

class GameError(CommandError):
    def __init__(self, *args):
        super(GameError, self).__init__(*args)

class CharacterUnknownError(CommandError):
    def __init__(self, guild, owner, *names):
        names = [str(n).capitalize() for n in names if n is not None]
        if not names:
            super(CharacterUnknownError, self).__init__(f"No character found for {owner} on {guild}.")
        elif owner is None:
            super(CharacterUnknownError, self).__init__(f"No {plural(len(names), 'character')} named {', '.join(names)} found on {guild}.")
        else:
            super(CharacterUnknownError, self).__init__(f"No {plural(len(names), 'character')} named {', '.join(names)} found for {owner} on {guild}.")

