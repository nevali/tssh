#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "p_tssh.h"

int
main(int argc, char **argv)
{
	LINE *line;
	int r;

	(void) argc;
	(void) argv;

	line = cli_create();
	cli_banner(line);
	while(line->state != LS_TERMINATED)
	{
		r = cli_prompt(line);
		if(r < 0)
		{
			if(line->state == LS_HELP)
			{
				line->state = LS_LOCAL;
			}
			continue;
		}
		switch(line->state)
		{
		case LS_USER:
			strncpy(line->user, line->bp, sizeof(line->user) - 1);
			line->user[sizeof(line->user)-1] = 0;
			line->state = LS_LOCAL;
			break;
		case LS_LOCAL:
			if(cli_parse(line) < 0)
			{
				break;
			}
			cli_exec(line);
			break;
		default:
			break;
		}
		puts("");
	}
	return 0;
}
