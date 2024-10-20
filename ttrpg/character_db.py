from character import Character
from items import ItemFactory
from effect import Effect
from errors import CharacterUnknownError
import sqlite3
import json

class CharacterDB(object):
    persistent_stats = ['name', 'color', 'portrait', 'level', 'physical', 'mental', 'social', 'hp', 'pp', 'mp', 'ap']

    def __init__(self, filename="ttrpg.db"):
        self.connection = sqlite3.connect(filename)
        self._create_table()
        self._cache = dict()

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
            CREATE TABLE IF NOT EXISTS character (id INTEGER PRIMARY KEY, user, guild, master, {", ".join(self.persistent_stats)});
            CREATE TABLE IF NOT EXISTS inventory (character, item, properties, location);
            CREATE TABLE IF NOT EXISTS skills (character, skill);
            CREATE TABLE IF NOT EXISTS effects (character, effect, duration, parameters);
            COMMIT;""")


    def store(self, guild, user, c, master=None):
        with self.connection as con:
            columns=dict(user=str(user), guild=str(guild))
            columns.update({stat:c[stat] for stat in self.persistent_stats})
            columns['id'] = c.id
            columns['master'] = master
            query = f"""REPLACE INTO character ({", ".join(columns)}) VALUES 
                ({", ".join(f":{p}" for p in columns)});"""
            cur = con.execute(query, columns)
            c.id = cur.lastrowid
            cur = self._clear_inventory(cur, c.id)
            cur = self._store_inventory(cur, c.id, c)
            cur = self._clear_skills(cur, c.id)
            cur = self._store_skills(cur, c.id, c.skill)
            cur = self._clear_effects(cur, c.id)
            cur = self._store_effects(cur, c.id, c.effects)
            for ally in c.allies:
                self.store(guild, user, ally, master=c.id)
            ally_ids = [ally.id for ally in c.allies]
            cur = self._clear_allies(cur, c.id, ally_ids)
            self._cache[c.id] = c

    @staticmethod
    def _encode_location(item, c):
        if c.main_hand() == item:
            return "main"
        elif c.off_hand() == item:
            return "off"
        elif item in c.worn:
            return "worn"
        else:
            return None

    def _clear(self, cursor, table, idx):
        return cursor.execute(f"""DELETE FROM {table} WHERE character==?""", [idx])

    def _clear_inventory(self, cursor, idx):
        return self._clear(cursor, "inventory", idx)

    def _store_inventory(self, cursor, idx, c):
        item_values = [dict(item=type(item).__name__,
                             properties=json.dumps(props) if (props := item.properties()) else None,
                             location = self._encode_location(item, c)) for item in c.inventory]
        if not item_values:
            return cursor
        query = f"""INSERT INTO inventory (character, item, properties, location) VALUES 
                ({int(idx)}, :item, :properties, :location)"""
        return cursor.executemany(query, item_values)


    def _clear_skills(self, cursor, idx):
        return self._clear(cursor, "skills", idx)

    def _store_skills(self, cursor, idx, skills):
        skill_values = [dict(skill=str(skill)) for skill in skills]
        if not skill_values:
            return cursor
        query = f"""INSERT INTO skills (character, skill) VALUES 
                ({int(idx)}, :skill)"""
        return cursor.executemany(query, skill_values)

    def _clear_effects(self, cursor, idx):
        return self._clear(cursor, "effects", idx)

    def _store_effects(self, cursor, idx, effects):
        effect_values = [dict(effect=e.name, duration = e.duration, parameters=json.dumps(e.boni)) for e in effects]
        if not effect_values:
            return cursor
        query = f"""INSERT INTO effects (character, effect, duration, parameters) VALUES 
                ({int(idx)}, :effect, :duration, :parameters)"""
        return cursor.executemany(query, effect_values)

    def _clear_allies(self, cursor, idx, remaining=[]):
        remaining_clause=f""" AND id NOT IN ({','.join(['?']*len(remaining))})""" if remaining else ""
        return cursor.execute(f"""DELETE FROM character WHERE master=? {remaining_clause}""", [idx]+remaining)

    def exists(self, guild, user, name):
            return self._find_character(guild, user, name) is not None

    def retrieve(self, guild, user, identity=None):
        if user is None and identity is None:
            raise CharacterUnknownError(guild, user, identity)
        if identity is None:
            id_query = "AND master IS NULL"
        elif isinstance(identity, int):
            id_query=f"""AND id=:identity"""
        elif identity is not None:
            id_query=f"""AND name=:identity COLLATE NOCASE"""
            identity = str(identity)
        user_query=f"""AND user=:user""" if user is not None else ''

        query = f"""SELECT id, user, {", ".join(self.persistent_stats)} FROM character 
            WHERE guild=:guild {user_query} {id_query}
            ORDER BY id DESC LIMIT 1"""

        cursor = self.connection.execute(query, dict(user=str(user), guild=str(guild), identity=identity))
        columns = [x[0] for x in cursor.description]
        if row := cursor.fetchone():
            record = {col: row[nr] for nr, col in enumerate(columns)}
            idx = record.get('id')
            if c := self._cache.get(idx):
                return c
            record['skill'] = self._retrieve_skills(cursor, idx)
            inventory = self._retrieve_inventory(cursor, idx)
            record['inventory'] = [i for location_items in inventory.values() for i in location_items]
            record['allies'] = self._retrieve_allies(cursor, idx)
            c = Character(**record)
            c.effects = self._retrieve_effects(cursor, c.id)
            c.worn = inventory.get('worn', [])
            c.held['main'] = inventory.get('main', [None])[0]
            c.held['off'] = inventory.get('off', [None])[0]
            self._cache[idx] = c
            return c
        else:
            return None

    def _retrieve_inventory(self, cursor, idx):
        query = f"""SELECT item, properties, location FROM inventory WHERE character=:id"""
        response = cursor.execute(query, dict(id=idx))
        result = dict()
        while row:=response.fetchone():
            location = row[2]
            result[location] = result.get(location,[]) + [ItemFactory(row[0], json.loads(row[1]) if row[1] else dict())]
        return result

    def _retrieve_skills(self, cursor, idx):
        query = f"""SELECT skill FROM skills WHERE character=:id"""
        response = cursor.execute(query, dict(id=idx))
        return [row[0] for row in response.fetchall()]

    def _retrieve_allies(self, cursor, idx):
        query = f"""SELECT guild, user, id FROM character WHERE master=:id"""
        response = cursor.execute(query, dict(id=idx))
        return [self.retrieve(row[0], row[1], row[2]) for row in response.fetchall()]

    def _retrieve_effects(self, cursor, idx):
        query = f"""SELECT effect, duration, parameters FROM effects WHERE character=:id"""
        response = cursor.execute(query, dict(id=idx))
        effects = list()
        while row:=response.fetchone():
            effects.append(Effect(row[0], duration=row[1], boni=json.loads(row[2])))
        return effects

    def _find_character(self, guild, user, name):
        user_query = f"""AND user=:user""" if user is not None else ''
        query = f"""SELECT Id FROM character WHERE guild=:guild {user_query} AND name=:name COLLATE NOCASE ORDER BY Id DESC LIMIT 1"""
        response = self.connection.execute(query, dict(user=str(user), guild=str(guild), name=str(name)))
        if result := response.fetchone():
            return result[0]
        else:
            return None

    def _delete_character(self, cursor, idx):
        return cursor.execute(f"""DELETE FROM character WHERE Id=?""", [idx])

    def user(self, guild, name):
        query = f"""SELECT user FROM character WHERE guild=:guild AND name=:name COLLATE NOCASE ORDER BY Id DESC LIMIT 1"""
        response = self.connection.execute(query, dict(guild=str(guild), name=str(name)))
        if result := response.fetchone():
            return result[0]
        else:
            return None

    def delete(self, guild, user, name):
        idx = self._find_character(guild, user, name)
        if idx is None:
            raise CharacterUnknownError(guild, user, name)
        with self.connection as con:
            cursor = self._delete_character(con.cursor(), idx)
            cursor = self._clear_inventory(cursor, idx)
            cursor = self._clear_skills(cursor, idx)
            cursor = self._clear_effects(cursor, idx)
            cursor = self._clear_allies(cursor, idx)
        return idx

