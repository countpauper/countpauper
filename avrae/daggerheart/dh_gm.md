`!dh_dm [<dice>] [adv]… [dis]… [=<target>/<difficulty>]… `

**Roll a 1d20 Daggerheart GM dice.**

The alias has the `dh_` prefix to prevent name clashes. If you only play Daggerheart you can rename it 
with `!servalias rename dh_gm gm`.

`dice` - Add additional dice with `<n>d<size>` or just a bonus 
`adv` - Add advantage, which will add a 1d6 if the number of advantages is larger than disadvantages.
`dis` - Add a disadvantage, whill will subtract a 1d6 if the number of disadvantages is larger than advantages.
`target` - This is a target value. It can be dice. If given the result will show if the target is met. 
`difficulty` - predefined target values: `trivial`=0, `very_easy`=5, `easy`=10, `average`=15, `hard`=20, `very_hard`=25, `nearly_impossible`=30

**Examples**
`!dh_gm 1d4` - Add a d4 bonus: `1d20 + 1d4`.
`!dh_gm dis` - Roll with disadvantage: `1d20 - 1d6`.
`!dh_gm =very_eas` - Roll the GM die and show if it beats very easy difficulty.
`!dh_gm = 18` - Roll the GM die and show if it meets is beats difficulty 18. 