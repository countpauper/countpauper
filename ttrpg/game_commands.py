from discord.ext import commands
import discord
from character_db import CharacterDB
from character import Character
import d20

class GameCommands(commands.Cog):
    def __init__(self, bot, db=CharacterDB()):
        self.bot = bot
        self.db = db

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

    @staticmethod
    def embed_sheet(c):
        description=f"""**Level:** {c.level}
    **Physical:** {c.physical}, **HP:** {c.hp}
    **Mental:** {c.mental}, **SP:** {c.sp}
    **Social:** {c.social}, **MP:** {c.mp}
    **Attack:** {c.attack_dice()} **Defense:** {c.defense_dice()}"""

        embed = discord.Embed(title=c.name.capitalize(),
                      description=description,
                      color=None if c.color is None else discord.Color.from_str(c.color))
        if c.portrait:
            embed.set_thumbnail(url=c.portrait)
        embed.add_field(name=f"Inventory [{c.carried()}/{c.capacity()}]", value="\n".join(str(i) for i in c.inventory), inline=True)
        embed.add_field(name="Skills", value="\n".join(str(s) for s in c.skills), inline=True)
        return embed


    @commands.command()
    async def generate(self, ctx, name=None, level=1):
        """Generate a new character and show its sheet.
        Syntax generate [<name>] [<level>=1]
            name - Character name, default: your discord nick name
            level - Character level (1-10), default 1
        The character will become your default character.

        Examples:
            generate - Generates a level 1 character with your nick, portrait and color
            generate Foo - Generates a level 1 character named foo.
            generate Bar 2 - Generate a level 2 character named Bar"""

        name = name or ctx.author.nick or ctx.author.name
        if c := self.db.exists(ctx.guild, ctx.author, name):
            raise commands.CommandError(f"You already have a character named '{name}'. Retire first.")
        c = Character.random_character(level)
        c.name = name
        c.color = str(ctx.author.color)
        c.portrait = str(ctx.author.display_avatar)
        self.db.store(ctx.guild, ctx.author, c)
        await ctx.send(embed=self.embed_sheet(c))
        await ctx.message.delete()

    @commands.command()
    async def sheet(self, ctx, name=None):
        """Show the sheet of the given character,.
        Syntax: sheet [<name>]
            name: the name of the charter whose sheet to show. If omitted your default character is shown
        Examples:
            sheet - Show the sheet of your active (latest) character.
            sheet foo - Show the sheet of your character foo."""
        if c := self.db.retrieve(ctx.guild, ctx.author, name):
            await ctx.send(embed=self.embed_sheet(c))
        else:
            await ctx.send(f"No character {name if name else ''} found for {ctx.author} on {ctx.guild}.")
        await ctx.message.delete()

    @commands.command()
    async def retire(self, ctx, name):
        """Delete a character
        Syntax: retire name
            name : name of the character to retire.
        Examples:
            retire foo - retire your character foo"""
        try:
            self.db.delete(ctx.guild, ctx.author, name)
            await ctx.send(f"Your character {name} has been retired.")
            await ctx.message.delete()
        except Exception as e:
            raise commands.CommandError(*e.args)
