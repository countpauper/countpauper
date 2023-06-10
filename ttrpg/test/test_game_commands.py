from game_commands import GameCommands
from unittest.mock import Mock, MagicMock
import pytest
from character import Character
from items import MeleeWeapon, Armor
from skills import Parry, Riposte, CrossCut
from effect import Effect
from discord.ext import commands
import discord
import re

class AsyncMock(Mock):
    async def __call__(self, *args, **kwargs):
        return super(AsyncMock, self).__call__(*args, **kwargs)


@pytest.fixture
def ctx():
    ctx = AsyncMock()
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
    assert embed.title == 'New: Baz'
    assert embed.color.value == int(ctx.author.color[1:], 16)
    assert "**Level:** 1" in embed.description
    assert f"**Physical:** {c.physical}" in embed.description
    assert f"**Mental:** {c.mental}" in embed.description
    assert f"**Social:** {c.social}" in embed.description
    assert embed.thumbnail.url == ctx.author.display_avatar
    assert len(embed.fields) == 2
    assert embed.fields[0].name.startswith("Inventory")
    assert embed.fields[1].name.startswith("Skills")

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
    c = Character(physical=2, mental=2, social=1, inventory=[MeleeWeapon(name="Practice Sword"), Armor(rating=1)], skill=[Parry])
    c.effects.append(Effect(name="displayed"))

    db.store(ctx.guild, ctx.author, c)

    bot = Mock()
    g = GameCommands(bot, db)
    await g.sheet(g, ctx)

    ctx.message.delete.assert_called_once_with()
    ctx.send.assert_called_once()

    embed = ctx.send.call_args[1].get('embed')

    assert embed is not None
    assert embed.title == "Nemo"
    assert embed.color is None
    assert re.match(r"\*\*Level:\*\* \d\n\*\*Physical\:\*\* \d \[1d\d\], \*\*HP\:\*\* \d\/\d\n\*\*Mental:\*\* \d \[1d\d\], \*\*PP\:\*\* \d\/\d\n\*\*Social:\*\* \d \[1], \*\*MP\:\*\* \d\/\d\n\*\*Attack:\*\* 1d\d, \*\*Defense:\*\* 1d\d\+1",
                    embed.description)
    assert embed.thumbnail.url is None
    assert embed.fields[0].name == f"Inventory [2/{c.capacity()}]"
    assert embed.fields[0].value == "Practice sword\nGambeson"
    assert embed.fields[1].name == "Skills [1/1]"
    assert embed.fields[1].value == "Parry"
    assert embed.fields[2].name == "Effects"
    assert embed.fields[2].value == "Displayed"

@pytest.mark.asyncio
async def test_sheet_allies(db, ctx):
    c = Character(social=3, allies=[
        Character(name="Foo", physical=2,level=0, hp=0),
        Character(name="Bar", mental=3, social=2, mp=2)
    ])
    db.store(ctx.guild, ctx.author, c)
    bot = Mock()
    g = GameCommands(bot, db)
    await g.sheet(g, ctx)

    ctx.message.delete.assert_called_once_with()
    ctx.send.assert_called_once()
    embed = ctx.send.call_args[1].get('embed')
    assert (foo_field := ([field for field in embed.fields if field.name == "Foo"]+[None])[0]) is not None
    assert (bar_field := ([field for field in embed.fields if field.name == "Foo"]+[None])[0]) is not None
    assert foo_field.value == """**Level:** 0
**Physical:** 2 [1d2], **HP:** 0/4
**Mental:** 1 [1], **PP:** 1/1
**Social:** 1 [1], **MP:** 1/1
**Attack:** 1d2, **Defense:** 1d2"""

@pytest.mark.asyncio
async def test_sheet_flavour(db, ctx):
    c = Character(color="#ABCDEF", portrait="http://portrait.com/face.jpg")
    db.store(ctx.guild, ctx.author, c)
    bot = Mock()
    g = GameCommands(bot, db)
    await g.sheet(g, ctx)
    ctx.send.assert_called_once()
    embed = ctx.send.call_args[1].get('embed')
    assert embed.thumbnail.url is not c.portrait
    assert embed.color == discord.Color.from_str('#ABCDEF')

@pytest.mark.asyncio
async def test_sheet_field_reduction(db, ctx):
    c = Character(physical=0, skill=[])
    db.store(ctx.guild, ctx.author, c)
    bot = Mock()
    g = GameCommands(bot, db)
    await g.sheet(g, ctx)
    ctx.send.assert_called_once()
    embed = ctx.send.call_args[1].get('embed')
    assert not any(field.name.lower().startswith("effect") for field in embed.fields)
    assert not any(field.name.lower().startswith("skill") for field in embed.fields)
    assert not any(field.name.lower().startswith("inventory") for field in embed.fields)

@pytest.mark.asyncio
async def test_no_sheet(db, ctx):
    bot = Mock()
    g = GameCommands(bot, db)
    with pytest.raises(commands.CommandError) as exc_info:
        await g.sheet(g, ctx, "foo")
    assert exc_info.value.args[0] == f"No character named Foo found for {ctx.author} on {ctx.guild}."
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
    assert exc_info.value.args[0] == f"No character named Foo found for {ctx.author} on {ctx.guild}."
    ctx.message.delete.assert_not_called()


