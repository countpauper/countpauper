!alias pray echo <drac2>
total=0
while True:
	mojo=[roll('1d20') for _ in range(randint(9)+1)]
	total+=len(mojo)
	average=sum(mojo)/len(mojo)
	if average>=12:
		break
</drac2>
prays to  me, Avrae 🐲. To clear any bad mojo {{get('they','they') }} rolled {{total}} d20s until the average was {{f"{average:.1f}"}}.