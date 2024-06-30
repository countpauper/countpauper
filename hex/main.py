import pygame
from hex_grid import HexGrid

# define a main function
def main():
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
        pygame.display.update()

if __name__=="__main__":
    main()
