`!dh_react [<trait>] ["<experience>"] [<dice>] [adv]… [dis]… [=<target>/<difficulty>]… `

**Roll a reaction for Daggerheart with 2d12.**

The alias has the `dh_` prefix to prevent name clashes. If you only play Daggerheart you can rename it 
with `!servalias rename dh_react react`.

`trait` - Use the name or part of a name of a trait to use the score from your character sheet. 
`experience` - Add an experience from your character sheet that contains this description. 
`dice` - Add additional dice with `<n>d<size>` or just a bonus 
`adv` - Add advantage, which will add a 1d6 if the number of advantages is larger than disadvantages.
`dis` - Add a disadvantage, whill will subtract a 1d6 if the number of disadvantages is larger than advantages.
`target` - This is a target value. It can be dice. If given the result will show if the target is met. 
`difficulty` - predefined target values: `trivial`=0, `very_easy`=5, `easy`=10, `average`=15, `hard`=20, `very_hard`=25, `nearly_impossible`=30


**Examples**

`!dh_react 1d4` - Add a d4 bonus: `1h12 + 1f12 + 1d4`.
`!dh_react AGI` - Add your agiligty trait to the roll. eg `1d12+1d12+1`.
`!dh_react alert` - Add your "Always Alert" experience to the roll if that is in your `!cvar dh_sheet`.
`!dh_react adv` - Roll with advantage: `1d12 + 1d12 + 1d6`.
`!dh_react =very_h` - Roll the reaction and show if it beats very hard difficulty.
`!duality = 18` - Roll the reaction dice and show if it meets is beats difficulty 18. 