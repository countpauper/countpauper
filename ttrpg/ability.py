class Ability(object):
    pass

class Physical(Ability):
    """Your physical ability describes your athletic prowess. Your strength, speed, and agility.
    Besides your talent in physical skill, it allows you fight better, carry more."""
    name = 'physical'

class Mental(Ability):
    """Your mental ability describes your intellectual prowess. Your memory, your creativity and your problem solving.
    Besides your talent for magical skill, it allows you to remember more lore and languages.
    The maximum amount of power points you have is also equal to your mental ability."""
    name = 'mental'

class Social(Ability):
    """Your social ability described your skill in working with others. Your team spirit, your motivation, your charisma, and your courage.
    Besides your talent for summoning, it allows you to motivate others with love or fear.
    The maximum amount of morale points you can have is also equal to your social ability."""
    name = 'social'

abilities = (Physical, Mental, Social)
