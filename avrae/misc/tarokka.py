!alias taro embed
<drac2>
g=load_json(get_gvar('a4a3a036-49db-43f1-889d-d5cf3cf80e92'))
c=min(max(int('%1%'),1),6)-1 if '%1%'!='%1'+'%' else 0
v=["first","second","third","fourth","fifth","sixth"]
deckList = g.common if c<3 else g.high if c==3 else g.common+g.high
card = randchoice(deckList)
</drac2>
-title "You draw the {{v[c]}} card... the {{card.name}}"
-desc "{{g.drawDesc[c]}} {{card.desc}}"
-image {{f'http://i.imgur.com/{card.image}.jpg'}}
