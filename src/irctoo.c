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
#include "pcre.h"

struct spam_filter
{
    struct spam_filter *next;
    char *text;
    long flags;
    char *target;
    char *reason;
    char *id;
    pcre *re;
    unsigned int len;
    unsigned long matches;
};

extern int user_modes[];
extern int services_jr;
extern struct spam_filter *spam_filters;
extern struct spam_filter *new_sf(char *text, long flags, char *reason, char *target);

int disable_helpop = 0; /* Disable the /helpop command */
int disable_nw = 0; /* Disable network wide warnings */
int level_chatops = 0;
int level_wallops = 0;

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

struct user_level *user_levels = NULL;

/* find_level - Finds a user level in memory
 *              Returns: On success - A pointer to the level
 *                       On failure - NULL
 */
struct user_level *find_level(unsigned int level)
{
    struct user_level *p = user_levels;

    for(; p; p = p->next)
    {
        if(p->level==level)
            return p; /* Found! */
    }

    return NULL; /* Not found */
}

/* new_level - Adds a new user level to memory
 *             Returns: A pointer to the new level
 */
struct user_level *new_level(unsigned int level, char *title, int sendq_new, int recvq_new, int sendq_plus, int recvq_plus, int maxchannels, unsigned int raw)
{
    struct user_level *p;

    p = find_level(level);
    if(p)
    {
        if(p->title)
            MyFree(p->title);
    }
    else
    {
        p = MyMalloc(sizeof(struct user_level));
        p->next = user_levels;
        user_levels = p;
    }
    if(title)
    {
        p->title = MyMalloc(strlen(title) + 1);
        strcpy(p->title, title);
    }
    else p->title = NULL;
    p->level = level;
    p->sendq_new = sendq_new;
    p->recvq_new = recvq_new;
    p->sendq_plus = sendq_plus;
    p->recvq_plus = recvq_plus;
    p->maxchannels = maxchannels;
    p->raw = raw;

    return p;
}

/* del_level - Deletes a user level from memory
 *             Returns: 1 = Success
 *                      0 = Failure
 */
int del_level(unsigned int level)
{
    aClient *acptr;
    struct user_level *p, *pprev, *pn;

    for(p = user_levels, pprev = NULL; p; pprev = p, p = pn)
    {
        pn = p->next;
        if(p->level==level)
        {
            if(pprev)
                pprev->next = p->next;
            else
                user_levels = p->next;
            if(p->title)
                MyFree(p->title);
            for(acptr = client; acptr; acptr = acptr->next)
            {
                if(IsClient(acptr) && acptr->user->level==p)
                    acptr->user->level = NULL;
            }
            MyFree(p);
            return 1; /* Success */
        }
    }

    return 0; /* Failure */
}

/* del_levels - Deletes all user levels from memory */
int del_levels()
{
    aClient *acptr;
    struct user_level *p, *pn;

    for(acptr = client; acptr; acptr = acptr->next)
    {
        if(IsClient(acptr) && acptr->user->level)
            acptr->user->level = NULL;
    }

    for(p = user_levels; p; p = pn)
    {
        pn = p->next;
        if(p->title)
            MyFree(p->title);
        MyFree(p);
    }
    user_levels = NULL;

    return 1;
}

/* load_levels - Load the user levels settings
 *               Returns: 1 = Success
 *                        0 = Failure
 */
