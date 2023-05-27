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
**Mental** {c.physical}, **SP** {c.sp}
**Social** {c.physical}, **MP** {c.mp}
**Attack** {c.attack_dice()} **Defense** {c.defense_dice()}"""

    embed = discord.Embed(title=c.name,
                  description=description,
                  color=discord.Color.from_str(c.color) or discord.Color.random())
    if c.portrait:
        embed.set_thumbnail(url=c.portrait)
    # TODO: field for inventory and skills
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
    result = d20.roll(arg)
    await ctx.send(f'**{ctx.author}**: :game_die: {result}')
    await ctx.message.delete()

@bot.command()
async def generate(ctx, name=None):
    c = Character.random_pc()
    c.name = name or ctx.author.nick or ctx.author.name
    c.color = str(ctx.author.color)
    c.portrait = str(ctx.author.display_avatar)
    db.store( ctx.guild, ctx.author, c)
    await ctx.send(embed=embed_sheet(c))
    await ctx.message.delete()

@bot.command()
async def sheet(ctx, name=None):
    if c := db.retrieve(ctx.guild, ctx.author, name):
        await ctx.send(embed=embed_sheet(c))
    else:
        raise Exception(f"No character {name} found for {ctx.author} on {ctx.guild}.")
    await ctx.message.delete()

# scan all messages, pass on to the bot
@bot.event
async def on_message(message):
    print(f"Message from {message.author} in {message.channel}: {message.content}")
    await bot.process_commands(message)

# handle all errors like argument parsing
@bot.event
async def on_command_error(ctx, error):
    if isinstance(error, discord.ClientException):
        await ctx.send(f"Error: {error}")
    else:
        await ctx.send(f"Bot Error.")
        raise error

bot.run(TOKEN)