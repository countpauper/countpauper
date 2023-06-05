import pytest
from character_db import CharacterDB
import os
from dice import Dice

@pytest.fixture
def db():
    test_name = os.environ.get('PYTEST_CURRENT_TEST').split(':')[-1].split(' ')[0]
    fn = f"{test_name}.db"
    char_db = CharacterDB(fn)

    yield char_db

    char_db.close()
    del char_db
    os.remove(fn)

@pytest.fixture
def dice_min():
    original = Dice.fix('min')
    yield
    Dice.fix(original)

@pytest.fixture
def dice_max():
    original = Dice.fix('max')
    yield
    Dice.fix(original)
