!alias row <drac2>

args=argparse(&ARGS&)
rowers = args.get('r') or [4]
rowers*=args.last('rr',default=4, type_=int)
speed=args.get('s',15)
base_roll=['1d20','2d20kh1','2d20kl1'][args.adv()]
rolls=[vroll(f'{base_roll}+{bonus}') for bonus in rowers]
total_check=sum(r.total for r in rolls)
remaining_speed=speed-total_check//4
rounded_speed=5*round(remaining_speed/5)
roll_desc='\n'.join(str(r) for r in rolls)
return f'embed -title "Rowing" -f "Rolls|{roll_desc}" -f "Speed to bridge|{rounded_speed}" '
</drac2>

