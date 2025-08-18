#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <iostream>
#include <vector>
#include <memory>
#include <functional>
#include <unistd.h>

/* Enum */
enum Code{ SPAWN, KILL, FOCUS, EXIT, MOVRESZ };
/* Arg */
union Arg{
	const void **v;
	const int i;
	const float f;
};
/* Key */
struct Key{
	unsigned int mod;
	KeySym keysym;
	unsigned int code;
	Arg args;
}; 
/* Button */
struct Button{
	unsigned int mod;
	unsigned int button;
	unsigned code;
	Arg args;
};

struct Client; struct Monitor;
class LinkedListClient;
/* Monitor */
struct Monitor{
	Client *select = nullptr;
	std::unique_ptr<LinkedListClient> clients;
};
/* Client */
struct Client{
	Window win;
	Window root;
	int x, y, width, height;
	Client *back = nullptr;
	Client *next = nullptr;
};
class Manager; class Events; class Functions;
struct Variables{
	Display *dpy = nullptr;
	Window root = None;
	bool running = true;
	Manager *man = nullptr;
	std::unique_ptr<Functions> func;
	std::unique_ptr<Events> event;
	std::unique_ptr<Monitor> selmon;
};

/* ## Forward Decleration ## */
/* Manager class */
class Manager{
public:
std::vector<Key> &p_keys;
std::vector<Button> &p_buttons;
std::unique_ptr<Variables> global;

Manager(std::vector<Key> &keys, std::vector<Button> &buttons);
void err_mass(std::string massage);
Display *open();
void init();
void cleanup();
void close();
void run();
void grabbuttons(Window &w);
void manage(Window &w);
void unmanage(Client *c);
void focus(Client *c);
};

/* Events class */
class Events{
private:
std::unique_ptr<Variables> &global;
public:
Events(std::unique_ptr<Variables> &global);
void keypress(XKeyEvent &event);
void buttonpress(XButtonEvent &event);
void maprequest(XMapRequestEvent &event);
void destroynotify(XDestroyWindowEvent &event);
void enternotify(XCrossingEvent &event);
};

/* Functions class */
class Functions{
private:
std::unique_ptr<Variables> &global;
public:
Functions(std::unique_ptr<Variables> &global);
void spawn(const Arg &args);
void exitman();
void kill();
void movresz(const Arg &args);
void adjustfocus(const Arg &args);
};

/* LinkedListClient */
class LinkedListClient{
public:
Client *client_head = nullptr;
Client *client_tail = nullptr;
int count = 0;

void assign(Client *c, Window w, XWindowAttributes &wa);
void cleanup();
Client *createNewClient();
Client *findClient(Window w);
void deleteClient(Client *c);
void display();
};
