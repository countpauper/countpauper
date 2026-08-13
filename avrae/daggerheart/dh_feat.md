`!dh_feat "<name>"` 

**Use or show a feature**

The feature can be a domain, class, subclass, or heritage feature. The feature database by default contains all SRD domain features in `!gvar 7a5a3d45-7ad0-4db9-9c9a-dc37294a216e` and class and heritage features in `!gvar 1eeba3a1-1fd9-4b72-8713-d6475ccfc204`
Features can be added or overwritten for homebrew by creating gvars and listing them in the `!svar dh_features` or `!uvar dh_features`. These variables themselves are YAML lists (i.e. comma separated) and the gvars that they refer to should be the same JSON format as the SRD ones above.
