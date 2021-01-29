<drac2>
# TODO gvar
languages={
	"common":	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz",
	"elvish":	"αβγδεζθηιjκλμνοπςρστυφώχψωαβγδεζθηιjκλμνοπςρστυφώχψω"
}
arg="&*&"

language=arg.split(maxsplit=1)
if language[0].lower() in languages.keys():
	source=languages['common']
	target=languages[language[0]]
	text=language[1]
else:
	text=arg
	target=languages['common']
	# auto recognize source
	source=None
	for language,code in languages.items():
		if text[0] in code:
			source=code
			break

# TODO check language proficiencies

translation=text.translate(''.maketrans(source,target))

return f'echo {translation}'
</drac2>
