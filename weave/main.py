import pygame
import argparse

def resize_font(height):
    return pygame.font.SysFont('Arial Rounded MT Bold', int(height*0.8)), int(height*0.1)

parser = argparse.ArgumentParser()
parser.add_argument("file", type=argparse.FileType('r'), default="lines.txt", nargs='?')
args = parser.parse_args()

lines = args.file.readlines()
args.file.close()
lines = [line.strip() for line in lines]

pygame.init()
screen = pygame.display.set_mode((800, 600), pygame.RESIZABLE)
height = screen.get_size()[1]
GAME_FONT, offset = resize_font(screen.get_size()[1])
running =  True

line = 0
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        if event.type == pygame.KEYUP and event.key == pygame.K_SPACE:
            line+=1 
            if line >= len(lines):
                line = 0
        if event.type == pygame.VIDEORESIZE:
            GAME_FONT, offset = resize_font(event.h)
        
    screen.fill((255,255,255))
    text_surface = GAME_FONT.render(lines[line], True, (0, 0, 0))
    screen.blit(text_surface, (40, offset))

    pygame.display.flip()

pygame.quit()