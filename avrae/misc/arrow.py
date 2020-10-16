snippet
arrow < drac2 >
# Find the quivers in the bags character variable
if not bags:
	err("Arrow counting only works after setting up a Quiver with !bag.")

bag = load_json(bags)
quivers = [bagIdx for bagIdx in range(len(bag)) if bag[bagIdx][0] == "Quiver"]

# find quivers with arrows
full_quivers = {quiverIdx: bag[quiverIdx][1].get("Arrows", 0) for quiverIdx in quivers if
				bag[quiverIdx][1].get("Arrows")}
if not full_quivers:
	err("There are no arrows left in any of your quivers.")

# remove the arrow from the bag
quiver = tuple(full_quivers.keys())[0]
arrows = full_quivers[quiver]
arrows -= 1
bag[quiver][1]["Arrows"] = arrows

# remove the arrow from the dictionary to compute all remaining arrows
full_quivers[quiver] = arrows
remaining_arrows = sum(full_quivers.values())

set_cvar("bags", dump_json(bag))
< / drac2 >
-f
"Arrows Remaining" | {{remaining_arrows}}