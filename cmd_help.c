#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "p_tssh.h"

int
cmd_help(LINE *line)
{
	size_t n, c;

	if(line->argc == 1)
	{
		line->state = LS_HELP;
		printf("HELP\n\n");
		printf("The online HELP facility allows you to access reference and tutorial\n"
			   "information about the terminal server. Choose one of the following options:\n"
			   "\n"
			   "o Enter TUTORIAL to see a succession of HELP frames with \"getting\n"
			   "  started\" information on basic terminal server functions (for beginners)\n"
			   "\n"
			   "o Enter HELP for full information on how to use the HELP facility\n"
			   "\n"
			   "o Choose a HELP topic from the following list:\n"
			   "\n"
			);
		
		c = 0;
		for(n = 0; commands[n].name; n++)
		{
			if(commands[n].level > line->state)
			{
				continue;
			}
			if(commands[n].flags & CF_HIDDEN)
			{
				continue;
			}
			printf("%-24s", commands[n].name);
			c++;
			if(!(c % 3))
			{
				putchar('\n');
			}
		}
		if(c % 3)
		{
			putchar('\n');
		}
	}
	return 0;
}
