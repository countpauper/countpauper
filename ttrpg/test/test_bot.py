from game_commands import GameCommands
from unittest.mock import Mock
import asyncio
import pytest

@pytest.yield_fixture()
def loop():
    # Set-up
    event_loop = asyncio.new_event_loop()
    asyncio.set_event_loop(event_loop)
    yield event_loop

    # Clean-up
    event_loop.close()

class AsyncMock(Mock):
    async def __call__(self, *args, **kwargs):
        return super(AsyncMock, self).__call__(*args, **kwargs)


def test_roll(loop):
    async def do_test():
        bot = Mock()
        g = GameCommands(bot)
        ctx = AsyncMock()
        ctx.message = AsyncMock()
        ctx.author = 'Foo'
        await g.roll(g, ctx, "1d1")
        ctx.message.delete.assert_called_with()
        ctx.send.assert_called_with('**Foo**: :game_die: 1d1 (**1**) = `1`')

    loop.run_until_complete(do_test())

