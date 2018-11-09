/**********************************/
/* Copyright (C) 2005-2018 IRCtoo */
/**********************************/

#include <sys/utsname.h>
#include "struct.h"
#include "common.h"
#include "sys.h"
#include "numeric.h"
#include "channel.h"
#include "h.h"
#include "throttle.h"
#include "irctoo.h"

extern int user_modes[];

int disable_nw = 0; /* Disable network wide warnings */

int m_nw(aClient *cptr, aClient *sptr, int parc, char *parv[])
{
    int level;

    if (!IsServer(sptr))
    {
        sendto_one(sptr, err_str(ERR_NOPRIVILEGES), me.name, parv[0]);
        return 0;
    }

    if(parc<3)
        return 0;

    if (strlen(parv[2]) > TOPICLEN)
        parv[2][TOPICLEN] = '\0';
    level = atoi(parv[1]);
    sendto_serv_butone_super(cptr, 0, ":%s NW %d :%s", parv[0], level, parv[2]);
    sendto_realops_lev(level, "from %s: %s", parv[0], parv[2]);

    return 0;
}

/* m_fixts - Changes a channel's TS
 * -Kobi_S 21/04/2007
 * parv[1] - channel
 * parv[2] - newts
 */
int m_fixts(aClient *cptr, aClient *sptr, int parc, char *parv[])
{
    aChannel *chptr;

    if(!IsULine(sptr) || (parc<3))
        return 0;

    if(!(chptr = find_channel(parv[1], NULL)))
        return 0;

    chptr->channelts = atol(parv[2]);
    sendto_serv_butone(cptr, ":%s FIXTS %s %s", parv[0], parv[1], parv[2]);

    return 0;
}

char *irctoo_umodes(aClient *cptr)
{
    static char umodes[255];
    int len = 0;
    int *s, flag;

    umodes[0] = '+';

    for(s = user_modes; (flag = *s); s += 2)
    {
        if(cptr->umode & flag)
            umodes[++len] = *(s + 1);
    }
    if(!(cptr->umode & UMODE_r))
    {
        if(len == 0) len--;
        umodes[++len] = '-';
        umodes[++len] = 'r';
    }

    umodes[++len] = '\0';

    return umodes;
}
