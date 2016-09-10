/* See LICENSE file for copyright and license details. */

/* self restart */
#include "selfrestart.c"

/* appearance */
static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "Liberation Mono:size=10" };
static const char dmenufont[]       = "Liberation Mono:size=10";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#005577";
static const char *colors[SchemeLast][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel] =  { col_gray4, col_cyan,  col_cyan  },
};

/* tagging */
static const char *tags[] = { "", "", "", "", "", "", "", "", "" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class             instance       title       tags mask     isfloating   monitor */
	{ "Gimp",            NULL,          NULL,       0,            1,           -1 },
	{ "Emacs",           NULL,          NULL,       1 << 1,       0,           -1 },
	{ "vivaldi-stable",  NULL,          NULL,       1 << 2,       0,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]",      tile },    /* first entry is default */
	{ "[]",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
static const char *termcmd[]  = { "st", NULL };
static const char *lockcmd[]  = { "lock", NULL };
static const char *surfcmd[]  = { "tsurf", NULL };
/* Media controls */
static const char *unmutecmd[]  = { "pactl", "set-sink-mute", "0", "false", NULL };
static const char *volupcmd[]   = { "pactl", "set-sink-volume", "0", "+5%", NULL };
static const char *voldncmd[]   = { "pactl", "set-sink-volume", "0", "-5%", NULL };
static const char *mutecmd[]    = { "pactl", "set-sink-mute", "0", "toggle", NULL };
static const char *micmutecmd[] = { "pactl", "set-source-mute", "1", "toggle", NULL };

static const char **volupcombo[] = { unmutecmd, volupcmd, NULL };
static const char **voldncombo[] = { unmutecmd, voldncmd, NULL };

static Key keys[] = {
	/* modifier             key                      function        argument */
	{ MODKEY,               XK_p,                    spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,     XK_Return,               spawn,          {.v = termcmd } },
	{ ControlMask|Mod1Mask, XK_l,                    spawn,          {.v = lockcmd } },
	{ MODKEY,               XK_s,                    spawn,          {.v = surfcmd } },
	{ MODKEY,               XK_b,                    togglebar,      {0} },
	{ MODKEY,               XK_k,                    focusstack,     {.i = +1 } },
	{ MODKEY,               XK_l,                    focusstack,     {.i = -1 } },
	{ MODKEY,               XK_i,                    incnmaster,     {.i = +1 } },
	{ MODKEY,               XK_d,                    incnmaster,     {.i = -1 } },
	{ MODKEY,               XK_j,                    setmfact,       {.f = -0.05} },
	{ MODKEY,               XK_semicolon,            setmfact,       {.f = +0.05} },
	{ MODKEY,               XK_Return,               zoom,           {0} },
	{ MODKEY,               XK_Tab,                  view,           {0} },
	{ MODKEY|ShiftMask,     XK_q,                    killclient,     {0} },
	{ MODKEY,               XK_t,                    setlayout,      {.v = &layouts[0]} },
	{ MODKEY,               XK_f,                    setlayout,      {.v = &layouts[1]} },
	{ MODKEY,               XK_m,                    setlayout,      {.v = &layouts[2]} },
	{ MODKEY,               XK_space,                setlayout,      {0} },
	{ MODKEY|ShiftMask,     XK_space,                togglefloating, {0} },
	{ MODKEY,               XK_0,                    view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,     XK_0,                    tag,            {.ui = ~0 } },
	{ MODKEY,               XK_comma,                focusmon,       {.i = -1 } },
	{ MODKEY,               XK_period,               focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,     XK_comma,                tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,     XK_period,               tagmon,         {.i = +1 } },
	TAGKEYS(                XK_1,                                    0)
	TAGKEYS(                XK_2,                                    1)
	TAGKEYS(                XK_3,                                    2)
	TAGKEYS(                XK_4,                                    3)
	TAGKEYS(                XK_5,                                    4)
	TAGKEYS(                XK_6,                                    5)
	TAGKEYS(                XK_7,                                    6)
	TAGKEYS(                XK_8,                                    7)
	TAGKEYS(                XK_9,                                    8)
	{ MODKEY|ShiftMask,     XK_r,                    self_restart,   {0} },
	{ MODKEY|ShiftMask,     XK_e,                    quit,           {0} },
	{ 0,                    XF86XK_AudioRaiseVolume, spawnmultiple,  {.v = volupcombo } },
	{ 0,                    XF86XK_AudioLowerVolume, spawnmultiple,  {.v = voldncombo } },
	{ 0,                    XF86XK_AudioMute,        spawn,          {.v = mutecmd } },
	{ 0,                    XF86XK_AudioMicMute,     spawn,          {.v = micmutecmd } },
};

/* button definitions */
/* click can be ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

