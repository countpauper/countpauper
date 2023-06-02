from discord.ext.commands import CommandError

class GameError(CommandError):
    def __init__(self, *args):
        super(GameError, self).__init__(*args)

class CharacterUnknownError(CommandError):
    def __init__(self, guild, owner, name=None):
        if name is None:
            super(CharacterUnknownError, self).__init__(f"No character found for {owner} on {guild}.")
        elif owner is None:
            super(CharacterUnknownError, self).__init__(f"No character {name}' found on {guild}.")
        else:
            super(CharacterUnknownError, self).__init__(f"No character '{name}' found for {owner} on {guild}.")

