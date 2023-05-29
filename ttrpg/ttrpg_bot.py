# proto_bot.py
import os
import discord
from discord.ext import commands

from game_commands import GameCommands

TOKEN = os.getenv('DISCORD_TOKEN')

intents = discord.Intents.default()
intents.members = True
intents.message_content = True
bot = commands.Bot(command_prefix='$', intents=intents)

# startup
@bot.event
async def on_ready():
    await bot.add_cog(GameCommands(bot), override=True)
    print(f'{bot.user} has connected to Discord!')
    print(
        f'{bot.user} is connected to the following guild:\n'+
        '\n'.join(f'{guild.name}(id: {guild.id})' for guild in bot.guilds)
    )

# scan all messages, pass on to the bot
@bot.event
async def on_message(message):
    print(f"Message from {message.author} in {message.channel}: {message.content}")
    await bot.process_commands(message)

# handle all errors like argument parsing
@bot.event
async def on_command_error(ctx, error):
    if isinstance(error, discord.ClientException):
        await ctx.send(f"{error}")
    elif isinstance(error, commands.CommandError):
        await ctx.send(f"{error}")
    else:
        await ctx.send(f"Internal Bot Error.")
        raise error

bot.run(TOKEN)
