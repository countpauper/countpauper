import sqlite3
from character import Character

class CharacterDB(object):
    properties = ['name', 'color', 'portrait', 'level', 'physical', 'mental', 'social', 'hp', 'sp', 'mp']

    def __init__(self, filename="ttrpg.db"):
        self.connection = sqlite3.connect(filename)
        self.create_table()

    def create_table(self):
        cur = self.connection.cursor()
        cur.execute(f"""CREATE TABLE IF NOT EXISTS character (Id INTEGER PRIMARY KEY, user, guild, {", ".join(self.properties)});""")
        cur.execute(f"""CREATE TABLE IF NOT EXISTS inventory (character, item, properties, location);""")
        # cur.execute("CREATE TABLE IF NOT EXISTS inventory(character, item, location)") # location main(hand), off(hand), worn, bag
        # cur.execute("CREATE TABLE IF NOT EXISTS inventory(character, skill)")

    @staticmethod
    def _format_column(v):
        if type(v) == str:
            return f"'{v}'"
        else:
            return str(v)

    # TODO: move database character link to another file
    def store(self, guild, user, c):
        # TODO allow GMs (anyone) to have multiple characters. Create with name. Create with template (monster) sheet (import json).
        # make a separate character database where unique character Ids are stored and linked to the owner and to the rest
        cur = self.connection.cursor()
        query = f"""REPLACE INTO character(user, guild, {", ".join(self.properties)}) VALUES 
            ('{user}', '{guild}', {", ".join(self._format_column(c.get(p)) for p in self.properties)});"""
        cur.execute(query)
        idx=cur.lastrowid
        query = f"""DELETE FROM inventory WHERE character=='{idx}'"""
        cur.execute(query)
        if c.inventory:
            query = f"""INSERT INTO inventory (character, item, properties, location) VALUES\n"""
            query_items=[]  # TODO: refactor
            for item in c.inventory:
                location = 'main' if c.main_hand() == item else \
                     'off' if c.off_hand() == item else \
                     'worn' if c.worn == item else None
                query_items.append(f"""({idx}, '{type(item).__name__}', '{item.name}', '{location}')""")
            query += ",\n".join(query_items)
            cur.execute(query)
        self.connection.commit()

    def retrieve(self, guild, user, name=None):
        cur = self.connection.cursor()
        name_query=f"""AND name='{name}' COLLATE NOCASE""" if name else ''
        query = f"""SELECT {", ".join(self.properties)} FROM character 
            WHERE user='{user}' AND guild='{guild}' {name_query}
            ORDER BY id DESC LIMIT 1"""

        res = cur.execute(query)
        columns = [x[0] for x in cur.description]
        if row := res.fetchone():
            record = {col: row[idx] for idx,col in enumerate(columns)}
            return Character(**record)
        else:
            return None

