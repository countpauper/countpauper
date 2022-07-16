!alias flight <drac2>
ch=character()
ccn='Gem Flight'
if not ch.cc_exists(ccn) :
   return f'echo You don\'t have {ccn}.'
cc=ch.cc(ccn)
if not cc.value:
  return f'echo You already used {cc.name}. You need to rest.'
cc.set(0)
if (c:=combat()) and (m:=c.me):
   m.add_effect('Gem Flight',10)
return f'embed -title "{name} uses {cc.name}." -desc "{cc.desc}" -f "{cc.name}|{cc}" -thumb {image} -color {color}'
</drac2>