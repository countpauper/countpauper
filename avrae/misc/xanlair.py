!alias xanlair <drac2>
        table={
                1:["2", "grell"],
                2:[None, "chuul"],
                3:["1d4", "intellect devourers"],
                4:["1d4", "veterans (intellect devourers) AC15, blindsight"],
                5:[None, "Otyugh"],
                6:[None, "Spectator"],
                7:["1d4+1", "Duergar"],
                8:["2d6", "Bandits"]
        }

        allies = ("&*& " + get("xan_allies","")).split()
        column = 0
        for a in allies:
                if a.isnumeric():
                        column = int(a)-1
        allies = [a for a in allies if not a.isnumeric()]
        allies_count=len(allies)
        allies_table=[
                ["1d8",     "1d10",    "1d12",   ],
                ["1d8",     "1d12",    "1d20",   ],
                ["1d10",    "1d20",    "2d20kh1" ],
                ["1d12",    "1d20",    "2d20kh1" ],
                ["1d20",    "2d20kh1", None      ],
                ["2d20kh1", None,      None      ]
        ]

        encounter_roll = None
        if ally_roll := (allies_table[allies_count][column] if allies_count<len(allies_table) else None):
                encounter_roll = vroll(ally_roll)
                if encounter := table.get(encounter_roll.total):
                        amount, creature = tuple(encounter)
                        return f"echo Roll {encounter_roll} : {vroll(amount) if amount else ''} {creature}."

        ally_encounter_roll = vroll("1d20")
        ally = randchoice(allies)
        table.update({8+idx: [None, a] for idx, a in enumerate(allies) if a!=ally})

        if encounter := table.get(ally_encounter_roll.total):
                amount, creature = tuple(encounter)
                return f"echo {ally} is fighting {vroll(amount) if amount else ''} {creature}."
        elif encounter_roll:
                return f'echo Roll {encounter_roll} : No Encounter'
        else:
                return f'echo {ally} already cleared this area'                
</drac2>