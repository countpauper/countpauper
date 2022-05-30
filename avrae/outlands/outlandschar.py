!servalias outlandschar embed -title "Generating random stats:"
-desc "{{ f"<@{ctx.author.id}>\n" }}Ruling: 80+ Total, at least one 17+" {{a=[y for x in range(100) if set('y',[vroll('4d6kh3') for i in range(6)]) or (sum([i.total for i in y])>=80 and [i.total for i in y if i.total>16]) ]}}
-f 'First Scores|{{'\n'.join([str(i) for i in a[0]]) if a else err('Bad luck! Nothing over 79 with a 15+ in 100 tries!')}}
Total = {{sum([i.total for i in a[0]])}}' -footer "{{ ctx.author }}"