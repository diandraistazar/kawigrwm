using std::vector;

struct Configuration{
const unsigned int default_tag = 1;
const unsigned int tags = 2;

/*Layouts*/
const LayoutCODE default_layout = TILING_HORIZONTAL;
const vector<LayoutCODE> layouts = {
	TILING_HORIZONTAL,
	TILING_VERTICAL,
};

/* Modifers */
#define MOD    Mod4Mask
#define ALT    Mod1Mask
#define SHIFT  ShiftMask

#define STR(a) { .s = (const void*)(a) }
#define INT(a) { .i = (a) }
#define FLT(a) { .f = (a) }
#define NOARG  {0}

/* Keyboard keybind */
// ! please, use lower case for keysym
const vector<Key> keys = {
// mod         keysym           code	 arg
	{MOD|SHIFT,XK_e,            EXIT,    NOARG },
	{MOD|SHIFT,XK_q,            KILL,    NOARG },
	{MOD,      XK_j,            FOCUS,   INT(-1)},
	{MOD,      XK_k,            FOCUS,   INT(1)},
	
	// Spawn program
	{MOD,      XK_Return,       SPAWN,   STR("alacritty")},

	// Adjust Workspace
	{MOD,      XK_1,            CHGWORK, INT(1)},
	{MOD,      XK_2,            CHGWORK, INT(2)},
	{MOD,      XK_3,            CHGWORK, INT(3)},
	{MOD|SHIFT,XK_1,            MOVWIN,  INT(1)},
	{MOD|SHIFT,XK_2,            MOVWIN,  INT(2)},
	{MOD|SHIFT,XK_3,            MOVWIN,  INT(3)},
	
	// Layout
	{MOD,      XK_i,            CHGLAY,  INT(-1)},
	{MOD,      XK_p,            CHGLAY,  INT(1)},
};

const vector<Button> buttons = {
	{MOD,      Button1,  MOVRESZ,   INT(-1)},
	{MOD,      Button3,  MOVRESZ,   INT(1)},
};
};
