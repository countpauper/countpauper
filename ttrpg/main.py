import d20 as d20
from character import Character

# This is a sample Python
# script.

# Press Shift+F10 to execute it or replace it with your code.
# Press Double Shift to search everywhere for classes, files, tool windows, actions, and settings.



# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    c = Character.random_character()
    print(c)
    m = Character().random_monster(1)
    print(m)

    while c.alive() and m.alive():
        if c.active():
            print(c.attack(m, 0))
            print(c.attack(m, 1))
            print(c.attack(m, 2))
        if m.active():
            print(m.attack(c, 0))
            print(m.attack(c, 1))
            print(m.attack(c, 2))

# See PyCharm help at https://www.jetbrains.com/help/pycharm/
