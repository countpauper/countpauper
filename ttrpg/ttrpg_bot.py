# proto_bot.py
import os
import discord
from discord.ext import commands
from character import Character
from character_db import CharacterDB
import d20

TOKEN = os.getenv('DISCORD_TOKEN')

intents = discord.Intents.default()
intents.members = True
intents.message_content = True
bot = commands.Bot(command_prefix='$', intents=intents)

db = CharacterDB()

def embed_sheet(c):
    description=f"""**Level:** {c.level}
**Physical** {c.physical}, **HP** {c.hp}
**Mental** {c.mental}, **SP** {c.sp}
**Social** {c.social}, **MP** {c.mp}
**Attack** {c.attack_dice()} **Defense** {c.defense_dice()}"""

    embed = discord.Embed(title=c.name,
                  description=description,
                  color=discord.Color.from_str(c.color) or discord.Color.random())
    if c.portrait:
        embed.set_thumbnail(url=c.portrait)
    embed.add_field(name=f"Inventory [{c.carried()}/{c.capacity()}]", value="\n".join(str(i) for i in c.inventory), inline=True)
    embed.add_field(name="Skills", value="\n".join(str(s) for s in c.skills), inline=True)
    return embed

# startup
@bot.event
async def on_ready():
    print(f'{bot.user} has connected to Discord!')
    print(
        f'{bot.user} is connected to the following guild:\n'+
        '\n'.join(f'{guild.name}(id: {guild.id})' for guild in bot.guilds)
    )

@bot.command()
async def roll(ctx, arg="1d6"):
    """Roll dice. By default it rolls a six sided die.
    Examples:
        roll 1d12 - rolls a twelve sided die
        roll 2d6 - rolls two six sided dice.
        roll 1d4+1 - rolls a four sided die with a +1 bonus."""
    result = d20.roll(arg)
    await ctx.send(f'**{ctx.author}**: :game_die: {result}')
    await ctx.message.delete()

@bot.command()
async def generate(ctx, name=None, level=1):
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
    if c := db.exists(ctx.guild, ctx.author, name):
        raise commands.CommandError(f"You already have a character named '{name}'. Retire first.")
    c = Character.random_character(level)
    c.name = name
    c.color = str(ctx.author.color)
    c.portrait = str(ctx.author.display_avatar)
    db.store(ctx.guild, ctx.author, c)
    await ctx.send(embed=embed_sheet(c))
    await ctx.message.delete()

@bot.command()
async def sheet(ctx, name=None):
    """Show the sheet of the given character,.
    Syntax: sheet [<name>]
        name: the name of the charter whose sheet to show. If omitted your default character is shown
    Examples:
        sheet - Show the sheet of your active (latest) character.
        sheet foo - Show the sheet of your character foo."""
    if c := db.retrieve(ctx.guild, ctx.author, name):
        await ctx.send(embed=embed_sheet(c))
    else:
        raise Exception(f"No character {name} found for {ctx.author} on {ctx.guild}.")
    await ctx.message.delete()

@bot.command()
async def retire(ctx, name):
    """Delete a character
    Syntax: retire name
        name : name of the character to retire.
    Examples:
        retire foo - retire your character foo"""
    try:
        db.delete(ctx.guild, ctx.author, name)
        await ctx.send(f"Your character {name} has been retired.")
        await ctx.message.delete()
    except Exception as e:
        raise commands.CommandError(*e.args)


# scan all messages, pass on to the bot
@bot.event
async def on_message(message):
    print(f"Message from {message.author} in {message.channel}: {message.content}")
    await bot.process_commands(message)

# handle all errors like argument parsing
@bot.event
async def on_command_error(ctx, error):
    if isinstance(error, discord.ClientException) or isinstance(error, commands.CommandError):
        await ctx.send(f"Error: {error}")
    else:
        await ctx.send(f"Bot Error.")
        raise error

bot.run(TOKEN)