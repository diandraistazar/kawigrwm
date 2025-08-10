#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <iostream>
#include <vector>
#include <unistd.h>

template <typename... Args>
void debugme(const char *specifier, Args... arg){
	const char *path = "/home/diandra/Documents/Coding/kawigrwm/debugme";
	FILE *ptr = fopen(path, "a");
	if(!ptr) return;
	std::fprintf(ptr, specifier, arg...);
	fclose(ptr);
}

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
}

void run(){
	XEvent event;
	while(this->running){
		XNextEvent(this->dpy, &event);
		switch(event.type){
		case KeyPress: keypress(event);break;
		}
	}
}

void cleanup(){
	// Cleaning what?
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

// Events Functions
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

};

// Configuration
#include "config.h"

int main() {
	kawigrwm wm(keys);
	if( wm.open() ){
		wm.err_mass("Can't open X Display");
		return EXIT_FAILURE;
	}
	wm.init();
	wm.run();
	wm.cleanup();
	wm.close();
	return EXIT_SUCCESS;
}
