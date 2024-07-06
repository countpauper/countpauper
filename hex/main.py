import pygame
from hex_grid import HexGrid

# define a main function
def main():
    # TODO: click gives hex coordinate 
    # TODO: argparser: background image and size
    # TODO: json or something contains name, description, random tables and so on (hue) at hex
    # TODO: fog of war ? 

    pygame.init()
    icon = pygame.image.load("hex.jpg")
    pygame.display.set_icon(icon)
    pygame.display.set_caption("Hex")

    screen = pygame.display.set_mode((1024,768)) 
    background_color = (100,50,0)
    screen.fill(background_color)

    white = (255,255,255)
    grid = HexGrid((4,3), False)
    grid.draw(screen, white)

    while True:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                exit(0)
            elif event.type == pygame.MOUSEBUTTONUP:
                pos = pygame.mouse.get_pos()
                scale = grid.scale(screen)
                hex_pos = pygame.Vector2(pos[0]/scale, pos[1]/scale)
                hex_coord = grid.pick(hex_pos)
                print(f"Click at {pos} = {hex_pos}, {hex_coord}") 
        pygame.display.update()

if __name__=="__main__":
    main()
