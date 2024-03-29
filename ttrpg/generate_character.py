from character import Character
from items import *
from skills import Skill
import random
from ability import abilities

sizes=dict(
    xs=dict(name='Extra Small', physical=2, weight=1),
    s=dict(name='Small', physical=3, weight=2),
    m=dict(name='Medium', physical=5, weight=3),
    l=dict(name='Large', physical=8, weight=5),
    xl=dict(name='Extra Large', physical=13, weight=8),
    xxl=dict(name='Extra Extra Large', physical=21, weight=13)) # TODO already out of dice range

races = [dict(name='Imp', size=sizes['xs'], social=4, mental=6), # physical 2 = 12
         dict(name='Pixie', size=sizes['xs'], social=4, mental=6), # physical 2 = 12
         dict(name='Halfling', size=sizes['s'], social=6, mental=3), # + physical 3 = 12
         dict(name='Gnome', size=sizes['s'], social=3, mental=6),   # +physical 3 = 12
         dict(name='Dwarf', size=sizes['s'], physical=6, mental=4, social=2), # 12
         dict(name='Orc', size=sizes['m'], physical=7, mental=2, social=3), # 12
         dict(name='Elf', size=sizes['m'], mental=5, social=4, physical=3), # 12
         dict(name='Satyr', size=sizes['m'], mental=3, social=4), # physical5 = 12
         dict(name='Human', size=sizes['m'], social=4, mental=3), # physical 5 = 12
         dict(name='Ogre', size=sizes['l'], social=2, mental=2)] # physical 8 = 12


def random_equipment(capacity):
    # TODO take enchantment level (which is like (level-4)//2 or sommit) and generate enchanted weapons, armors and jewelry
    if capacity < 2:
        weapon = random.choice([MeleeWeapon(), RangedWeapon(), None])
    else:
        weapon = random.choice([MeleeWeapon(), MeleeWeapon(heavy=True), RangedWeapon(), RangedWeapon(heavy=True), None])
    capacity -= weapon.weight() if weapon else 0
    if capacity and (not weapon or weapon.hands() == 1):
        offhand = random.choice([MeleeWeapon() if weapon else None, Shield(), None])
        capacity -= offhand.weight() if offhand else 0
    else:
        offhand = None

    if capacity and (armor_rating := random.randint(0, min(capacity, 3))):
        armor = Armor(rating=armor_rating)
        capacity -= armor.weight()
    else:
        armor = None
    return [i for i in (weapon, offhand, armor) if i]


def random_skills(character):
    ability_skill_amounts = { ability:max(0, character.ability(ability) - 2) for ability in abilities }
    skills = [s for s in Skill.all if character.approve_cost(s.cost)]
    ability_skills={ability : [s for s in skills if s.ability==ability] for ability in ability_skill_amounts.keys()}
    selected_skills = []
    for ability, skills in ability_skills.items():
        random.shuffle(skills)
        selected_skills += skills[:ability_skill_amounts.get(ability, 0)]
    random.shuffle(selected_skills)
    return [s(character) for s in selected_skills[:character.level]]

def _level_up(stats, level=None):
    if level is None:
        level = stats['level']+1
    available = (6+level) - sum(stats[ability.name] for ability in abilities)
    result = stats.copy()
    while available > 0:
        ability = random.choice(abilities)
        boost = available if available == 1 else random.randrange(1, available)
        available -= boost
        result[ability.name] += boost

    # TODO: random skills and magic items if allowed. No magical cloak bears please
    return result


def random_character(level=1):
    stats = random.choice(races)

    stats['physical'] = stats.get('physical', stats['size']['physical'])
    stats['weight'] = stats['size']['weight']
    stats['level'] = level
    stats = _level_up(stats, level)
    c = Character(**stats)
    c.obtain(*random_equipment(c.capacity()))
    c.skill=random_skills(c)
    c.auto_equip()
    return c



def random_monster(level):
    total_ability = 6 + level
    if level<-3:
        raise ValueError("Minimum monster level is -3")
    size = random.choice([s for s in sizes.values() if s['physical'] <= total_ability])
    stats=dict(name=f'{size["name"]} Monster',
               physical = size['physical'],
               level = level,
               social = 0,
               mental = 0,
               weight = size['weight'])
    stats = _level_up(stats, level)
    return Character(**stats)
