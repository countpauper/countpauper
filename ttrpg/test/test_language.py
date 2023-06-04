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
    assert indefinite_article(RuntimeError("error")) == 'an'
    with pytest.raises(ValueError):
        indefinite_article("")


def test_plural_amount():
    assert plural(1, 'cheese') == 'cheese'
    assert plural(2, 'cheese') == 'cheeses'
    assert plural(0, 'cheese') == 'cheeses'
    assert plural(-1, 'cheese') == 'cheese'
    assert plural(-2, 'cheese') == 'cheeses'


def test_plural_postfix():
    assert plural(2, 'bow') == 'bows'
    assert plural(2, 'x') == "x's"
    assert plural(2, 'kiss') == 'kisses'
    assert plural(2, 'fish') == 'fishes'
    assert plural(2, 'fez') == 'fezes'
    assert plural(2, 'ax') == 'axes'
    assert plural(2, 'cherry') == 'cherries'
    assert plural(2, 'monkey') == 'monkeys'
    assert plural(2, 'hero') == 'heroes'
    assert plural(2, 'pistachio') == 'pistachios'
    assert plural(2, 'knife') == 'knives'
    assert plural(2, 'wolf') == 'wolves'
    assert plural(2, 'stiff') == 'stiffs'
    assert plural(2, 'crisis') == 'crises'
    # TODO: exceptions. Words that don't change (sheep, moose) totally irregular (foot, child, die)


def test_list_off():
    assert list_off() == ""
    assert list_off(["cheese"]) == "cheese"
    assert list_off(("pickles", "cheese")) == "pickles and cheese"
    assert list_off(("olives", "pickles", "cheese")) == "olives, pickles and cheese"
    assert list_off(("olives", "pickles", "ham and cheese")) == "olives, pickles, and ham and cheese"
    assert list_off(("olives", "two pickles", "cheese")) == "olives, two pickles, and cheese"