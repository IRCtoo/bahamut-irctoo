/************************************************************************
 *   IRC - Internet Relay Chat, include/h.h
 *   Copyright (C) 1992 Darren Reed
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 1, or (at your option)
 *   any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
/*
 * "h.h". - Headers file.
 * 
 * Most of the externs and prototypes thrown in here to 'cleanup' things.
 * -avalon
 * 
 * 
 */

/* $Id$ */

#include "send.h"
#include "ircsprintf.h"

extern int  R_do_dns, R_fin_dns, R_fin_dnsc, R_fail_dns, R_do_id,
    R_fin_id, R_fail_id;

extern aMotd *motd;
extern struct tm *motd_tm;

#ifdef SHORT_MOTD
extern aMotd *shortmotd;
#endif

extern aMotd *helpfile;		/* oper helpfile is a link list of aMotd */

extern DLink *server_list;
extern DLink *oper_list;
extern DLink *listing_clients;
extern DLink *recvq_clients;

extern aConnect *connects;
extern aAllow *allows;
extern aPort *ports;
extern Conf_Me *MeLine;
extern aOper *opers;
extern aUserv *uservers;


#include "fdlist.h"
extern int  	 lifesux;
extern fdlist 	 serv_fdlist;
extern fdlist 	 busycli_fdlist;
extern fdlist 	 default_fdlist;
extern fdlist 	 oper_fdlist;
extern int  	 MAXCLIENTS;
extern struct Counter Count;

extern time_t 	 NOW;
extern time_t	 last_stat_save;
extern time_t 	 nextconnect, nextdnscheck, nextping, timeofday;
extern aClient  *client, me, *local[];
extern aChannel *channel;
extern struct    stats *ircstp;
extern int  	 bootopt;

extern char 	*canonize(char *);
extern void 	 check_fdlists();
extern aChannel *find_channel(char *, aChannel *);
extern aBan 	*nick_is_banned(aChannel *, char *, aClient *);
extern void 	 remove_matching_bans(aChannel *, aClient *, aClient *);
extern void 	 remove_user_from_channel(aClient *, aChannel *);
extern void 	 del_invite(aClient *, aChannel *);
extern void 	 send_user_joins(aClient *, aClient *);
extern int  	 can_send(aClient *, aChannel *, char *);
extern int   	 is_chan_op(aClient *, aChannel *);
extern int  	 has_voice(aClient *, aChannel *);
extern int  	 count_channels(aClient *);
extern char 	*pretty_mask(char *);


extern aClient 	*find_chasing(aClient *, char *, int *);

extern int  	  find_restrict(aClient *);
extern int  	  rehash(aClient *, aClient *, int);
extern int  	  initconf(int, int, aClient *);
extern int   	  openconf(char *);
extern int  	  lock_kline_file();

extern void 	  clear_scache_hash_table(void);
extern char 	 *find_or_add(char *);
extern void 	  count_scache(int *, u_long *);
extern void 	  list_scache(aClient *, aClient *, int, char **);

extern char 	 *MyMalloc(size_t);
extern char 	 *MyRealloc(char *, size_t);

/* MyFree is defined as a macro in sys.h 
 * extern  void     MyFree (char *); 
 */

extern char 	 *debugmode, *configfile, *sbrk0;
extern char 	 *klinefile;
extern char  	 *zlinefile;
extern char 	 *getfield(char *);
extern void 	  get_sockhost(aClient *, char *);
extern char 	 *rpl_str(int);
extern char 	 *err_str(int);
extern char 	 *getreply(int);
extern char 	 *strerror(int);
extern int  	  dgets(int, char *, int);
extern char 	 *inetntoa(char *);
extern int  	  dbufalloc, dbufblocks, debuglevel, errno, h_errno;
extern int  	  highest_fd, debuglevel, portnum,
    debugtty, maxusersperchannel;