@pytest.mark.asyncio
async def test_take(db, ctx):
    c = Character(physical=2)
    db.store(ctx.guild, ctx.author, c)
    g = GameCommands(bot := Mock(), db)
    await g.take(g, ctx, "sword", "shield")
    ctx.message.delete.assert_called_with()
    ctx.send.assert_called_once_with(f"**Nemo** takes the sword and shield. Carried [2/2]")
    c = db.retrieve(ctx.guild, ctx.author, c.name)
    assert c.main_hand().name == "sword"
    assert c.off_hand().name == "shield"

@pytest.mark.asyncio
async def test_drop(db, ctx):
    c = Character(physical=2, inventory=[MeleeWeapon(name="sword"), Armor(rating=0)])
    c.auto_equip()
    assert c.main_hand()
    assert c.worn
    db.store(ctx.guild, ctx.author, c)
    g = GameCommands(bot := Mock(), db)
    await g.drop(g, ctx, c.name, "sword", "shirt")
    ctx.message.delete.assert_called_with()
    ctx.send.assert_called_once_with(f"**Nemo** drops the sword and shirt. Carried [0/2]")
    c = db.retrieve(ctx.guild, ctx.author, c.name)
    assert c.main_hand() is None
    assert c.worn == []

@pytest.mark.asyncio
async def test_attack_with_default_character(db, ctx):
    attacker = Character(name="attacker")
    target = Character(name="target")
    db.store(ctx.guild, ctx.author, attacker)
    db.store(ctx.guild, "Opponent", target)
    g = GameCommands(bot := Mock(), db)
    await g.attack(g, ctx, "target", "-1")
    ctx.message.delete.assert_called_with()
    ctx.send.assert_called_once_with(f"**Attacker** attacks Target: 1 - 1 = 0 VS 1 misses.")


@pytest.mark.asyncio
async def test_attack_with_specific_character(db, ctx):
    attacker = Character(name="attacker")
    target = Character(name="target")
    default_char = Character(name="Default")
    db.store(ctx.guild, ctx.author, attacker)
    db.store(ctx.guild, ctx.author, default_char)
    db.store(ctx.guild, "Opponent", target)
    bot = Mock()
    g = GameCommands(bot, db)
    await g.attack(g, ctx, "Attacker", "target", "+1")
    ctx.message.delete.assert_called_with()
    ctx.send.assert_called_once_with(f"**Attacker** attacks Target [4/5]: 1 + 1 = 2 VS 1 hits for 1 damage.")
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
    attacker = Character(name="Attacker")
    db.store(ctx.guild, ctx.author, attacker)
    g = GameCommands(bot := Mock(), db)
    await g.attack(g, ctx)
    ctx.message.delete.assert_called_with()
    ctx.send.assert_called_once_with(f"**{attacker.name}** attacks: 1.")


@pytest.mark.asyncio
async def test_skilll_list(db, ctx):
    attacker = Character(name="Attacker", level=2, skill=[Parry, Riposte], inventory=[MeleeWeapon(name="axe")])
    db.store(ctx.guild, ctx.author, attacker)
    g = GameCommands(bot := Mock(), db)
    await g.skill(g, ctx)
    ctx.message.delete.assert_called_with()
    ctx.send.assert_called_once_with(f"**{attacker.name}** knows Parry and Riposte.")


@pytest.mark.asyncio
async def test_skilll_without_target(db, ctx):
    attacker = Character(name="Attacker", skill=[Parry], inventory=[MeleeWeapon(name="axe")])
    db.store(ctx.guild, ctx.author, attacker)
    g = GameCommands(bot := Mock(), db)
    await g.skill(g, ctx, "parry")
    ctx.message.delete.assert_called_with()
    ctx.send.assert_called_once_with(f"**{attacker.name}** parries with an axe.")

@pytest.mark.asyncio
async def test_skilll_with_target(db, ctx):
    attacker = Character(name="attacker", physical=2, skill=[CrossCut], inventory=[MeleeWeapon(), MeleeWeapon()])
    db.store(ctx.guild, ctx.author, attacker)
    target = Character(name="target", physical=0, level=-3)
    db.store(ctx.guild, ctx.author, target)
    g = GameCommands(bot := Mock(), db)
    await g.skill(g, ctx, "attacker", "cross cut", "target")
    ctx.message.delete.assert_called_with()
    target = db.retrieve(ctx.guild, ctx.author, target.name)
    assert target.hp == 0


@pytest.mark.asyncio
async def test_level_up(db, ctx):
    c = Character(name="leveler", physical=2, skill=[Parry])
    db.store(ctx.guild, ctx.author, c)
    g = GameCommands(bot := Mock(), db)
    await g.level(g, ctx, "physical")
    ctx.message.delete.assert_called_with()
    ctx.send.assert_called_once()
    embed = ctx.send.call_args[1].get('embed')
    assert embed.title == "Leveler levels up to 2!"
    assert len(embed.fields) == 1
    assert embed.fields[0].name == "Skills [1/2]"
    leveled = db.retrieve(ctx.guild, ctx.author, c.name)
    assert leveled.level == 2
    assert leveled.physical == 3


@pytest.mark.asyncio
async def test_learn(db, ctx):
    c = Character(name="learner", level=2, mental=2)
    db.store(ctx.guild, ctx.author, c)
    g = GameCommands(bot := Mock(), db)
    await g.learn(g, ctx, "heal", "explosion")
    ctx.message.delete.assert_called_with()
    ctx.send.assert_called_once_with(f"**Learner** learns heal and explosion.")

