/* TODO */
// allow spawn window (DONE)
// allow exitwm (DONE)
// allow focus (DONE)
// add another functions (Working) 
	// allow move window (working)
	// allow resize window
	// allow change another workspace (tags)
// allow adjust focus window (DONE)
// allow patches like dwm
// be minimalist and fast

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <iostream>
#include <vector>
#include <memory>
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

/* LinkedList data structure for Client */
class LinkedListClient{
public:
Client *client_head = nullptr;
Client *client_tail = nullptr;
int count = 0;

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
		count--;
	}
	client_head = nullptr;
	client_head = nullptr;
}

Client *createNewClient(){
	Client *c = new Client;
	if(!client_head) client_head = c;	
	else{
		client_tail->next = c;
		c->back = client_tail;
	}
	client_tail = c;
	count++;
	return c;
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
	
	for(temp = client_head; temp && temp != c; temp = temp->next);
	back = temp->back; next = temp->next;
	if(next) next->back = back;
	else client_tail = back;

	if(back) back->next = next;
	else client_head = next;
	delete temp;
	count--;
}

void display(){
	if(!client_head) return;
	debugme("HEAD: %p\n", client_head);
	for(Client *temp = client_head; temp != client_head && temp != client_tail; temp = temp->next)
		debugme("CLIENT: %p\n", temp);
	debugme("TAIL: %p\n", client_tail);
}

};

class kawigrwm; class Events; class Functions;
struct Variables{
	Display *dpy = nullptr;
	Window root = None;
	bool running = true;
	Monitor *selmon = nullptr;
	kawigrwm *wm = nullptr;
	Functions *func = nullptr;
	Events *event = nullptr;
};

/* Kawigrwm class */
class kawigrwm{
public:
std::vector<Key> *p_keys = nullptr;
std::vector<Button> *p_buttons = nullptr;
std::unique_ptr<LinkedListClient> clients;
std::unique_ptr<Monitor> selmon;
std::unique_ptr<Events> event;
std::unique_ptr<Functions> func;
std::unique_ptr<Variables> global;

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
Variables *global = nullptr;
public:
void init(Variables *global);
void keypress(XKeyEvent &event);
void buttonpress(XButtonEvent &event);
void maprequest(XMapRequestEvent &event);
void destroynotify(XDestroyWindowEvent &event);
};

/* Functions class */
class Functions{
private:
Variables *global = nullptr;
public:
void init(Variables *global);
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
