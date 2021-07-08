!alias luck embed {{args=argparse(&ARGS&)}}
{{r=['1d20','2d20kh1','2d20kl1'][args.adv()]}}
{{ro=character().csettings.get('reroll')}}
{{D=vroll(r.replace('d20',f'd20ro{ro}') if ro else r)}}
-title "<name> makes a luck roll" -desc "Result is {{D}}
You can spend one inspiration point to re-roll a luck check" -thumb <image> -color <color>