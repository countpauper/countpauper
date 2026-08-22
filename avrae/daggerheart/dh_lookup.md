`!dh_lookup <term to find>`

**Use or show a feature**

Lookup information on: 
- classes (TODO)
- sub classes (TODO)
- ancestries (TODO)
- communities (TODO)
- class features
- heritages features 
- domain cards 

The search term can be a partial name of any of these. If a matching term is found the information will be displayed in an embed. No quotes are needed.

**Database Override**
The database by default contains all SRD domain cards in `!gvar 7a5a3d45-7ad0-4db9-9c9a-dc37294a216e` and class and heritage features in `!gvar 1eeba3a1-1fd9-4b72-8713-d6475ccfc204`. It contains class information in `!gvar 0342d22f-af8d-44d0-811a-5f1ec8049248.gvar`.
Cards can be added or overwritten for homebrew by creating gvars and listing them in the `!svar dh_cards` or `!uvar dh_cards`. These variables themselves are YAML lists (i.e. comma separated) and the gvars that they refer to should be the same JSON format as the SRD ones above.
Similarly classes and their sub classes can be added or overwritten using gvars in `!svar dh_classes` or `!uvar dh_classes`. To extend or modify, first make a copy of the original json object.