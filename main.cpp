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

struct Client; struct Monitor;

/* Enum */
enum Code{ SPAWN, EXIT };
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
/* Monitor */
struct Monitor{
	int mx, my, mw, mh;
	Client *select = nullptr;
};
/* Client */
struct Client{
Window win;
Window root;
int x, y, width, height;
Client *next = nullptr;
Monitor *mon = nullptr;
};

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
	Client *c = nullptr, *n = nullptr;
	int index = 1;

	if(!client_head){
		client_head = new Client;
		return client_head;
	}
	for(c = client_head; c->next; c = c->next) index++;
	n = new Client;
	c->next = n;
	return n;
}

Client *findWindowClient(Window w){
	Client *temp = client_head;
	while(temp){
		if(temp->win == w) return temp;
		temp = temp->next;
	}
	return nullptr;
}

};

class kawigrwm; class Events; class Functions;

class kawigrwm{
public:
/* Variabels */
Display *dpy = nullptr;
Window root = None;
bool running = true;
std::vector<Key> *p_keys = nullptr;
LinkedListClient client;
Monitor *mon = nullptr;
Functions *func = nullptr;
Events *event = nullptr;

kawigrwm(std::vector<Key> &keys);
void err_mass(std::string massage);
Display *open();
void close();
void init();
void run();
void manageClient(Window &w, XWindowAttributes &wa);
};

class Events{
private:
kawigrwm *wm = nullptr;
Functions *func = nullptr;
public:
void init(kawigrwm *wm, Functions *func);
void keypress(XEvent &event);
void maprequest(XEvent &event);
};

class Functions{
private:
kawigrwm *wm = nullptr;
Events *event = nullptr;
public:
void init(kawigrwm *wm, Events *event);
void spawn(const Arg &args);
void exitwm();
};

// Full implementation of class
#include "include/kawigrwm-cls.h"
#include "include/events-cls.h"
#include "include/functions-cls.h"

// Configuration
#include "config.h"

int main(){
	kawigrwm wm(keys);
	if (!wm.open()){
		wm.err_mass("Can't open X display");
		return EXIT_FAILURE;
	}
	wm.init();
	wm.run();
	wm.close();
	return EXIT_SUCCESS;
}
