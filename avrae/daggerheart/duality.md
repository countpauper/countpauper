`!duality [<hope>=1h12]… [<fear>=1f12]… [<trait>] ["<experience>"]… [<dice>] [adv]… [dis]… [=<target>/<difficulty>]…`

**Roll duality dice**
Increases `Hope` counter on hope. On critical it also  reduces the `Stress` counter.

**Parameters**

`hope` - Change the hope die by using an `h` to separate the number of the dice and the size of the die. 
`fear` - Change the fear die by using an `f` to separate the number of dice aand the size of the die.
`trait` - Use the name or part of a name of a trait to use the score from your character sheet. 
`experience` - Add an experience from your character sheet that contains this description. 
`dice` - Add additional dice with `<n>d<size>` or just a bonus 
`adv` - Add advantage, which will add a 1d6 if the number of advantages is larger than disadvantages.
`dis` - Add a disadvantage, whill will subtract a 1d6 if the number of disadvantages is larger than advantages.
`target` - This is a target value. It can be dice. If given the result will show if the target is met. 
`difficulty` - predefined target values: `trivial`=0, `very_easy`=5, `easy`=10, `average`=15, `hard`=20, `very_hard`=25, `nearly_impossible`=30

**Examples**

`!duality h20` - Use a 20 sided hope die for your Orderborne community feature. 
`!duality 1d4` - Add a d4 bonus: `1h12 + 1f12 + 1d4`.
`!duality know` - Add your knowledge trait to the roll. eg `1h12+1f12-1`.
`!duality giant` - Add your "Giant Slayer" experience to the roll, if that is in your `!cvar dh_sheet`.
`!duality adv` - Roll with advantage: `1h12 + 1f12 + 1d6`.
`!duality adv dis` - Advantage and disadvantage cancel each other out, so this is just `1h12+1f12`.
`!duality =average` - Roll the duality dice and show if they beat average difficulty .
`!duality = 2d12+2` - Roll the duality dice and shof if they meet the reaction roll of some adversary with +2 trait bonus. 
