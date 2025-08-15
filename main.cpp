/* TODO */
// allow spawn window (DONE)
// allow exitwm (DONE)
// allow focus (DONE)
// add another functions
// allow move window
// allow resize window
// allow adjust focus window
// allow change another workspace
// allow patches like dwm
// be minimalist and fast

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <iostream>
#include <vector>
#include <unistd.h>

template <typename... Args>
void debugme(int stream, const char* massage, Args... arg){
	FILE *pStream = nullptr;
	switch(stream){
		case 0: pStream = stdout;break;
		case 1: pStream = stdin;break;
		case 2: pStream = stderr;break;
	}
	
	std::cout << NAME << ": ";
	std::fprintf(pStream, massage, arg...);
}

/* Enum */
enum Code{ SPAWN, KILL, EXIT };
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
	Code code;
	Arg args;
}; 
/* Button */
struct Button{
	unsigned int mod;
	unsigned int button;
	Code code;
	Arg args;
};

struct Client; struct Monitor;
class LinkedListClient;
/* Monitor */
struct Monitor{
	int mw, mh;
	Client *select = nullptr;
	LinkedListClient *clients = nullptr;
};
/* Client */
struct Client{
Window win;
Window root;
int x, y, width, height;
Client *next = nullptr;
Monitor *mon = nullptr;
};

/* LinkedList data structure for Client */
class LinkedListClient{
public:
Client *client_head = nullptr;

void assign(Client *c, Window w, XWindowAttributes &wa){
	c->win = w;
	c->root = wa.root;
	c->x = wa.x;
	c->y = wa.y;
	c->width = wa.width;
	c->height = wa.height;
}

void cleanup(){
	Client *temp = nullptr;
	while(client_head){
		temp = client_head->next;
		debugme(0, "Deleted %p\n", client_head);
		delete client_head;
		client_head = temp;
	}
}

Client *createNewClient(){
	Client *c = nullptr;

	if(!client_head){
		client_head = new Client;
		return client_head;
	}
	for(c = client_head; c->next; c = c->next);
	c->next = new Client;
	return c->next;
}

Client *findClient(Window w){
	Client *temp = client_head;
	while(temp){
		if(temp->win == w) return temp;
		temp = temp->next;
	}
	return nullptr;
}

void deleteClient(Client *c){
	Client *temp = nullptr, *next = nullptr;
	// Checking first
	if(!client_head) return;

	if(!client_head->next){
		delete client_head;
		client_head = nullptr;
		return;
	}
	for(temp = client_head; temp->next != c; temp = temp->next);
	next = temp->next->next;
	delete temp->next;
	temp->next = next;
}

};

class kawigrwm; class Events; class Functions;

/* Kawigrwm class */
class kawigrwm{
public:
/* Variabels */
Display *dpy = nullptr;
Window root = None;
bool running = true;
std::vector<Key> *p_keys = nullptr;
Monitor *selmon = nullptr;	/* select monitor */
Functions *func = nullptr;
Events *event = nullptr;

kawigrwm(std::vector<Key> &keys);
void err_mass(std::string massage);
Display *open();
void cleanup();
void close();
void init();
void run();
void manage(Window &w, XWindowAttributes &wa);
void unmanage(Client *c);
void focus(Client *c);
};

/* Events class */
class Events{
private:
kawigrwm *wm = nullptr;
Functions *func = nullptr;
public:
void init(kawigrwm *wm, Functions *func);
void keypress(XEvent &event);
void maprequest(XEvent &event);
void destroynotify(XEvent &event);
};

/* Functions class */
class Functions{
private:
kawigrwm *wm = nullptr;
Events *event = nullptr;
public:
void init(kawigrwm *wm, Events *event);
void spawn(const Arg &args);
void exitwm();
void kill();
};

// Full implementation of class
#include "include/kawigrwm-cls.hpp"
#include "include/events-cls.hpp"
#include "include/functions-cls.hpp"

// Configuration
#include "config.hpp"

/* Main functions */
int main(){
	kawigrwm wm(keys);
	if (!wm.open()){
		wm.err_mass("Can't open X display");
		return EXIT_FAILURE;
	}
	wm.init();
	wm.run(); 
	wm.cleanup(); 
	wm.close();
	return EXIT_SUCCESS;
}
