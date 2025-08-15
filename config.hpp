/* Modifers */
#define MOD    Mod4Mask
#define ALT    Mod1Mask
#define SHIFT  ShiftMask

#define SPAWN(a) { .v = (const void*[]){"sh", "-c", (a), NULL} }

/* Keyboard keybind */
static std::vector<Key> keys = {
	{MOD,      XK_Return,SPAWN, SPAWN("alacritty")},
	{MOD|SHIFT,XK_e,     EXIT,  {0} },
};
