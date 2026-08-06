`!duality [<hope>=1h12]… [<fear>=1f12]… [<trait>] ["<experience>"]… [<dice>] [adv]… [dis]…`

Roll duality dice. Increase hope counter on hope and critical, reduces stress counter on critical.

**Parameters**

`hope` - Change the hope die by using an `h` to separate the number of the dice and the size of the die. 
`fear` - Change the fear die by using an `f` to separate the number of dice aand the size of the die.
`trait` - Use the name or part of a name of a trait to use the score from your character sheet. 
`experience` - Add an experience from your character sheet that contains this description. 
`dice` - Add additional dice with `<n>d<size>` or just a bonus 
`adv` - Add advantage, which will add a 1d6 if the number of advantages is larger than disadvantages.
`dis` - Add a disadvantage, whill will subtract a 1d6 if the number of disadvantages is larger than advantages.

**Examples**

`!duality h20` - Use a 20 sides hope die for your Orderborne cummunity feature. 
`!duality 1d4` - Add a d4 bonus: `1h12 + 1f12 + 1d4`
`!duality know` - Add your knowledge trait to the roll. eg `1h12+1f12-1`
`!duality giant` - Add your "Giant Slayer" experience to the roll if that is in your `!cvar dh_sheet`
`!duality adv` - Roll with advantage: `1h12 + 1f12 + 1d6`
`!duality adv dis` - Advantage and disadvantage cancel each other out, so this is just `1h12+1f12`
