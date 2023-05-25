# proto_bot.py
import os
import discord
from discord.ext import commands
import random
import sqlite3
import time

TOKEN = os.getenv('DISCORD_TOKEN')

intents = discord.Intents.default()
intents.members = True
intents.message_content = True
bot = commands.Bot(command_prefix='$', intents=intents)
db = sqlite3.connect("proto.db")

# startup
@bot.event
async def on_ready():
    print(f'{bot.user} has connected to Discord!')

    cur = db.cursor()
    cur.execute("CREATE TABLE IF NOT EXISTS mind(user, time, memory)")

    print(
        f'{bot.user} is connected to the following guild:\n'+
        '\n'.join(f'{guild.name}(id: {guild.id})' for guild in bot.guilds)
    )

# scan all messages, pass on to the bot
@bot.event
async def on_message(message):
    print(f"Message from {message.author} in {message.channel}: {message.content}")
    await bot.process_commands(message)

#@bot.event
#async def on_message(message):
#    if message.author == bot.user:
#        print('Saw my own message')
#        return
#    print(f'Message by {message.author}')

# add rows to the database
@bot.command()
async def memory(ctx, *, arg):
    print(ctx.message.content)
    db_cur = db.cursor()
    db_cur.execute(f"""INSERT INTO mind VALUES ('{ctx.author}', {time.time()}, '{arg}')""")
    db.commit()
    embed=discord.Embed(title="Mind",
                        url="https://www.sqlite.org/index.html",
                        description="This embed will show how to build an embed and the different components",
                        color=discord.Color.blue())
    embed.set_thumbnail(url='https://images.pexels.com/photos/1003914/pexels-photo-1003914.jpeg')
    embed.add_field(name="Remeber=",value=arg)
    await ctx.send(embed=embed)
    await ctx.message.delete()


# select rows from the database
@bot.command(name='remember')
async def _remember(ctx):
    print(ctx.message.content)
    db_cur = db.cursor()
    res = db_cur.execute(f"""SELECT memory FROM mind WHERE user='{ctx.author}'""")
    all_results = res.fetchall()
    print(all_results)
    await ctx.send(f'memories: {", ".join(col[0] for col in all_results)}')

# delete rows from the database
@bot.command()
async def forget(ctx):
    print(ctx.message.content)
    db_cur = db.cursor()
    res = db_cur.execute(f"""DELETE FROM mind WHERE user='{ctx.author}'""")
    db.commit()
    await ctx.send(f'Forgot {res.rowcount} memories')

# handle all errors like argument parsing
@bot.event
async def on_command_error(ctx, error):
    await ctx.send(f"{error}")

bot.run(TOKEN)