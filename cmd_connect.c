/*
 * Copyright 2013 Mo McRoberts.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "p_tssh.h"

extern char **environ;

COMMAND connect_commands[] = 
{
	{ "SSH", cmd_connect_ssh, LS_SECURE, CF_FULLMATCH|CF_DEFAULT, 0, NULL, NULL, NULL },
	{ NULL, NULL, LS_IDLE, CF_NONE, 0, NULL, NULL, NULL },
};

int
cmd_connect(LINE *line)
{
	(void) line;
	
	fprintf(stderr, "Usage: CONNECT host\n");
	return -1;
}

static int
usage_ssh(LINE *line)
{
	(void) line;
	
	fprintf(stderr, "Usage: CONNECT SSH host [AS user] [[PORT] tcp-port]\n");
	return -1;
}

int
cmd_connect_ssh(LINE *line)
{
	int c, status;
	const char *user;
	const char *port;
	const char *args[16];
	posix_spawn_file_actions_t actions;
	posix_spawnattr_t attr;
	pid_t childpid;

	user = line->user;
	port = NULL;
	if(!(line->argc - line->argind))
	{
		return usage_ssh(line);
	}
	for(c = line->argind + 1; c < line->argc; c++)
	{
		if(!strcasecmp(line->argv[c], "AS"))
		{
			c++;
			if(c >= line->argc)
			{
				return usage_ssh(line);
			}
			user = line->argv[c];
			continue;
		}
		if(!strcasecmp(line->argv[c], "PORT"))
		{
			c++;
			if(c >= line->argc)
			{
				return usage_ssh(line);
			}
			port = line->argv[c];
			continue;
		}
		if(port)
		{
			return usage_ssh(line);
		}
		port = line->argv[c];
	}
	c = 0;
	args[c] = "ssh";
	c++;
	if(port)
	{
		args[c] = "-p";
		c++;
		args[c] = port;
		c++;
	}
	if(user)
	{
		args[c] = "-l";
		c++;
		args[c] = user;
		c++;
	}
	args[c] = "--";
	c++;
	args[c] = line->argv[line->argind];
	c++;
	args[c] = NULL;
	posix_spawn_file_actions_init(&actions);
	
	posix_spawnattr_init(&attr);
	
	if(posix_spawnp(&childpid, args[0], &actions, &attr, args, environ))
	{
		perror("posix_spawnp");
		return 1;
	}
	/* Wait for child */
	status = 0;
	for(;;)
	{
		if(waitpid(childpid, &status, 0))
		{
			if(errno == ECHILD)
			{
				break;
			}
		}
		else
		{
			break;
		}
	}
	return 0;
}