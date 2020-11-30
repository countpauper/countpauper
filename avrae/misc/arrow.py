<drac2>
# Find the quivers in the bags character variable
if not exists('bags'):
	err("Arrow counting only works after setting up a Quiver with `!bag $ Quiver`.")

arrow_names=['Arrows','Arrow','arrows','arrow']
bag = load_json(bags)
quivers = [(b[1],sum([b[1].get(n,0) for n in arrow_names])) for b in bag if b[0].lower() == "quiver"]
if not quivers:
	err("You do not own a quiver. Where are you pulling that arrow from?")

quivers = [(b,a) for (b,a) in quivers if a>0]
total_remaining = sum([q for (_,q) in quivers])
if total_remaining<=0:
	err("There are no arrows left in any of your quivers.")

# remove the arrow or arrows from the first quiver
quiver = quivers[0][0]
for arrow_name in arrow_names:
	if remaining:=quiver.get(arrow_name,0):
		remaining-=1
		total_remaining-=1
		if remaining:
			quiver[arrow_name]=remaining
		else:
			quiver.pop(arrow_name)
		break
set_cvar("bags", dump_json(bag))
</drac2>
-f "Arrows Remaining"|{{total_remaining}}