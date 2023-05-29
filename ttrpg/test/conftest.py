import pytest
from character_db import CharacterDB
import os

@pytest.fixture
def db():
    test_name = os.environ.get('PYTEST_CURRENT_TEST').split(':')[-1].split(' ')[0]
    fn = f"{test_name}.db"
    char_db = CharacterDB(fn)

    yield char_db

    char_db.close()
    del char_db
    os.remove(fn)
