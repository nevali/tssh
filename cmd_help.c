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
