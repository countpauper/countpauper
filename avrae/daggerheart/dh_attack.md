`!dh_attack <weapon> ["<experience>"] [<dice>] [adv]… [dis]… [=<difficulty>]… `

**Make an attack in Daggerheart**

The alias has the `dh_` prefix to prevent name clashes. If you only play Daggerheart you can rename it 
with `!servalias rename dh_react react`.

`weapon` - Name of the weapon to use. Partial matching is used. This is taken from your character sheet. The weapons bonus and damage die will be used.
`hope` - Change the hope die by using an `h` to separate the number of the dice and the size of the die. 
`fear` - Change the fear die by using an `f` to separate the number of dice aand the size of the die.
`experience` - Add an experience from your character sheet that contains this description. 
`dice` - Add additional dice with `<n>d<size>` or just a flat bonus. 
`adv` - Add advantage, which will add a 1d6 if the number of advantages is larger than disadvantages.
`dis` - Add a disadvantage, whill will subtract a 1d6 if the number of disadvantages is larger than advantages.
`difficultyevasion` - This is each target's difficulty or evasion.  If given damage will be rolled if the attack hits.

**Examples**

`!dh_attack mace` - Attack with a mace with a +2 Strength bonus on the sheet. It will deal `<proficiency>d8+1` damage, 
`!dh_attack longbow sharp` - Add your "Sharpshooter" experience to the roll,  if that is in your `!cvar dh_sheet`.
`!dh_attack broad adv` - Attack with your broadsword with advantage: `1d12 + 1d12 + 3 + 1d6`.
`!dh_attack dagger = "14[petty noble]"` - Attack a Petty Noble with difficulty. If hit, the `1d8+1 will also be rolled`
