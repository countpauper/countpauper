from game import Game
from player import Player
import yaml

with open('world.yaml') as f:
	world_data = yaml.load(f.read(), Loader=yaml.Loader)
game=Game()
game.world.populate("World", world_data)
game.players.append(Player())
while True:
	for player in game.players:
		player.control(game.world)