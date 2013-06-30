#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "p_tssh.h"

COMMAND commands[] = 
{
	/* Hidden/full matches are always placed first to avoid triggering a
	 * partial "multiple matches" error before matching them
	 */
	{ "LO", cmd_logout, LS_SECURE, CF_HIDDEN|CF_FULLMATCH, 0, NULL, NULL, NULL },
	
	{ "BACKWARDS", NULL, LS_SECURE, CF_NONE, 0, NULL, NULL, NULL },
	{ "BROADCAST", NULL, LS_SECURE, CF_NONE, 0, NULL, NULL, NULL },
	{ "CONNECT", NULL, LS_SECURE, CF_NONE, 0, connect_commands, NULL, NULL },
	{ "DEFINE", NULL, LS_SECURE, CF_NONE, 0, NULL, NULL, NULL },
	{ "DISCONNECT", NULL, LS_SECURE, CF_NONE, 0, NULL, NULL, NULL },
	{ "FORWARDS", NULL, LS_SECURE, CF_NONE, 0, NULL, NULL, NULL },
	{ "HELP", cmd_help, LS_SECURE, CF_NONE, 0, NULL, NULL, NULL },
	{ "LIST", NULL, LS_SECURE, CF_NONE, 0, NULL, NULL, NULL },
	{ "LOCK", NULL, LS_SECURE, CF_NONE, 0, NULL, NULL, NULL },
	{ "LOGOUT", cmd_logout, LS_SECURE, CF_NONE, 0, NULL, NULL, NULL },
	{ "RESUME", NULL, LS_SECURE, CF_NONE, 0, NULL, NULL, NULL },
	{ "SET", NULL, LS_SECURE, CF_NONE, 0, NULL, NULL, NULL },
	{ "SHOW", NULL, LS_SECURE, CF_NONE, 0, NULL, NULL, NULL },
	{ "TEST", NULL, LS_SECURE, CF_NONE, 0, NULL, NULL, NULL },
	{ NULL, NULL, LS_IDLE, CF_NONE, 0, NULL, NULL, NULL },
};

LINE *
cli_create(void)
{
	LINE *p;

	p = (LINE *) calloc(1, sizeof(LINE));
	if(!p)
	{
		return NULL;
	}
	p->state = LS_USER;
	strcpy(p->server_prompt, "Local> ");
	return p;
}

int
cli_banner(LINE *line)
{
	(void) line;

	printf("Terminal Server " __DATE__ " " __TIME__ "\n\n"
		   "Please type HELP if you need assistance\n\n"
		);

	return 0;
}

int
cli_prompt(LINE *line)
{
	char *t;

	memset(line->buf, 0, sizeof(line->buf));
	line->bp = line->buf;
	switch(line->state)
	{
	case LS_USER:
		printf("Enter username> ");
		break;
	case LS_LOCAL:
		fputs(line->server_prompt, stdout);
		break;
	case LS_HELP:
		printf("Topic? ");
		break;
	default:
		return -1;
	}
	fflush(stdout);
	fgets(line->buf, sizeof(line->buf) - 1, stdin);
	t = strchr(line->buf, '\n');
	if(t)
	{
		*t = 0;
		t--;
	}
	else
	{
		t = strchr(line->buf, 0);
		t--;
	}
	while(t > line->bp && isspace(*t))
	{
		*t = 0;
		t--;
	}
	while(isspace(line->bp[0]))
	{
		line->bp++;
	}
	if(!line->bp[0])
	{
		return -1;
	}
	return 0;
}

int
cli_parse(LINE *line)
{
	char *p, *t;
	int q, e, ls;

	line->argind = 0;
	p = line->bp;
	t = line->bp;
	q = 0;
	e = 0;
	line->argc = 0;
	ls = 1;
	while(*p)
	{
		if(e)
		{			
			e = 0;
			*t = *p;
			t++;
			p++;
			continue;
		}
		if(q)
		{
			if(*p == q)
			{
				q = 0;
				p++;
				continue;
			}
			*t = *p;
			t++;
			p++;
			continue;
		}
		if(isspace(*p))
		{
			if(ls)
			{
				/* skip any intervening spaces */
				p++;
				continue;
			}
			ls = 1;
			*t = 0;
			t++;
			p++;			
		}
		if(ls)
		{
			/* this is the first non-blank character */			
			ls = 0;
			line->argv[line->argc] = t;
			line->argc++;
		}
		if(*p == '\\')
		{
			e = *p;
			p++;
			continue;
		}
		if(*p == '\'' || *p == '"')
		{
			q = *p;
			p++;
			continue;
		}	  
		*t = *p;
		t++;
		p++;
	}
	*t = 0;
	return 0;
}

COMMAND *
cli_match(LINE *line, COMMAND *commands, COMMAND **def)
{
	COMMAND *match;
	int mult;
	size_t l, n;
	const char *arg;

	if(def)
	{
		*def = NULL;
	}
	arg = line->argv[line->argind];	
	match = NULL;
	mult = 0;
	l = strlen(arg);
	for(n = 0; commands[n].name; n++)
	{
		if(commands[n].level > line->state)
		{
			continue;
		}
		if(def && (commands[n].flags & CF_DEFAULT))
		{
			*def = &(commands[n]);
		}
		if(!strcasecmp(commands[n].name, arg))
		{
			/* If it's a full-string match (i.e., the whole command),
			 * it doesn't matter if there are other partial matches
			 */
			match = &(commands[n]);
			return match;
		}
		if(!(commands[n].flags & CF_FULLMATCH) && !strncasecmp(commands[n].name, arg, l))
		{
			if(mult)
			{
				fprintf(stderr, ", %s", commands[n].name);
				continue;
			}
			if(match)
			{
				if(line->argind)
				{
					fprintf(stderr, "Multiple sub-command matches: %s, %s", match->name, commands[n].name);
				}
				else
				{
					fprintf(stderr, "Multiple command matches: %s, %s", match->name, commands[n].name);
				}
				mult = 1;
				continue;
			}
			match = &(commands[n]);
		}
	}
	if(mult)
	{
		fputc('\n', stderr);
		return NULL;
	}
	if(!match)
	{
		if(!def || !*def)
		{
			fprintf(stderr, "Unrecognised command '%s'\n", arg);
		}
		return NULL;
	}
	return match;
}

int
cli_exec(LINE *line)
{
	COMMAND *match;
	
	match = cli_match(line, commands, NULL);
	if(!match)
	{
		return -1;
	}
	return cli_exec_command(line, match, 1);
}

int
cli_exec_subcommand(LINE *line, COMMAND *command)
{
	COMMAND *match, *def;
	
	match = cli_match(line, command->subcommands, &def);
	if(!match)
	{
		if(def)
		{
			return cli_exec_command(line, def, 0);
		}
		return -1;
	}
	return cli_exec_command(line, match, 1);
}

int
cli_exec_command(LINE *line, COMMAND *match, int apply)
{
	COMMAND *prev;
	int r;
	
	if(apply)
	{
		line->argv[line->argind] = match->name;
		line->argind++;
	}
	prev = line->command;
	line->command = match;	
	if(match->subcommands && line->argind < line->argc)
	{
		r = cli_exec_subcommand(line, match);
	}
	else if(match->handler)
	{
		r = match->handler(line);
	}
	else
	{
		fprintf(stderr, "%s: command not implemented\n", match->name);
		r = -3;		
	}
	line->command = prev;
	if(apply)
	{
		line->argind--;
	}
	return r;
}
