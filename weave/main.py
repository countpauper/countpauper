import pygame
import argparse

def resize_font(height):
    return pygame.font.SysFont('Arial Rounded MT Bold', int(height*0.8)), int(height*0.1)

# Load the lines
def load_lines(line_file):
    lines = args.file.readlines()
    line_file.close()
    return [line.strip() for line in lines]

def open_window(size):
    # initialize the window
    pygame.init()
    screen = pygame.display.set_mode(size, pygame.RESIZABLE)

    return screen

def main_loop(screen, lines):
    GAME_FONT, offset = resize_font(screen.get_size()[1])
    running =  True
    line = 0
    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            elif event.type == pygame.KEYUP and event.key == pygame.K_SPACE:
                line+=1 
                if line >= len(lines):
                    line = 0
            elif event.type == pygame.KEYUP and event.key == pygame.K_BACKSPACE:
                line-=1
                if line < 0:
                    line = len(lines)-1
            if event.type == pygame.VIDEORESIZE:
                GAME_FONT, offset = resize_font(event.h)
            
        screen.fill((255,255,255))
        if args.nr:
            text = f"{line+1}:{lines[line]}"
        else:
            text = lines[line]
        text_surface = GAME_FONT.render(text, True, (0, 0, 0))
        screen.blit(text_surface, (40, offset))

        pygame.display.flip()

# parse the arguments
parser = argparse.ArgumentParser()
parser.add_argument("file", type=argparse.FileType('r'), default="lines.txt", nargs='?')
parser.add_argument("-nr", action='store_true', required=False)
args = parser.parse_args()

lines = load_lines(args.file)
screen = open_window((800,600))
main_loop(screen, lines)


pygame.quit()