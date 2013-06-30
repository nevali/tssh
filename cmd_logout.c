#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "p_tssh.h"

int
cmd_logout(LINE *line)
{
	line->state = LS_USER;
	return 0;
}
