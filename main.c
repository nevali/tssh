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