extern int  	  readcalls, udpfd, resfd;
extern aClient 	 *add_connection(aListener *, int);
extern int  	  add_listener(aPort *);
extern void 	  add_local_domain(char *, int);
extern int  	  check_client(aClient *);
extern int  	  check_server_init(aClient *);
extern void 	  close_connection(aClient *);
extern void 	  close_listeners();
extern void       open_listeners();
extern int   	  connect_server(aConnect *, aClient *, struct hostent *);
extern void 	  get_my_name(aClient *, char *, int);
extern int  	  get_sockerr(aClient *);
extern int  	  inetport(aClient *, char *, int, u_long);
extern void 	  init_sys();
extern int  	  read_message(time_t, fdlist *);
extern void 	  report_error(char *, aClient *);
extern void 	  set_non_blocking(int, aClient *);
extern int  	  setup_ping(void);
extern void 	  summon(aClient *, char *, char *, char *);
extern int  	  unixport(aClient *, char *, int);
extern int  	  utmp_open(void);
extern int  	  utmp_read(int, char *, char *, char *, int);
extern int  	  utmp_close(int);

extern void 	do_dns_async(void);
extern int 	completed_connection(aClient *);
extern void 	accept_connection(aListener *);
extern char *	irc_get_sockerr(aClient *);
extern int 	read_packet(aClient *);
extern int 	do_client_queue(aClient *);
extern void 	read_error_exit(aClient *, int, int);
extern int 	readwrite_client(aClient *, int, int);

extern char      *get_listener_name(aListener *);
extern int        attach_Iline(aClient *, struct hostent *, char *);
extern aConnect  *find_aConnect(char *);
extern aOper     *find_oper(char *, char *, char *, char *);
extern aConnect  *find_aConnect_match(char *, char *, char *);
extern aUserv    *find_aUserver(char *);
extern void       clear_conflinks(aClient *);

extern void 	  start_auth(aClient *);
extern void 	  read_authports(aClient *);
extern void 	  send_authports(aClient *);

extern void 	  restart(char *);
extern void 	  send_channel_modes(aClient *, aChannel *);
extern void 	  server_reboot(void);
extern void 	  terminate(void), write_pidfile(void);

extern int  	  match(char *, char *);
extern char  	 *collapse(char *);

extern int  	  writecalls, writeb[];
extern int  	  deliver_it(aClient *, char *, int);

extern int  	  check_registered(aClient *);
extern int  	  check_registered_user(aClient *);
extern char 	 *get_client_name(aClient *, int);
extern char 	 *get_client_host(aClient *);
extern char 	 *my_name_for_link(char *, aConnect *);
extern char 	 *myctime(time_t), *date(time_t);
extern int  	  exit_client(aClient *, aClient *, aClient *, char *);
extern void 	  initstats(void), tstats(aClient *, char *);
extern void 	  serv_info(aClient *, char *);
extern char      *make_parv_copy(char *, int, char **);

int               msg_has_colors(char *);

extern int  	  parse(aClient *, char *, char *);
extern void 	  init_tree_parse(struct Message *);

extern int  	  do_numeric(int, aClient *, aClient *, int, char **);
extern int  	  hunt_server(aClient *, aClient *, char *, int, int, char **);
extern aClient 	 *next_client(aClient *, char *);
extern aClient 	 *next_client_double(aClient *, char *);

extern int  	  m_umode(aClient *, aClient *, int, char **);
extern int  	  m_names(aClient *, aClient *, int, char **);
extern int  	  m_server_estab(aClient *);
extern void 	  send_umode(aClient *, aClient *, int, int, char *);
extern void	  send_umode_out(aClient *, aClient *, int);
extern int 	  del_silence(aClient *, char *);


