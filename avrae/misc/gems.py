!alias gems tembed 
<drac2>
gp = (10*2**randint(16))
data = load_json(get_gvar("228489aa-20bb-4568-83e3-4b5bb0b43ad9"))
gems, qualities, sizes = data["gems"], data["qualities"], data["sizes"]
gem = list(gems.keys())[randint(len(gems))]
size = list(sizes.keys())[randint(len(sizes))]
quality  = list(qualities.keys())[randint(len(sizes))] if randint(4)==0 else "%s"
ret = size % (quality % gem)
</drac2> 
-f Gems|"{{ ret }}" -f Gold|"{{gp}} gp"