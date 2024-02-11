############### Render results
row_width = 7
nl="\n"
def format_stats(stats):
        return "\t".join(f"{v}"[:row_width] for v in stats.values())

def format_table(table):
        "Format a list of dictionaries (assumed to have the same keys as the first) into a TSV"
        header = [head[:row_width] for head in table[0].keys()]
        return "\t".join(header) + nl + nl.join(format_stats(lvl_stats) for lvl_stats in table)
