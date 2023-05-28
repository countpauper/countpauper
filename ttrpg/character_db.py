import json
import sqlite3
from character import Character
from items import *
import json

class CharacterDB(object):
    properties = ['name', 'color', 'portrait', 'level', 'physical', 'mental', 'social', 'hp', 'sp', 'mp']

    def __init__(self, filename="ttrpg.db"):
        self.connection = sqlite3.connect(filename)
        self._create_table()

    def close(self):
        if self.connection:
            self.connection.close()
        self.connection = None

    def __del__(self):
        self.close()

    def _create_table(self):
        cur = self.connection.cursor()
        cur.executescript(f"""
            BEGIN;
            CREATE TABLE IF NOT EXISTS character (Id INTEGER PRIMARY KEY, user, guild, {", ".join(self.properties)});
            CREATE TABLE IF NOT EXISTS inventory (character, item, properties, location);
            COMMIT;""")

    # TODO: move database character link to another file
    def store(self, guild, user, c):
        with self.connection as con:
            properties = {p: c.get(p) for p in self.properties}
            query = f"""REPLACE INTO character(user, guild, {", ".join(self.properties)}) VALUES 
                (:user, :guild, {", ".join(f":{p}" for p in properties.keys())});"""
            properties.update(dict(user=user, guild=guild))
            cur = con.execute(query, properties)
            idx = cur.lastrowid
            cur = self._clear_inventory(cur, idx)
            self._store_inventory(cur, idx, c)

    @staticmethod
    def _encode_location(item, c):
        if c.main_hand() == item:
            return "main"
        elif c.off_hand() == item:
            return "off"
        elif c.worn == item:
            return "worn"
        else:
            return None

    def _store_inventory(self, cursor, idx, c):
        cursor = self.connection.cursor()
        item_columns = [dict(item=type(item).__name__,
                             properties=json.dumps(props) if (props := item.properties()) else None,
                             location = self._encode_location(item, c)) for item in c.inventory]
        if not item_columns:
            return
        query = f"""INSERT INTO inventory (character, item, properties, location) VALUES 
                ({int(idx)}, :item, :properties, :location)"""
        return cursor.executemany(query, item_columns)

    def _clear_inventory(self, cursor, idx):
        return cursor.execute(f"""DELETE FROM inventory WHERE character==?""", [idx])

    def exists(self, guild, user, name):
        return self._find_character(guild, user, name) is not None

    def retrieve(self, guild, user, name=None):
        name_query=f"""AND name=:name COLLATE NOCASE""" if name else ''
        query = f"""SELECT Id, {", ".join(self.properties)} FROM character 
            WHERE user=:user AND guild=:guild {name_query}
            ORDER BY id DESC LIMIT 1"""

        cursor = self.connection.execute(query, dict(user=user, guild=guild, name=name))
        columns = [x[0] for x in cursor.description]
        if row := cursor.fetchone():
            record = {col: row[idx] for idx, col in enumerate(columns)}
            c = Character(**record)
            inventory = self._retrieve_inventory(cursor, record['Id'])

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

    def _retrieve_inventory(self, cursor, idx):
        query = f"""SELECT item, properties, location FROM inventory WHERE character=:id"""
        response = cursor.execute(query, dict(id=idx))
        result = dict()
        while row:=response.fetchone():
            location = row[2]
            result[location] = result.get(location,[]) + [self._create_item(row[0], json.loads(row[1]) if row[1] else dict())]
        return result

    def delete(self, guild, user, name):
        idx = self._find_character(guild, user, name)
        if idx is None:
            raise RuntimeError(f"There is no character named '{name}' to retire.")
        with self.connection as con:
            cursor = self._delete_character(con.cursor(), idx)
            self._delete_inventory(cursor, idx)
        return idx

    def _find_character(self, guild, user, name):
        query = f"""SELECT Id FROM character WHERE user=:user AND guild=:guild AND name=:name COLLATE NOCASE ORDER BY Id DESC LIMIT 1"""
        response = self.connection.execute(query, dict(user=user, guild=guild, name=name))
        result = response.fetchone()
        if not result:
            return None
        return result[0]

    def _delete_character(self, cursor, idx):
        return cursor.execute(f"""DELETE FROM character WHERE Id=?""", [idx])

    def _delete_inventory(self, cursor, idx):
        return cursor.execute(f"""DELETE FROM inventory WHERE character=?""", [idx])