int load_levels()
{
    FILE *fle;
    char line[1024];
    char *para[MAXPARA + 1];
    int parc;

    if(!(fle = fopen("irctoo.conf", "r")))
        return 0; /* Can't open file! */

    while(fgets(line, sizeof(line), fle))
    {
        char *tmp = strchr(line, '\n');
        if(!tmp)
            break;
        *tmp = '\0';
        tmp = line;
        parc = 0;
        while(*tmp)
        {
            while(*tmp==' ')
             *tmp++ = '\0';

            if(*tmp==':')
            {
                para[parc++] = tmp + 1;
                break;
            }
            para[parc++] = tmp;
            while(*tmp && *tmp!=' ')
                tmp++;
        }
        para[parc + 1] = NULL;
        if(parc>8 && !mycmp(para[0],"UL"))
            new_level(atoi(para[1]), para[parc-1], atoi(para[2]), atoi(para[3]),
                      atoi(para[4]), atoi(para[5]), atoi(para[6]), atoi(para[7]));
        else if(parc>1 && !mycmp(para[0],"SCJ"))
            services_jr = atoi(para[1]);
        else if(parc>1 && !mycmp(para[0],"DHO"))
            disable_helpop = atoi(para[1]);
        else if(parc>1 && !mycmp(para[0],"DNW"))
            disable_nw = atoi(para[1]);
        else if(parc>1 && !mycmp(para[0],"WO"))
            level_wallops = atoi(para[1]);
        else if(parc>1 && !mycmp(para[0],"CO"))
            level_chatops = atoi(para[1]);
        else if(!mycmp(para[0],"SF"))
        {
            if(parc>4)
                new_sf(para[1], atol(para[2]), para[4], para[3]);
            else if(parc>3)
                new_sf(para[1], atol(para[2]), para[3], NULL);
        }
    }
    fclose(fle);

    return 1;
}

/* save_levels - Save the user levels settings
 *               Returns: 1 = Success
 *                        0 = Failure
 */
int save_levels()
{
    FILE *fle;
    struct user_level *p = user_levels;
    struct spam_filter *sf = spam_filters;

    fle = fopen("irctoo.conf","w");
    if(!fle)
        return 0;

    for(; p; p = p->next)
    {
        fprintf(fle, "UL %u %d %d %d %d %d %u :%s\n", p->level, p->sendq_new, p->recvq_new, p->sendq_plus, p->recvq_plus, p->maxchannels, p->raw, p->title);
    }
    for(; sf; sf = sf->next)
    {
        if(sf->target)
            fprintf(fle, "SF %s %ld %s :%s\n", sf->text, sf->flags, sf->target, sf->reason);
        else
            fprintf(fle, "SF %s %ld :%s\n", sf->text, sf->flags, sf->reason);
    }
    fprintf(fle, "SCJ %d\n", services_jr);
    fprintf(fle, "DHO %d\n", disable_helpop);
    fprintf(fle, "DNW %d\n", disable_nw);
    fprintf(fle, "WO %d\n", level_wallops);
    fprintf(fle, "CO %d\n", level_chatops);
    fclose(fle);

    return 1;
}

/* m_ul - Set User Levels
 * parv[1] - Command (+,-,RESET)
 * parv[2] - level
 * parv[3] - sendq_new or sendq_plus
 * parv[4] - recvq_new or recvq_plus
 * parv[5] - maxchannels
 * parv[6] - raw
 * parv[7] - Whois title (should actually be parc-1 and not 7)
 */
int m_ul(aClient *cptr, aClient *sptr, int parc, char *parv[])
{
    char pbuf[512];
    AliasInfo *ai = &aliastab[AII_NS];
    int sendq_new = 0;
    int recvq_new = 0;
    int sendq_plus = 0;
    int recvq_plus = 0;

    if(!IsServer(sptr) || parc<2)
        return 0;

    if(parc > 6 && !mycmp(parv[1], "+"))
    {
        if(!IsULine(sptr) && ai->client && ai->client->from!=cptr->from)
        {
            /*
             * We don't accept commands from a non-services direction.
             * Also, we remove non-existed levels if they come from this location.
             * Note: we don't need to worry about existed levels on the other side
             * because they will be overrided anyway.
             */
            if(!find_level(atoi(parv[2])))
                sendto_one(cptr, ":%s UL - %s", me.name, parv[2]);
            return 0;
        }
        if(parv[3][0]=='+' || parv[3][0]=='-')
            sendq_plus = atoi(parv[3]);
        else
            sendq_new = atoi(parv[3]);
        if(parv[4][0]=='+' || parv[4][0]=='-')
            recvq_plus = atoi(parv[4]);
        else
            recvq_new = atoi(parv[4]);
        new_level(atoi(parv[2]), parv[parc-1], sendq_new, recvq_new, sendq_plus, recvq_plus,
                  atoi(parv[5]), atoi(parv[6]));
        make_parv_copy(pbuf, parc, parv);
        sendto_serv_butone(cptr, ":%s UL %s", parv[0], pbuf);
        return 1;
    }

    if(ai->client && ai->client->from!=cptr->from)
        return 0; /* Ignore it if services are on-line from another direction */

    make_parv_copy(pbuf, parc, parv);
    sendto_serv_butone(cptr, ":%s UL %s", parv[0], pbuf);

    if(parc > 2 && !mycmp(parv[1], "-"))
    {
        return del_level(atoi(parv[2]));
    }
    else if(!mycmp(parv[1], "RESET"))
    {
        return del_levels();
    }
    else if(!mycmp(parv[1], "SAVE"))
    {
        return save_levels();
    }
    else if(!mycmp(parv[1], "LOAD"))
    {
        return load_levels();
    }

    return 0;
}

