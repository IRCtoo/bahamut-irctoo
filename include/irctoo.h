/**********************************/
/* Copyright (C) 2005-2018 IRCtoo */
/**********************************/

extern int level_chatops;
extern int level_wallops;
extern int disable_nw;

extern int m_spoof(aClient *, aClient *, int, char **);
extern int m_ctrl(aClient *, aClient *, int, char **);
extern int m_redir(aClient *, aClient *, int, char **);
extern int do_redir(aClient *acptr, char *reason);
extern int do_redir_all(char *reason);
extern int m_nw(aClient *, aClient *, int, char **);
extern int m_fixts(aClient *, aClient *, int, char **);
extern int m_sl(aClient *, aClient *, int, char **);
extern int m_ul(aClient *, aClient *, int, char **);
extern int m_lwo(aClient *, aClient *, int, char **);
extern int m_lgo(aClient *, aClient *, int, char **);
extern int m_helpop(aClient *, aClient *, int, char **);
extern int load_levels();
extern int del_levels();
extern int save_levels();
extern void send_irctoo_chatops(char *pattern, ...);
extern char *sha256crypt(char *txt); /* from sha256.c */
extern char *irctoo_umodes(aClient *cptr);
