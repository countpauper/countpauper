import json
import sqlite3
from character import Character
from items import *
import json

class CharacterDB(object):
    properties = ['name', 'color', 'portrait', 'level', 'physical', 'mental', 'social', 'hp', 'sp', 'mp']

    def __init__(self, filename="ttrpg.db"):
        self.connection = sqlite3.connect(filename)
        self.create_table()

    def create_table(self):
        cur = self.connection.cursor()
        cur.execute(f"""CREATE TABLE IF NOT EXISTS character (Id INTEGER PRIMARY KEY, user, guild, {", ".join(self.properties)});""")
        cur.execute(f"""CREATE TABLE IF NOT EXISTS inventory (character, item, properties, location);""")
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
        idx = cur.lastrowid
        self._clear_inventory(idx)
        self._store_inventory(idx, c)
        self.connection.commit()

    def _store_inventory(self, idx, c):
        cur = self.connection.cursor()
        query_items=[]  # TODO: refactor
        for item in c.inventory:
            location = "'main'" if c.main_hand() == item else \
                 "'off'" if c.off_hand() == item else \
                 "'worn'" if c.worn == item else "NULL"
            prop_str=f"'{json.dumps(props)}'" if (props := item.properties()) else "NULL"
            query_items.append(f"""({idx}, '{type(item).__name__}', {prop_str}, {location})""")
        if not query_items:
            return

        query = f"""INSERT INTO inventory (character, item, properties, location) VALUES
        {", ".join(query_items)}"""
        cur.execute(query)

    def _clear_inventory(self, idx):
        cur = self.connection.cursor()
        query = f"""DELETE FROM inventory WHERE character=='{idx}'"""
        cur.execute(query)

    def exists(self, guild, user, name):
        return self._find_character(guild, user, name) is not None

    def retrieve(self, guild, user, name=None):
        cur = self.connection.cursor()
        name_query=f"""AND name='{name}' COLLATE NOCASE""" if name else ''
        query = f"""SELECT Id, {", ".join(self.properties)} FROM character 
            WHERE user='{user}' AND guild='{guild}' {name_query}
            ORDER BY id DESC LIMIT 1"""

        cur = cur.execute(query)
        columns = [x[0] for x in cur.description]
        if row := cur.fetchone():
            record = {col: row[idx] for idx, col in enumerate(columns)}
            c = Character(**record)
            inventory = self._retrieve_inventory(record['Id'])
            c.inventory = [i for location_items in inventory.values() for i in location_items]
            c.worn = inventory.get('worn',[None])[0]
            c.held['main'] = inventory.get('main', [None])[0]
            c.held['off'] = inventory.get('off', [None])[0]
            return c
        else:
            return None

    @staticmethod
    def _create_item(item, properties):
        if item=='Weapon':
            return Weapon(**properties)
        elif item=='RangedWeapon':
            return RangedWeapon(**properties)
        elif item=='Shield':
            return Shield(**properties)
        elif item=='Armor':
            return Armor(**properties)
        elif item=='Equipment':
            return Equipment(**properties)
        else:
            raise RuntimeError(f"Item type '{item}' unsupported.")

    def _retrieve_inventory(self, idx):
        query = f"""SELECT item, properties, location FROM inventory WHERE character=:id"""
        cur = self.connection.cursor()
        cur = cur.execute(query, dict(id=idx))
        result = dict()
        while row:=cur.fetchone():
            location=row[2]
            result[location] = result.get(location,[]) + [self._create_item(row[0], json.loads(row[1]) if row[1] else dict())]
        return result

    def delete(self, guild, user, name):
        idx = self._find_character(guild, user, name)
        if idx is None:
            raise Exception(f"There is no character named '{name}' to retire.")
        self._delete_character(idx)
        self._delete_inventory(idx)

    def _find_character(self, guild, user, name):
        cur = self.connection.cursor()
        query = f"""SELECT Id FROM character WHERE user='{user}' AND guild='{guild}' AND name='{name}' COLLATE NOCASE ORDER BY Id DESC LIMIT 1"""
        cur = cur.execute(query)
        result = cur.fetchone()
        if not result:
            return None
        return result[0]

    def _delete_character(self, idx):
        cur = self.connection.cursor()
        query = f"""DELETE FROM character WHERE Id={idx}"""
        cur = cur.execute(query)

    def _delete_inventory(self, idx):
        cur = self.connection.cursor()
        query = f"""DELETE FROM inventory WHERE character={idx}"""
        cur = cur.execute(query)
        self.connection.commit()
