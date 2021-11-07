!alias flip embed <drac2>
coins = [dict(name='Heads',
			  img='https://cdn.discordapp.com/attachments/751098635038621868/904671493982666752/waterdeep_heads.png'),
		 dict(name='Tails',
			  img='https://cdn.discordapp.com/attachments/751098635038621868/904671514169856010/waterdeep_tails.png')]
c=coins[randint(2)]
return f'-title "{ctx.author.display_name} flips a coin..." -desc "... and it comes up **{c.name}**." -image {c.img}'
</drac2>