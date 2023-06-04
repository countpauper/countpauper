from character import Character
from generate_character import random_character
from errors import CharacterUnknownError
from skills import Skill
from character_db import CharacterDB
from discord.ext import commands
import discord
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

        embed = discord.Embed(title=c.Name(),
                      description=description,
                      color=None if c.color is None else discord.Color.from_str(c.color))
        if c.portrait:
            embed.set_thumbnail(url=c.portrait)
        embed.add_field(name=f"Inventory [{c.carried()}/{c.capacity()}]", value="\n".join(str(i).capitalize() for i in c.inventory), inline=True)
        embed.add_field(name="Skills", value="\n".join(Skill.name(s).capitalize() for s in c.skill), inline=True)
        embed.add_field(name="Effects", value="\n".join(str(s).capitalize() for s in c.effects), inline=True)
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
        c = random_character(level)
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
        else:
            raise CharacterUnknownError(ctx.guild, ctx.author, name)

    @commands.command()
    async def retire(self, ctx, name):
        """Delete a character
        Syntax: retire name
            name : name of the character to retire.
        Examples:
            retire foo - retire your character foo"""
        self.db.delete(ctx.guild, ctx.author, name)
        await ctx.send(f"Your character {name} has been retired.")
        await ctx.message.delete()

    def _optional_character_argument(self, guild, owner, args):
        """Get the character from the specified guild and owner. Owner can be None.
        Uses the first argument if it's the name of a character and returns the remaining arguments.
        or returns the default character and all unused arguments."""
        args=list(args)
        if args and (c := self.db.retrieve(guild, owner, args[0])):
            return c, args[1:]
        elif c := self.db.retrieve(guild, owner, None):
            return c, args
        else:
            raise CharacterUnknownError(guild, owner)

    @commands.command()
    async def take(self, ctx, *args):
        c, args = self._optional_character_argument(ctx.guild, ctx.author, args)
        if not args:
            raise commands.CommandError("The item {c.name} will take is a required argument, which is missing.")
        items = c.obtain(*args)
        c.auto_equip()
        self.db.store(ctx.guild, ctx.author, c)
        await ctx.send(f"**{c.name}** takes the {' and '.join(str(item) for item in items)}.")
        await ctx.message.delete()

    @commands.command()
    async def drop(self, ctx, *args):
        c, args = self._optional_character_argument(ctx.guild, ctx.author, args)
        if not args:
            raise commands.CommandError("The item {c.name} will drop is a required argument, which is missing.")
        items = c.lose(*args)
        c.auto_equip()
        self.db.store(ctx.guild, ctx.author, c)
        await ctx.send(f"**{c.name}** drops the {' and '.join(str(item) for item in items)}.")
        await ctx.message.delete()


    @commands.command()
    async def turn(self, ctx, name=None):
        if c := self.db.retrieve(ctx.guild, ctx.author, name):
            c.turn()
            self.db.store(ctx.guild, ctx.author, c)
            await ctx.send(f"**{c.name}** ap: {c.ap}")
            await ctx.message.delete()
        else:
            raise CharacterUnknownError(ctx.guild, ctx.author, name)

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
        attacker, args = self._optional_character_argument(ctx.guild, ctx.author, args)
        bonus = None
        if args:
            try:
                bonus=int(args[-1])
                args=args[:-1]
            except ValueError:
                pass
        if args:
            target_name = args[0]
            owner = self.db.user(ctx.guild, target_name)
            target = self.db.retrieve(ctx.guild, owner, target_name)
            if not target:
                raise CharacterUnknownError(ctx.guild, None, target_name)
        else:
            target_name, target = "", None
        if target:
            result = attacker.attack(target, attacker.attack_dice(0) + bonus)
            self.db.store(ctx.guild, owner, target)
            self.db.store(ctx.guild, ctx.author, attacker)
            if result.hit():
                await ctx.send(f"**{attacker.Name()}** attacks {target.Name()} [{target['hp']}]: {result}.")
            else:
                await ctx.send(f"**{attacker.Name()}** attacks {target.Name()}: {result}.")
        else:
            result = attacker.attack(None, attacker.attack_dice(0) + bonus)
            self.db.store(ctx.guild, ctx.author, attacker)
            await ctx.send(f"**{attacker.Name()}** attacks: {result}.")
        await ctx.message.delete()


    @commands.command()
    async def cover(self, ctx, name=None):
        if c := self.db.retrieve(ctx.guild, ctx.author, name):
            item = c.cover()
            self.db.store(ctx.guild, ctx.author, c)
            if item:
                await ctx.send(f"**{c.name}** seeks cover behind their {item}.")
            else:
                await ctx.send(f"**{c.name}** seeks cover nearby.")
            await ctx.message.delete()
        else:
            raise CharacterUnknownError(ctx.guild, ctx.author, name)


    @commands.command()
    async def skill(self, ctx, *args):
        e, args = self._optional_character_argument(ctx.guild, ctx.author, args)
        if not args:
            await ctx.message.delete()
            await ctx.send(f"""**{e.Name()}** knows {', '.join(Skill.name(s) for s in e.skill)}.""")
        else:
            skill=args.pop(0)
            targets={arg:self.db.retrieve(ctx.guild, None, arg) for arg in args}
            if unknown:=[arg for arg, target in targets.items() if target is None]:
                raise CharacterUnknownError(ctx.guild, None, *unknown)
            result = e.execute(skill, *targets.values())
            self.db.store(ctx.guild, ctx.author, e)
            for t in targets.values():
                self.db.store(ctx.guild, t.user, t)
            # TODO: store targets, if they are dirty (used) and need their owner when retrieving (simplify in attack as well)
            await ctx.send(f"**{e.name}** {result}.")    # TODO don't put name in result so it can be formatted
            await ctx.message.delete()

