from character import Character
from actions import Cover, Attack
from generate_character import random_character
from errors import CharacterUnknownError
from ability import Physical, Mental, Social, abilities
from character_db import CharacterDB
from discord.ext import commands
from language import list_off, possessive, is_name
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
    def character_description(c):
        return f"""**Level:** {c.level}
**Physical:** {c.physical} [{c.ability_dice(Physical)}], **HP:** {c.hp}
**Mental:** {c.mental} [{c.ability_dice(Mental)}], **PP:** {c.pp}
**Social:** {c.social} [{c.ability_dice(Social)}], **MP:** {c.mp}
**Attack:** {c.attack_dice()}, **Defense:** {c.defense_dice()}"""

    @staticmethod
    def embed_sheet(c, **kwargs):
        description=GameCommands.character_description(c)

        embed = discord.Embed(title=kwargs.get('title', str(c)),
                      description=description,
                      color=None if c.color is None else discord.Color.from_str(c.color))
        if c.portrait:
            embed.set_thumbnail(url=c.portrait)
        if kwargs.get('inventory', True) and c.capacity():
            embed.add_field(name=f"Inventory [{c.carried()}/{c.capacity()}]", value="\n".join(str(i).capitalize() for i in c.inventory), inline=True)
        if kwargs.get('skills', True) and c.skill:
            embed.add_field(name=f"Skills [{len(c.skill)}/{c.memory()}]", value="\n".join(str(s).capitalize() for s in c.skill), inline=True)
        if kwargs.get('effects', True) and c.effects:
            embed.add_field(name="Effects", value="\n".join(str(e).capitalize() for e in c.effects), inline=True)
        if kwargs.get('allies', True):
            for ally in c.allies:
                embed.add_field(name=str(ally), value=GameCommands.character_description(ally))
        return embed

    @staticmethod
    def embed_result(c, verb, result):
        description = f"{result}."
        embed = discord.Embed(title=f"{c} {verb}.",
                              description=description,
                              color = None if c.color is None else discord.Color.from_str(c.color))
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
        if not is_name(name):
            raise commands.CommandError(f"{name} is not a valid name. The name must be between 3 and 30 characters and consists of letters and spaces.")
        if self.db.exists(ctx.guild, ctx.author, name):
            raise commands.CommandError(f"You already have a character named '{name}'. Retire first.")
        elif self.db.exists(ctx.guild, None, name):
            raise commands.CommandError(f"'{name}' already exists on {ctx.guild}. Chose another name.")
        c = random_character(level)
        c.name = name
        c.color = str(ctx.author.color)
        c.portrait = str(ctx.author.display_avatar)
        self.db.store(ctx.guild, ctx.author, c)
        await ctx.send(embed=self.embed_sheet(c, title=f"New: {c}", effects=False))
        await ctx.message.delete()

    @commands.command()
    async def portrait(self, ctx, *args):
        """Change the portrait of a character
        Syntax portrait [<name>] [<url>]
            name: the name of the character whose portrait will be changed. If omitted your default character is shown
            url: the url of the image to change the portrait to. It must end in an image extension, eg .jpg or .png
        Examples:
            portrait - Show the portrait of your current character
            portrait foo - Show the portrait of the character named 'Foo
            portrait foo http://www.portraits.com/dwarf.jpg - Change Foo's portrait to a dwarf
            portrait http://www.portraits.com/zombie.png - Change your character's portrait to a zombie."""
        c, args = self._optional_character_argument(ctx.guild, ctx.author, args)
        if args:
            url = args[0]
            if not any(url.endswith(f".{ext}") for ext in ["jpg", "jpeg", "png", "gif", "gifv", "mp4", "flac"]):
                raise commands.CommandError("Invalid file extension");
            c.portrait = url
            self.db.store(ctx.guild, ctx.author, c)
        else:
            url = c.portrait
        embed = discord.Embed(type="image", title=f"{possessive(c)} portrait.")
        embed.set_image(url=url)
        await ctx.send(embed=embed)
        await ctx.message.delete()

    @commands.command()
    async def sheet(self, ctx, name=None):
        """Show the sheet of the given character,.
        Syntax: sheet [<name>]
            name: the name of the character whose sheet to show. If omitted your default character is shown
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
        if (name := args[0] if args else None) and is_name(name) and (c := self.db.retrieve(guild, owner, name)):
            return c, list(args)[1:]
        elif c := self.db.retrieve(guild, owner, None):
            return c, args
        else:
            raise CharacterUnknownError(guild, owner, name)

    @commands.command()
    async def take(self, ctx, *args):
        c, args = self._optional_character_argument(ctx.guild, ctx.author, args)
        if not c:
            raise commands
        if not args:
            raise commands.CommandError("The item {c.name} will take is a required argument, which is missing.")
        items = c.obtain(*args)
        c.auto_equip()
        self.db.store(ctx.guild, ctx.author, c)
        await ctx.send(f"**{c}** takes the {list_off(items)}. Carried [{c.carried()}/{c.capacity()}]")
        await ctx.message.delete()

    @commands.command()
    async def drop(self, ctx, *args):
        c, args = self._optional_character_argument(ctx.guild, ctx.author, args)
        if not args:
            raise commands.CommandError("The item {c.name} will drop is a required argument, which is missing.")
        items = c.lose(*args)
        c.auto_equip()
        self.db.store(ctx.guild, ctx.author, c)
        await ctx.send(f"**{c}** drops the {list_off(items)}. Carried [{c.carried()}/{c.capacity()}]")
        await ctx.message.delete()

    @commands.command()
    async def level(self, ctx, *args):
        c, args = self._optional_character_argument(ctx.guild, ctx.author, args)
        if args:
            c.level_up(args[0])
            self.db.store(ctx.guild, ctx.author, c)
            await ctx.send(embed=self.embed_sheet(c, title=f"{c} levels up to {c.level}!", inventory=False, effects=False, allies=False))
            await ctx.message.delete()
        else:
            raise commands.CommandError(f"You have to specify the ability to level up: {list_off(a.name for a in abilities)}")

    @commands.command()
    async def learn(self, ctx, *args):
        c, args = self._optional_character_argument(ctx.guild, ctx.author, args)
        if args:
            skills = c.learn(*args)
            self.db.store(ctx.guild, ctx.author, c)
            await ctx.message.delete()
            await ctx.send(f"**{c}** learns {list_off(skills)}.")
        else:
            raise commands.CommandError(f"You have to specify the ability to level up: {list_off(a.name for a in abilities)}")


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
            if target_name.lower() == attacker.name:
                owner = ctx.author
                target = attacker
            else:
                owner = self.db.user(ctx.guild, target_name)
                target = self.db.retrieve(ctx.guild, owner, target_name)
            if not target:
                raise CharacterUnknownError(ctx.guild, None, target_name)
        else:
            target_name, target = "", None
        dice = attacker.attack_dice(0) + bonus
        if target:
            result = attacker.execute(Attack, target, attack_dice=dice)
            self.db.store(ctx.guild, owner, target)
        else:
            result = attacker.execute(Attack, attack_dice=dice)
        self.db.store(ctx.guild, ctx.author, attacker)
        await ctx.send(embed = self.embed_result(attacker, "attacks", result))
        await ctx.message.delete()


    @commands.command()
    async def cover(self, ctx, name=None):
        if c := self.db.retrieve(ctx.guild, ctx.author, name):
            result = c.execute(Cover)
            self.db.store(ctx.guild, ctx.author, c)
            await ctx.send(f"**{c.name}** {result}.")
            await ctx.message.delete()
        else:
            raise CharacterUnknownError(ctx.guild, ctx.author, name)


    @commands.command()
    async def skill(self, ctx, *args):
        e, args = self._optional_character_argument(ctx.guild, ctx.author, args)
        if not args:
            await ctx.message.delete()
            await ctx.send(f"""**{e}** knows {list_off(str(s).capitalize() for s in e.skill)}.""")
        else:
            args = list(args)
            skill=args.pop(0)
            targets={arg:e if arg.lower()==e.name.lower() else self.db.retrieve(ctx.guild, None, arg) for arg in args}
            if unknown:=[arg for arg, target in targets.items() if target is None]:
                raise CharacterUnknownError(ctx.guild, None, *unknown)
            result = e.execute(skill, *targets.values())
            self.db.store(ctx.guild, ctx.author, e)
            for t in targets.values():
                self.db.store(ctx.guild, t.user, t)
            # TODO: store targets, if they are dirty (used) and need their owner when retrieving (simplify in attack as well)
            await ctx.send(embed = self.embed_result(e, result.action.verb(), result))
            await ctx.message.delete()
