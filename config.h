using Key = keybind::Key;
using Button = keybind::Button;
using Code = keybind::Code;

/* Modifers */
#define MOD    Mod4Mask
#define ALT    Mod1Mask
#define SHIFT  ShiftMask

#define SPAWN(a) { .v = (const void*[]){"sh", "-c", (a), NULL} }

/* Keyboard keybind */
static std::vector<Key> keys = {
	{MOD,      XK_Return,Code::SPAWN, SPAWN("urxvt")},
	{MOD|SHIFT,XK_e,     Code::EXIT,  {0} },
};
