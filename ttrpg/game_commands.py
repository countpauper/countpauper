from discord.ext import commands
import d20

class GameCommands(commands.Cog):
    def __init__(self, bot):
        self.bot = bot

    @commands.command()
    async def roll(self, ctx, arg="1d6"):
        """Roll dice. By default it rolls a six sided die.
        Examples:
            roll 1d12 - rolls a twelve sided die
            roll 2d6 - rolls two six sided dice and adds them.
            roll 1d4+1 - rolls a four sided die and adds 1."""
        result = d20.roll(arg)
        await ctx.send(f'**{ctx.author}**: :game_die: {result}')
        await ctx.message.delete()
