/* TODO */
// allow spawn window (DONE)
// allow exitwm (DONE)
// allow focus (DONE)
// add another functions (Working) 
	// allow move window (working)
	// allow resize window
	// allow change another workspace (tags)
// allow adjust focus window
// allow patches like dwm
// be minimalist and fast

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <iostream>
#include <vector>
#include <unistd.h>

template <typename... Args>
void debugme(const char* massage, Args... arg){
	FILE *pStream = fopen("/home/diandra/Documents/Coding/kawigrwm/debugme", "a");
	std::fprintf(pStream, massage, arg...);
	fclose(pStream);
}

/* Enum */
enum Code{ SPAWN, KILL, FOCUS, EXIT, MOVE };
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
	int mw, mh;
	Client *select = nullptr;
	LinkedListClient *clients = nullptr;
};
/* Client */
struct Client{
Window win;
Window root;
int x, y, width, height;
Client *back = nullptr;
Client *next = nullptr;
};

/* LinkedList data structure for Client */
class LinkedListClient{
public:
Client *client_head = nullptr;
Client *client_tail = nullptr;

void assign(Client *c, Window w, XWindowAttributes &wa){
	if(!c) return;

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
		delete client_head;
		client_head = temp;
	}
}

Client *createNewClient(Window &w, XWindowAttributes &wa){
	Client *c = nullptr;

	if(!client_head){
		client_head = new Client;
		client_tail = client_head;
		return client_head;
	}
	for(c = client_head; c->next; c = c->next);
	c->next = new Client;
	c->next->back = c;
	client_tail = c->next;
	return c->next;
}

Client *findClient(Window w){
	Client *temp = nullptr;
	for(temp = client_head; temp; temp = temp->next)
			if(temp->win == w) return temp;
	return nullptr;
}

void deleteClient(Client *c){
	Client *temp = nullptr, *next = nullptr, *back = nullptr;
	// Checking first
	if(!client_head) return;
	
	debugme("%p deleted\n", c);
	if(client_head == c){
		next = client_head->next;
		delete client_head;
		client_head = next;
		if(client_head) client_head->back = nullptr;
		client_tail = tail();
		return;
	}

	for(temp = client_head; temp != c; temp = temp->next);
	back = temp->back; next = temp->next;
	delete temp;
	if(back) back->next = next;
	if(next) next->back = back;
	client_tail = tail();

}

Client *tail(){
	Client *tail = nullptr;
	if(!client_head) return nullptr;
	for(tail = client_head; tail->next; tail = tail->next);
	return tail;
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
std::vector<Button> *p_buttons = nullptr;
Monitor *selmon = nullptr;	/* select monitor */
Functions *func = nullptr;
Events *event = nullptr;

kawigrwm(std::vector<Key> &keys, std::vector<Button> &buttons);
void err_mass(std::string massage);
Display *open();
void init();
void cleanup();
void close();
void run();
void grabbuttons(Window &w);
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
void keypress(XKeyEvent &event);
void buttonpress(XButtonEvent &event);
void maprequest(XMapRequestEvent &event);
void destroynotify(XDestroyWindowEvent &event);
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
void movewindow();
void adjustfocus(const Arg &args);
};

// Full implementation of class
#include "include/kawigrwm-cls.hpp"
#include "include/events-cls.hpp"
#include "include/functions-cls.hpp"

// Configuration
#include "config.hpp"

/* Main functions */
int main(){
	kawigrwm wm(keys, buttons);
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
