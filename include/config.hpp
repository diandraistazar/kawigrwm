struct Configuration{
const unsigned int tags = 2;

/* Modifers */
#define MOD    Mod4Mask
#define ALT    Mod1Mask
#define SHIFT  ShiftMask

#define SPAWN(a) { .v = (const void*[]){"sh", "-c", (a), NULL} }
#define STR(a) { .s = (const void*)(a) }
#define INT(a) { .i = (a) }

/* Keyboard keybind */
// ! please, use lower case for keysym
const std::vector<Key> keys = {
// mod         keysym           code	 arg
	{MOD,      XK_Return,       SPAWN,   SPAWN("alacritty")},
	{MOD,      XK_j,            FOCUS,   INT(-1)},
	{MOD,      XK_k,            FOCUS,   INT(1)},
	
	{MOD,      XK_1,            CHGWORK, INT(1)},
	{MOD,      XK_2,            CHGWORK, INT(2)},
	{MOD,      XK_3,            CHGWORK, INT(3)},
	{MOD|SHIFT,XK_1,            MOVWIN,  INT(1)},
	{MOD|SHIFT,XK_2,            MOVWIN,  INT(2)},
	{MOD|SHIFT,XK_3,            MOVWIN,  INT(3)},
	
	{MOD|SHIFT,XK_q,            KILL,    {0} },
	{MOD|SHIFT,XK_e,            EXIT,    {0} },
};

const std::vector<Button> buttons = {
	{MOD,      Button1,  MOVRESZ,   {.i = -1}},
	{MOD,      Button3,  MOVRESZ,   {.i = 1}},
};
};
