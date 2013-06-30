#ifndef P_TSSH_H_
# define P_TSSH_H_                      1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <spawn.h>
#include <errno.h>
#include <unistd.h>

typedef struct line_s LINE;
typedef struct command_s COMMAND;

typedef int (*HANDLER)(LINE *line);

typedef enum
{
	LS_IDLE,
	LS_TERMINATED,
	LS_USER,
	LS_SECURE,
	LS_NONPRIVILEGED,
	LS_LOCAL,
	LS_HELP,
	LS_CONNECTED,
	LS_PRIVILEGED
} LINE_STATE;

typedef enum
{
	CF_NONE,
	CF_DEFAULT = (1<<0),
	CF_FULLMATCH = (1<<1),
	CF_HIDDEN = (1<<2)
} COMMAND_FLAGS;

struct line_s
{
	LINE_STATE state;
	char user[64];
	char server_prompt[16];
	char buf[1024];
	char *bp;
	int argc;
	int argind;
	const char *argv[64];
	COMMAND *command;
};

struct command_s
{
	const char *name;
	HANDLER handler;
	LINE_STATE level;
	COMMAND_FLAGS flags;
	int tag;
	COMMAND *subcommands;
	const char *shortdesc;
	const char *help;
};

extern COMMAND commands[];
extern COMMAND connect_commands[];

LINE *cli_create(void);
int cli_banner(LINE *line);
int cli_prompt(LINE *line);
int cli_parse(LINE *line);
COMMAND *cli_match(LINE *line, COMMAND *commands, COMMAND **def);
int cli_exec(LINE *line);
int cli_exec_subcommand(LINE *line, COMMAND *command);
int cli_exec_command(LINE *line, COMMAND *command, int apply);

int cmd_help(LINE *line);
int cmd_logout(LINE *line);
int cmd_connect(LINE *line);
int cmd_connect_ssh(LINE *line);

#endif /*!P_TSSH_*/
