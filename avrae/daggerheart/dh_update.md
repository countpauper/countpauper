`!dh_update [-exp[<n>] "Experience[=<score>]"]…`

**Synchronize your Daggerheart sheet with your imported aheet** 

The `!import` command does not import everything you need to play Daggerheart with the workshop aliases. 
This alias can be used to synchronize and set the remaining character details. 

It creates counters that don't exist or updates them if they do: 
- Hope 
- Stress, according to your maximum stress score, on your sheet (H38=Z12)
- Armor, according to the maximum armor score on your sheet (V12)

Experience descriptions are not imported, so they must be described with this alias. These descriptions can be used when rolling `!duality`, `!dh_attack` or `!dh_react`.  
- `-exp "<Experience>"` Add an <Experience>. These will be added to your list of experience by order in your sheet. You can add two after character creation and one every tier. 
- `-exp<n> "<Experience>"` - Name or rename a specific experience by index (starting at 1) in your character sheet. 
To disable an experience, set the name to "". This will not change the assignment or score of any other experiences. 

**Examples**
`!dh_update -exp "Dance in the moonlight" -exp "Loves animals"` - Adds two experiences which will use the first 2 scores in the sheet
`!dh_update -exp2 "Loves cute animals"` - Rename one of the experiences. 
`!dh_update -exp2 "Loves animals=3"` -Set the score of Loves animals to 3 instead of mapping it to the 2nd score in the sheet.  
