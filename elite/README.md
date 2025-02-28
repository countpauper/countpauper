## Setup
1. start git bash 
1. `python -m venv .venv`
1. `source .venv/Scripts/activate`
1. `python -m pip -r requirements.txt`


### Run 

`python crop_systems.py ~/Downloads/galaxy.json.gz -r 1500`

crop columns to reduce the size of galaxy data: 
`python crop-system.py ~/Downloads/galaxe.json.gz -col id64,name,coords,population,allegiance,government,primaryEconomy,secondaryEconomy,security,controllingPower,powerState`

### Profile 

`python -m cProfile crop_systems.py ~/Downloads/galaxy_1day.json.gz`
