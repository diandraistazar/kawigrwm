/* Modifers */
#define MOD    Mod4Mask
#define ALT    Mod1Mask
#define SHIFT  ShiftMask

#define SPAWN(a) { .v = (const void*[]){"sh", "-c", (a), NULL} }

/* Keyboard keybind */
// ! please, use lower case for keysym
static std::vector<Key> keys = {
// mod        keysym    code   arg
	{MOD,      XK_Return,SPAWN, SPAWN("alacritty")},
	{MOD,      XK_j,     FOCUS,  { .i = -1 }},
	{MOD,      XK_k,     FOCUS,  { .i = 1 }},
	{MOD|SHIFT,XK_q,     KILL,  {0} },
	{MOD|SHIFT,XK_e,     EXIT,  {0} },
};

static std::vector<Button> buttons = {
	{MOD,      Button1,  MOVE,    {0}},
	{MOD,      Button3,  RESIZE,  {0}},
};
