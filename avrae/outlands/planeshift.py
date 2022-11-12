embed <drac2>
g = load_json(get_gvar("69a38728-86c1-41f5-9905-3a50aaca762e"))
races=g[1:]
# parse arguments
a='''&*&'''
nl="\n"
if not a:
	return f'''-title "Plane Shift: Races" 
-desc "Plane Shift is a set of WOTC-published articles that bring content from Magic: The Gathering into the world of D&D. The following races have been approved for use on this server.
{nl.join(f"**{race.name}**" for race in races)}" -thumb "https://cdn.discordapp.com/attachments/813353462263578624/813503884029984809/vISRXkg.png" 
-footer "Use {ctx.prefix}{ctx.alias} <race> to learn about a race."'''

# match race names
b=[race for race in races if a.lower()==race.name.lower()]
if not b:
	b=[race for race in races if (a.lower() in race.name.lower() or all([x in race.name.lower() for x in a.lower().split()])) ]
if not b:
	return f'-desc "No matches found." -footer "Use {ctx.prefix}{ctx.alias} to see all available planeshift races"'
if len(b)>1:
	return f'-title "Races Containing: `{a}`" -desc "{nl.join([race.name for race in b])}" '
c=b[0]

return f'''-title "Plane Shift: Races" 
-desc "[**{c.name}**]({c.link})" 
-footer "Race | Plane Shift - {c.psar}" 
-f "Speed|{c.spd1}|inline" 
-f "Size|{c.siz1}|inline" 
-f "Ability Score Increase|{c.abil}" 
-f "Languages|{c.lang}" 
-f "Age|{c.ages}" 
-f "Alignment|{c.algn}" 
-f "Size|{c.siz2}" 
-f "Speed|{c.spd2}" '''+(f'-f "{c.fea1}" ' if "fea1" in c else '')+(f'-f "{c.fea2}" ' if "fea2" in c else '')+(f'-f "{c.fea3}" ' if "fea3" in c else '')+(f'-f "{c.fea4}" ' if "fea4" in c else '')

</drac2>