/* m_sl - Sets the user's level
 * -Kobi_S 13/11/2005
 * parv[1] - nick
 * parv[2] - ts
 * parv[3] - level
 * parv[4] - [+/-]flags (optional)
 */
int m_sl(aClient *cptr, aClient *sptr, int parc, char *parv[])
{
    aClient *acptr;
    ts_val tsinfo;
    struct user_level *level;

    if(!IsServer(sptr) || (parc<4))
        return 0;

    if(!(acptr = find_person(parv[1], NULL)))
         return 0;
    tsinfo = atol(parv[2]);
    if(tsinfo && tsinfo!=acptr->tsinfo)
        return 0; /* wrong tsinfo */

    if(!IsULine(sptr))
    {
        if(cptr->from!=acptr->from)
            return 0; /* Wrong direction */
    }

    level = find_level(atoi(parv[3])); /* Note: level *CAN* be NULL! */

    acptr->user->level = level;

    if(parc==5)
    {
        if(!MyClient(acptr))
        {
            sendto_serv_butone(cptr, ":%s SL %s %ld %s %s", parv[0], acptr->name, tsinfo,
                               parv[3], parv[4]);
            return 0;
        }
        if(parv[4][0]=='-')
            acptr->oflag &= ~(atol(parv[4]) * -1);
        else if(parv[4][0]=='+')
            acptr->oflag |= atol(parv[4]);
        else
            acptr->oflag = atol(parv[4]);
    }
    sendto_serv_butone(cptr, ":%s SL %s %ld %s", parv[0], acptr->name, tsinfo, parv[3]);

    return 0;
}

/* m_lwo - Level WallOps
           parv[1] = minimum level
           parv[2] = maximum level (optinal)
           parv[2] or parv[3] = message
 */
int m_lwo(aClient *cptr, aClient *sptr, int parc, char *parv[])
{
    int i;
    unsigned int minlevel,maxlevel;
    char *msg;
    aClient *acptr;

    if(!IsULine(sptr) || parc<3)
        return 0;

    minlevel = atoi(parv[1]);
    if(parc==4)
    {
        maxlevel = atoi(parv[2]);
        msg = parv[3];
    }
    else
    {
        maxlevel = minlevel;
        msg = parv[2];
    }
    for(i=0;i<=highest_fd;i++)
    {
        if((acptr=local[i])!=NULL)
        {
            if(!IsRegisteredUser(acptr) || !SendWallops(acptr) || !acptr->user->level || acptr->user->level->level<minlevel || acptr->user->level->level>maxlevel|| (maxlevel<25 && IsAnOper(acptr)))
                continue;
            sendto_prefix_one(acptr, sptr, ":%s WALLOPS :%s", parv[0], msg);
        }
    }

    if(parc==4)
        sendto_serv_butone(cptr, ":%s LWO %s %s :%s", parv[0], parv[1], parv[2], msg);
    else
        sendto_serv_butone(cptr, ":%s LWO %s :%s", parv[0], parv[1], msg);

    return 0;
}

/* m_lgo - Level GlobOps
           parv[1] = minimum level
           parv[2] = maximum level (optinal)
           parv[2] or parv[3] = message
 */
