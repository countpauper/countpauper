from game_commands import GameCommands
from unittest.mock import Mock, MagicMock
import pytest
from character import Character
from discord.ext import commands

class AsyncMock(Mock):
    async def __call__(self, *args, **kwargs):
        return super(AsyncMock, self).__call__(*args, **kwargs)

@pytest.fixture
def ctx():
    ctx = AsyncMock()
    # TODO fixture for author and perhaps message
    ctx.message = AsyncMock()
    ctx.author = MagicMock()
    ctx.author.__str__.return_value = 'Foo#1234'
    ctx.author.color = '#123456'
    ctx.author.nick = 'baz'
    ctx.author.display_avatar = "http://portrait.com/avatar.jpg"
    ctx.guild = 'BarGuild'
    return ctx

@pytest.mark.asyncio
async def test_roll(ctx):
    bot = Mock()
    g = GameCommands(bot)
    await g.roll(g, ctx, "1d1")
    ctx.message.delete.assert_called_once_with()
    ctx.send.assert_called_once_with(f"**{ctx.author}**: :game_die: 1d1 (**1**) = `1`")

@pytest.mark.asyncio
async def test_generate(db, ctx):
    bot = Mock()
    g = GameCommands(bot, db)
    await g.generate(g, ctx)

    c = db.retrieve("BarGuild", "Foo#1234", "Baz")
    assert c is not None

    ctx.message.delete.assert_called_once_with()
    ctx.send.assert_called_once()

    # TODO refactor embed comparison, it's a dict
    embed = ctx.send.call_args_list[0][1].get('embed')

    assert embed is not None
    assert embed.title == 'Baz'
    assert embed.color.value == int(ctx.author.color[1:], 16)
    assert "**Level:** 1" in embed.description
    assert f"**Physical:** {c.physical}" in embed.description
    assert f"**Mental:** {c.mental}" in embed.description
    assert f"**Social:** {c.social}" in embed.description
    assert embed.thumbnail.url == ctx.author.display_avatar

@pytest.mark.asyncio
async def test_generate_duplicate(db, ctx):
    c = Character()
    db.store(ctx.guild, ctx.author, c)

    bot = Mock()
    g = GameCommands(bot, db)
    with pytest.raises(commands.CommandError) as exc_info:
        await g.generate(g, ctx, c.name)
    assert exc_info.value.args[0] == f"You already have a character named '{c.name}'. Retire first."


@pytest.mark.asyncio
async def test_generate_existing(db, ctx):
    c = Character()
    db.store(ctx.guild, "baz", c)

    bot = Mock()
    g = GameCommands(bot, db)
    with pytest.raises(commands.CommandError) as exc_info:
        await g.generate(g, ctx, c.name)
    assert exc_info.value.args[0] == f"'{c.name}' already exists on {ctx.guild}. Chose another name."


@pytest.mark.asyncio
async def test_sheet(db, ctx):
    c = Character()
    db.store(ctx.guild, ctx.author, c)

    bot = Mock()
    g = GameCommands(bot, db)
    await g.sheet(g, ctx)

    ctx.message.delete.assert_called_once_with()
    ctx.send.assert_called_once()

    embed = ctx.send.call_args[1].get('embed')

    assert embed is not None
    assert embed.title == c.name.capitalize()
    assert embed.color is None
    assert "**Level:** 1" in embed.description
    assert f"**Physical:** {c.physical}" in embed.description
    assert f"**Mental:** {c.mental}" in embed.description
    assert f"**Social:** {c.social}" in embed.description
    assert f"**HP:** {c.hp}" in embed.description
    assert f"**MP:** {c.mp}" in embed.description
    assert f"**SP:** {c.sp}" in embed.description
    assert embed.thumbnail.url is None

