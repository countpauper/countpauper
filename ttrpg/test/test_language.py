import pytest

from language import *

def test_camel_to_words():
    assert camel_to_words("Dromedary") == "dromedary"
    assert camel_to_words("CamelCase") == "camel case"
    assert camel_to_words("") == ""

def test_indefinite_article():
    assert indefinite_article("cheese") == 'a'
    assert indefinite_article("ogre") == 'an'
    assert indefinite_article("yeti") == 'a'
    assert indefinite_article("1 olive") == ""
    assert indefinite_article("Foo") == ""
    with pytest.raises(ValueError):
        indefinite_article("")