int m_lgo(aClient *cptr, aClient *sptr, int parc, char *parv[])
{
    int i;
    unsigned int minlevel,maxlevel;
    char *msg;
    aClient *acptr;

    if(!IsULine(sptr) || parc<3)
        return 0;

    minlevel = atoi(parv[1]);
    if(parc==4)
    {
        maxlevel = atoi(parv[2]);
        msg = parv[3];
    }
    else
    {
        maxlevel = minlevel;
        msg = parv[2];
    }
    for(i=0;i<=highest_fd;i++)
    {
        if((acptr=local[i])!=NULL)
        {
            if(!IsRegisteredUser(acptr) || !SendGlobops(acptr) || !acptr->user->level || acptr->user->level->level<minlevel || acptr->user->level->level>maxlevel || (maxlevel<25 && IsAnOper(acptr)))
                continue;
            sendto_prefix_one(acptr, sptr, ":%s NOTICE %s :*** Global -- from %s: %s",
                              me.name, acptr->name, parv[0], msg);
        }
    }

    if(parc==4)
        sendto_serv_butone(cptr, ":%s LGO %s %s :%s", parv[0], parv[1], parv[2], msg);
    else
        sendto_serv_butone(cptr, ":%s LGO %s :%s", parv[0], parv[1], msg);

    return 0;
}

/* send_helpops - Send a notice to all the local +h users.
 * -Kobi_S 01/12/2005
 */
void send_helpops(char *pattern, ...)
{
    aClient *cptr;
    char nbuf[1024];
    va_list vl;
    int i;

    va_start(vl, pattern);
    for (i = 0; i <= highest_fd; i++)
    {
        if ((cptr = local[i]) && !IsServer(cptr) && !IsMe(cptr) && IsUmodeh(cptr))
        {
            ircsprintf(nbuf, ":%s NOTICE %s :*** HelpOp -- %s",
                       me.name, cptr->name, pattern);
            vsendto_one(cptr, nbuf, vl);
        }
    }
    va_end(vl);

    return;
}

/* m_helpop - Send a notice to all the +h users/opers.
              parv[1] = message
 */
int m_helpop(aClient *cptr, aClient *sptr, int parc, char *parv[])
{
    char *message = parc > 1 ? parv[1] : NULL;

    if (BadPtr(message))
    {
        if (MyClient(sptr))
            sendto_one(sptr, err_str(ERR_NEEDMOREPARAMS),
                       me.name, parv[0], "HELPOP");
        return 0;
    }

    if(disable_helpop && ((disable_helpop > 1) || (MyConnect(sptr) && !IsUmodeh(sptr) && !IsAnOper(sptr))))
    {
        sendto_one(sptr, ":%s NOTICE %s :The helpop command has been disabled.",
                   me.name, parv[0]);
        return 0;
    }

    if (strlen(message) > TOPICLEN)
        message[TOPICLEN] = '\0';
    sendto_serv_butone_super(cptr, 0, ":%s HELPOP %s", parv[0], message);
    if (MyConnect(sptr) && !IsUmodeh(sptr) && !IsAnOper(sptr))
    {
        sptr->since += 4;
        sendto_one(sptr, ":%s NOTICE %s :Your help request has been forwarded to our helpers,",
                   me.name, parv[0]);
        sendto_one(sptr, ":%s NOTICE %s :They will try to assist you as soon as possible.",
                   me.name, parv[0]);
        sendto_one(sptr, ":%s NOTICE %s :If you need help with %s's services or help from an IRC Operator goto #IRCtoo", me.name, parv[0], Network_Name);
        sendto_one(sptr, ":%s NOTICE %s :Thank you for using %s!", me.name, parv[0],
                   Network_Name);
        send_helpops("from %s (Local): %s", parv[0], message);
    }
    else
        send_helpops("from %s: %s", parv[0], message);
    return 0;
}

/* send_irctoo_chatops - Send a notice to all the local +b users (not only to +b opers).
 * -Kobi_S 04/12/2005
 */
void send_irctoo_chatops(char *pattern, ...)
{
    aClient *cptr;
    char nbuf[1024];
    va_list vl;
    int i;

    va_start(vl, pattern);
    for (i = 0; i <= highest_fd; i++)
    {
        if ((cptr = local[i]) && !IsServer(cptr) && !IsMe(cptr) && SendChatops(cptr))
        {
            ircsprintf(nbuf, ":%s NOTICE %s :*** ChatOps -- %s",
                       me.name, cptr->name, pattern);
            vsendto_one(cptr, nbuf, vl);
        }
    }
    va_end(vl);

    return;
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