@pytest.mark.asyncio
async def test_sheet(db, ctx):
    c = Character()
    db.store(ctx.guild, ctx.author, c)

    bot = Mock()
    g = GameCommands(bot, db)
    await g.sheet(g, ctx)

    ctx.message.delete.assert_called_once_with()
    ctx.send.assert_called_once()

    embed = ctx.send.call_args[1].get('embed')
    assert embed.title == c.name.capitalize()
    assert embed.color is None
    assert "**Level:** 1" in embed.description
    assert f"**Physical:** {c.physical}" in embed.description
    assert f"**Mental:** {c.mental}" in embed.description
    assert f"**Social:** {c.social}" in embed.description
    assert f"**HP:** {c.hp}" in embed.description
    assert f"**MP:** {c.mp}" in embed.description
    assert f"**SP:** {c.sp}" in embed.description
    assert embed.thumbnail.url is None


@pytest.mark.asyncio
async def test_no_sheet(db, ctx):
    bot = Mock()
    g = GameCommands(bot, db)
    with pytest.raises(commands.CommandError) as exc_info:
        await g.sheet(g, ctx, "foo")
    assert exc_info.value.args[0] == f"No character 'foo' found for {ctx.author} on {ctx.guild}."
    ctx.message.delete.assert_not_called()

@pytest.mark.asyncio
async def test_retire(db, ctx):
    c = Character()
    db.store(ctx.guild, ctx.author, c)
    bot = Mock()
    g = GameCommands(bot, db)
    await g.retire(g, ctx, c.name)
    assert not db.exists(ctx.guild, ctx.author, c.name)
    ctx.send.assert_called_once_with(f"Your character {c.name} has been retired.")
    ctx.message.delete.assert_called_once_with()


@pytest.mark.asyncio
async def test_no_retire(db, ctx):
    bot = Mock()
    g = GameCommands(bot, db)
    with pytest.raises(commands.CommandError) as exc_info:
        await g.retire(g, ctx, "foo")
    assert exc_info.value.args[0] == f"No character 'foo' found for {ctx.author} on {ctx.guild}."
    ctx.message.delete.assert_not_called()


@pytest.mark.asyncio
async def test_attack_with_default_character(db, ctx):
    attacker = Character(name="Attacker", physical=0)
    target = Character(name="Target", physical=0)
    db.store(ctx.guild, ctx.author, attacker)
    db.store(ctx.guild, "Opponent", target)
    bot = Mock()
    g = GameCommands(bot, db)
    await g.attack(g, ctx, "target", "-1")
    ctx.message.delete.assert_called_with()
    ctx.send.assert_called_once_with(f"**{attacker.name}** attacks: 1 - 1 = `0` VS 1 = `1` misses {target.name}")


@pytest.mark.asyncio
async def test_attack_with_specific_character(db, ctx):
    attacker = Character(name="Attacker", physical=0)
    target = Character(name="Target", physical=0)
    default_char = Character(name="Default")
    db.store(ctx.guild, ctx.author, attacker)
    db.store(ctx.guild, ctx.author, default_char)
    db.store(ctx.guild, "Opponent", target)
    bot = Mock()
    g = GameCommands(bot, db)
    await g.attack(g, ctx, "Attacker", "target", "+1")
    ctx.message.delete.assert_called_with()
    ctx.send.assert_called_once_with(f"**{attacker.name}** attacks: 1 + 1 = `2` VS 1 = `1` hits {target.name} (4/5)[-1]")
    target = db.retrieve(ctx.guild, "Opponent", target.name)
    assert target.hp == 4

@pytest.mark.asyncio
async def test_cant_attack_without_default_character(db, ctx):
    db.store(ctx.guild, "Opponent", Character(name="Target"))
    bot = Mock()
    g = GameCommands(bot, db)
    with pytest.raises(commands.CommandError) as exc_info:
        await g.attack(g, ctx, "target")
    assert exc_info.value.args[0] == f"No character found for {ctx.author} on {ctx.guild}."


@pytest.mark.asyncio
async def test_attack_without_target(db, ctx):
    attacker = Character(name="Attacker", physical=0)
    db.store(ctx.guild, ctx.author, attacker)
    bot = Mock()
    g = GameCommands(bot, db)
    await g.attack(g, ctx)
    ctx.message.delete.assert_called_with()
    ctx.send.assert_called_once_with(f"**{attacker.name}** attacks: 1 = `1`")


