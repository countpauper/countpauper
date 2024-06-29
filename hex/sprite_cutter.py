#!.venv/bin/python3
import sys
import argparse
import pygame



def size_argument(arg):
    result = arg.split('x',maxsplit=1)
    if len(result)!=2:
        raise argparse.ArgumentError(f"Faulty size format. {arg} should be <Width>x<Height>")
    result = int(result[0]), int(result[1])
    if result[0]<0 or result[1]<0:
        raise argparse.ArgumentError(f"Faulty size argument. {arg} should only contain positive numbers")
    return result 

if __name__=="__main__":
    parser = argparse.ArgumentParser(
                    prog=sys.argv[0],
                    description='Cut a sprite sheet into individual files')
    parser.add_argument("filename", default="input.png",type=argparse.FileType('rb'))
    parser.add_argument("-s","--size", type=size_argument, required=True)
    parser.add_argument("-o","--output", type=str, default="output")
    parser.add_argument("-f","--format", type=str, default="png")
    args = parser.parse_args()

    input = pygame.image.load(args.filename)
    index = 0
    for y in range(0, input.height, args.size[1]):
        for x in range(0, input.width, args.size[0]):
            sprite = input.subsurface((x,y, args.size[0], args.size[1]))
            pygame.image.save(sprite, f"{args.output}{index}.{args.format}")
            index+=1
