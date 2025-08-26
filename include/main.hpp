#pragma once

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <iostream>
#include <vector>
#include <memory>
#include <functional>
#include <cstring>
#include <unistd.h>

template <typename... Args>
void debugme(const char* massage, Args... args){
	FILE *ptr = fopen("/home/diandra/Documents/Coding/kawigrwm/debugme", "a");
	std::fprintf(ptr, massage, args...);
	fclose(ptr);
}

/* Enum */
enum Code{ SPAWN, KILL, FOCUS, EXIT, MOVRESZ, CHGWORK, MOVWIN };
/* Arg */
union Arg{
	const void **v;
	const void *s;
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
class ClientList;
/* Monitor */
struct Monitor{
	Client *select = nullptr;
	unsigned int tag;
};
/* Client */
struct Client{
	Window win;
	Window root;
	int x, y, width, height;
	unsigned int tag;
	Client *back = nullptr;
	Client *next = nullptr;
};
struct ClientTG{
	Client *client_head = nullptr;
	Client *client_tail = nullptr;
};
class Manager; class Events; class Functions;
struct Configuration;
struct Variables{
	Display *dpy;
	Window root = None;
	bool running = true;
	Manager *man;
	std::unique_ptr<Functions> func;
	std::unique_ptr<Events> event;
	std::unique_ptr<Monitor> selmon;
	std::unique_ptr<ClientList> clients;
	std::unique_ptr<Configuration> config;
};

// Configuration
#include "../include/config.hpp"

/* ## Forward Decleration ## */
/* Manager class */
class Manager{
public:
std::unique_ptr<Variables> global;
Manager();
void err_mass(const std::string &massage);
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
Variables *global;
public:
Events(Variables *global);
void keypress(XKeyEvent &event);
void buttonpress(XButtonEvent &event);
void maprequest(XMapRequestEvent &event);
void destroynotify(XDestroyWindowEvent &event);
void enternotify(XCrossingEvent &event);
};

/* Functions class */
class Functions{
private:
Variables *global;
public:
Functions(Variables *global);
void spawn(const Arg &args);
void exitman();
void kill();
void movresz(const Arg &args);
void adjustfocus(const Arg &args);
void changeworkspace(const Arg &args);
void move_win_to_another_workspace(const Arg &args);
};

/* ClientList */
class ClientList{
private:
Variables *global;
public:
std::vector<ClientTG*> clients;
ClientList(Variables *global);
void assign(Client *c, Window w, XWindowAttributes &wa, Monitor *selmon);
void cleanup();
Client *createNewClient();
Client *findClient(Window w);
void deleteClient(Client *c);
void moveClientToAnotherTag(Client *select, unsigned int which_tag);
void display();
private:
void removeClientFromTag(Client *select, ClientTG *tagsel);
void addClientToTag(Client *select, ClientTG *dest_tag);
};