extern void 	  free_client(aClient *);
extern void 	  free_link(Link *);
extern void 	  free_dlink(DLink *);
extern void 	  free_chanmember(chanMember *);
extern void 	  free_class(aClass *);
extern void 	  free_user(anUser *, aClient *);
extern void 	  free_channel(aChannel *);
extern aChannel  *make_channel();
extern Link 	 *make_link(void);
extern CLink     *make_clink(void);
extern DLink 	 *make_dlink(void);
extern chanMember *make_chanmember(void);
extern anUser 	 *make_user(aClient *);
extern aClass 	 *make_class(void);
extern aServer   *make_server(aClient *);
extern aClient   *make_client(aClient *, aClient *);
extern chanMember *find_user_member(chanMember *, aClient *);
extern Link 	 *find_str_link(Link *, char *);
extern void 	  add_client_to_list(aClient *);
extern void 	  checklist(void);
extern void 	  remove_client_from_list(aClient *);
extern void 	  initlists(void);
extern void 	  block_garbage_collect(void);	/* list.c */
extern void       block_destroy(void);	        /* list.c */

extern void 	  add_class(int, int, int, int, long);
extern long 	  get_sendq(aClient *);
extern int  	  get_con_freq(aClass *);
extern int  	  get_client_ping(aClient *);
extern int  	  get_client_class(aClient *);
extern void 	  report_classes(aClient *);

extern struct hostent *get_res(char *);
extern struct hostent *gethost_byaddr(char *, Link *);
extern struct hostent *gethost_byname(char *, Link *);
extern void 	  flush_cache(void);
extern int  	  init_resolver(int);
extern time_t 	  timeout_query_list(time_t);
extern time_t 	  expire_cache(time_t);
extern void 	  del_queries(char *);

extern void 	  clear_channel_hash_table(void);
extern void 	  clear_client_hash_table(void);
extern int  	  add_to_client_hash_table(char *, aClient *);
extern int  	  del_from_client_hash_table(char *, aClient *);
extern int  	  add_to_channel_hash_table(char *, aChannel *);
extern int   	  del_from_channel_hash_table(char *, aChannel *);
extern aChannel  *hash_find_channel(char *, aChannel *);
extern aClient   *hash_find_client(char *, aClient *);
extern aClient   *hash_find_nickserver(char *, aClient *);
extern aClient   *hash_find_server(char *, aClient *);

extern void 	  add_history(aClient *, int);
extern aClient   *get_history(char *, time_t);
extern void 	  initwhowas(void);
extern void 	  off_history(aClient *);

extern int  	  dopacket(aClient *, char *, int);
extern int  	  client_dopacket(aClient *, char *, int);

/* VARARGS2 */

extern void 	  debug(int level, char *pattern, ...);

#ifdef DEBUGMODE
extern void 	  send_usage(aClient *, char *);
extern void 	  send_listinfo(aClient *, char *);
extern void 	  count_memory(aClient *, char *);
#endif

extern void 	  send_list(aClient *, int);
extern aChannel  *hash_get_chan_bucket(int);

#ifdef DUMP_DEBUG
extern FILE 	 *dumpfp;
#endif

void        	  remove_one_ip(unsigned long);
void        	  clear_ip_hash_table(void);
void        	  rehash_ip_hash(void);

#ifdef FLUD
int         	  check_for_ctcp();
int         	  check_for_flood();
void        	  free_fluders();
void        	  free_fludees();
#define MyFludConnect(x)	(((x)->fd >= 0) || ((x)->fd == -2))

#endif /* FLUD */

#ifdef ANTI_SPAMBOT
#define MIN_SPAM_NUM 5
#define MIN_SPAM_TIME 60
#endif

#define MAXKILLS 20
extern void 	  count_watch_memory(int *, u_long *);
extern void    	  clear_watch_hash_table(void);
extern int     	  add_to_watch_hash_table(char *, aClient *);
extern int     	  del_from_watch_hash_table(char *, aClient *);
extern int     	  hash_check_watch(aClient *, int);
extern int     	  hash_del_watch_list(aClient  *);
extern aWatch 	 *hash_get_watch(char *);
#define MAXWATCH       128

DLink *add_to_list(DLink **, void *);
void remove_from_list(DLink **, void *, DLink *);
void print_list_memory(aClient *);

#include "find.h"
