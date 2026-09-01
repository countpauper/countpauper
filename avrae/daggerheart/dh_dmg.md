`!dh_dmg <dice> [armor]` 

**Damage yourself** 
An amount of damage (can be an amount that was already rolled, for example by `!dh_atk`). The damage is compared with your thresholds and the resulting amount of injury is applied to your hitpoints.

`<dice>` - The amount of damage 
`armor` - Whether to use an armor slot to reduce the damage (if any). Can be used more than once.

**Examples**

`!dh_dmg 10` - Deal yourself 10 damage and if your major threshold is 12 this will subtract 1 hitpoint 
`!dh_dmg 1d6-10` Deal yourself 1d6-10 damage. If the total is 0 or below no hitpoints are subtracted 
`!dh_dmg 28 armor` - Deal yourself 28 damage. If your severe damage threshold is 24 and you have at least one armor, an armor slot is used and you only lose two hitpoints. 
`!dh_dmg 100` - If configured in your sheet and this exceeds your massive damage threshold, this may deal four hitpoints of damage. If you only have three left you must do a death move. 

