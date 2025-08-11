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

class LinkedListClient{
private:
class Client{
public:
Window win;
Window root;
int x, y, width, height;
Screen *screen;
Client *next = nullptr;
};

public:
Client *client_head = nullptr;

void assign(Client *c, Window w, XWindowAttributes &wa){
	c->win = w;
	c->root = wa.root;
	c->x = wa.x;
	c->y = wa.y;
	c->width = wa.width;
	c->height = wa.height;
	c->screen = wa.screen;
}

void createNewClient(Window w, XWindowAttributes &wa){
	Client *c = nullptr, *n = nullptr;
	int index = 1;

	if(!client_head){
		client_head = new Client;
		assign(client_head, w, wa);
	}
	for(c = client_head; c->next; c = c->next) index++;
	n = new Client;
	assign(n, w, wa);
	c->next = n;
}

};

class keybind{
public:	

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

};

class kawigrwm{
private:
/* Variabels */
Display *dpy = nullptr;
Window root = None;
bool running = true;
std::vector<keybind::Key> *keys;

LinkedListClient client;

public:
kawigrwm(std::vector<keybind::Key> &keys){
	this->keys = &keys;
}

bool open(){
	this->dpy = XOpenDisplay(NULL);
	if(this->dpy != nullptr) return 0;
	return 1;
}

bool close(){
	if( XCloseDisplay(this->dpy) != BadGC ) return 0;
	return 1;
}

void err_mass(std::string massage){
	std::cerr << NAME << "-" << VERSION << ": "\
	<< massage << std::endl;
}

/* Several Main functions */
void init(){
	unsigned int event_mask = KeyPressMask|ButtonPressMask|PointerMotionMask\
			                    |SubstructureRedirectMask;
	
	this->root = DefaultRootWindow(this->dpy);
	
	/* Grab Keys */
	// Yang Dilakukan, untuk mengambil dan mengetahui rentang keycodes yang dapat digunakan nantinya di grab
	int start, end, k, skip;
	KeySym *syms = nullptr;
	XUngrabKey(this->dpy, AnyKey, AnyModifier, this->root);
	XDisplayKeycodes(this->dpy, &start, &end);
	syms = XGetKeyboardMapping(this->dpy, start, end - start + 1, &skip);
	if(!syms) return;
	
	for(k = start; k <= end; k++)
		for(const keybind::Key &key : *this->keys)
			if(key.keysym == syms[ (k - start) * skip])
				XGrabKey(this->dpy, (KeyCode)k, key.mod, this->root, true, GrabModeAsync, GrabModeAsync);

	XSelectInput(this->dpy, this->root, event_mask);
}

void run(){
	XEvent event;
	while(this->running){
		XNextEvent(this->dpy, &event);
		switch(event.type){
		case KeyPress:   keypress(event);break;
		case MapRequest: maprequest(event);break;
		}
	}
}

// Events Function
void keypress(XEvent &event){
	using Code = keybind::Code;
	XKeyPressedEvent *e = &event.xkey;
	KeySym sym = XKeycodeToKeysym(this->dpy, (KeyCode)e->keycode, 0);

	for(const keybind::Key &key : *this->keys)
		if(key.mod == e->state && key.keysym == sym)
			switch(key.code){
				case Code::SPAWN: spawn(key.args);break;
				case Code::EXIT:  exitwm();break;
			}
}

void maprequest(XEvent &event){
	XMapRequestEvent *e = &event.xmaprequest;
	XWindowAttributes wa;
	
	if( !XGetWindowAttributes(this->dpy, e->window, &wa) ) return;
	client.createNewClient(e->window, wa);
}

// Functions
void spawn(const keybind::Arg &args){
	pid_t pid = fork();
	if(pid == 0){
		execvp((char*)args.v[0], (char**)args.v);
		err_mass("Can't open the program");
	}
}

void exitwm(){
	this->running = false;
}

};

// Configuration
#include "config.h"

int main(){
	kawigrwm wm(keys);
	if ( wm.open() ){
		wm.err_mass("Can't open X display");
		return EXIT_FAILURE;
	}
	wm.init();
	wm.run();
	wm.close();
	return EXIT_SUCCESS;
}
