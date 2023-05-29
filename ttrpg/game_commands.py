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
        if self.db.exists(ctx.guild, ctx.author, name):
            raise commands.CommandError(f"You already have a character named '{name}'. Retire first.")
        elif self.db.exists(ctx.guild, None, name):
            raise commands.CommandError(f"'{name}' already exists on {ctx.guild}. Chose another name.")
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
            await ctx.message.delete()
        elif name is None:
            raise commands.CommandError(f"No character found for {ctx.author} on {ctx.guild}.")
        else:
            raise commands.CommandError(f"No character '{name}' found for {ctx.author} on {ctx.guild}.")

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
        except RuntimeError as e:
            raise commands.CommandError(*e.args)

    @commands.command()
    async def attack(self, ctx, *args):
        """Delete a character
        Syntax: attack [<attacker>] [<target>] [<bonus>]
            attacker : name of your character to attack with
            targer : name of the target to attack
        Examples:
            attack - Roll an attack with your current character without a target.
            attack Bar - Attack Bar with your default character.
            attack Foo Bar - Foo will attack Bar.
            attack Bar 1 - Your default character will attack Bar with a +1 bonus.
        """
        if not args:
            attacker_name, target_name, bonus = None, None, None
        else:
            try:
                bonus=int(args[-1])
                args=args[:-1]
            except ValueError:
                bonus = None

            if not args:
                attacker_name, target_name = None, None
            elif len(args)==1:
                attacker_name = None
                target_name = args[0]   # check if its int first
            else:
                attacker_name, target_name = args[:2]

        if attacker_name:
            attacker = self.db.retrieve(ctx.guild, ctx.author, attacker_name)
            if not attacker:
                raise commands.CommandError(f"No character {attacker_name} found for {ctx.author} on {ctx.guild}.")
        else: # default character
            attacker = self.db.retrieve(ctx.guild, ctx.author)
            if not attacker:
                raise commands.CommandError(f"No character found for {ctx.author} on {ctx.guild}.")

        if target_name:
            owner = self.db.user(ctx.guild, target_name)
            target = self.db.retrieve(ctx.guild, None, target_name)
            if not target:
                raise commands.CommandError(f"No target {target_name} found on {ctx.guild}.")
        else:
            target = None
        if target:
            result = attacker.attack(target, 0, bonus)
            self.db.store(ctx.guild, owner, target)
            await ctx.send(result)
        else:
            result = d20.roll(str(attacker.attack_dice(0, bonus)))
            await ctx.send(f'**{attacker.name}** attacks: {result}')
        await ctx.message.delete()
