import re

def camel_to_words(camel):
    return re.sub(r'(?<!^)(?=[A-Z])', ' ', camel).lower()

def indefinite_article(phrase):
    if not phrase:
        raise ValueError("Empty words don't have articles.")
    # TODO: some words start with a vowel, but phonetically don't like 'a one'
    # TODO (not for this project) abreviations if the whole word is capitals or with periods
    if phrase[0].isdigit():
        return ""
    elif phrase[0].isupper():
        return ""
    if phrase[0] in 'aeiou':
        return 'an'
    else:
        return 'a'
