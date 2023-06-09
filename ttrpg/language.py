import re

def camel_to_words(camel):
    return re.sub(r'(?<!^)(?=[A-Z])', ' ', camel).lower()

def indefinite_article(phrase):
    if not phrase:
        raise ValueError("Empty words don't have articles.")
    phrase = str(phrase)
    # TODO: some words start with a vowel, but phonetically don't like 'a one'
    # TODO (not for this project) abbreviations if the whole word is capitals or with periods
    if phrase[0].isdigit():
        return ""
    elif phrase[0].isupper():
        return ""
    if phrase[0] in 'aeiou':
        return 'an'
    else:
        return 'a'


latin_sufixes = {'us': 'i', 'is': 'es', 'ex': 'ices', 'ix': 'ices', 'um': 'a'}

def plural(amount, phrase):
    amount=int(amount)
    phrase = str(phrase)
    if abs(amount) == 1:
        return phrase
    if len(phrase) == 1:
        return phrase+"'s"
    if any(phrase.endswith(suffix) for suffix in latin_sufixes):
        return phrase[:-2]+latin_sufixes[phrase[-2:]]
    if phrase.endswith('us'):
        return phrase[:-2]+'i'
    elif phrase.endswith('is'):
        return phrase[:-2]+'es'
    elif any(phrase.endswith(suffix) for suffix in ('s', 'x', 'sh', 'ch', 'z')):
        return phrase+'es'
    elif phrase.endswith('y'):
        if phrase[-2] in 'aeiou':
            return phrase+'s'
        else:
            return phrase[:-1]+'ies'
    elif phrase.endswith('o'):
        if phrase[-2] in 'aeiou':
            return phrase + 's'
        else:
            return phrase+'es'
    elif phrase.endswith('f') and phrase[-2]!='f':
        return phrase[:-1]+'ves'
    elif phrase.endswith('fe'):
        return phrase[:-2]+'ves'
    else:
        return phrase+'s'


def list_off(items=None):
    if not items:
        return ""
    items = tuple(items)
    if len(items) == 1:
        return str(items[0])
    else:
        oxford = ',' if 'and' in str(items[-1]) or ' ' in str(items[-2]) else ''
        return ", ".join(str(i) for i in items[:-1])+f"{oxford} and {items[-1]}"

irregular_possessive=dict(i="my", you="your", he="his", she="her", we="ours", they="their")
def possessive(owner):
    regular_possessive=f"{owner}'" if owner[-1] == "s" else f"{owner}'s"
    possessive = irregular_possessive.get(owner.lower(), regular_possessive)
    if owner[0].isupper() and owner != "I":
        return possessive.capitalize()
    else:
        return possessive
