/**********************************/
/* Copyright (C) 2005-2018 IRCtoo */
/**********************************/

extern int level_chatops;
extern int level_wallops;
extern int disable_nw;

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
extern char *irctoo_umodes(aClient *cptr);
