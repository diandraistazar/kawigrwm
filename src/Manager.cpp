#include "../include/main.hpp"

/* Manager class */
Manager::Manager(std::vector<Key> &keys, std::vector<Button> &buttons){
	this->p_keys = &keys;
	this->p_buttons = &buttons;
	this->global = std::make_unique<Variables>();
}

void Manager::err_mass(std::string massage){
	std::cerr << NAME << "-" << VERSION << ": "\
	<< massage << std::endl;
}

Display *Manager::open(){
	this->global->dpy = XOpenDisplay(nullptr);
	return this->global->dpy ? this->global->dpy : nullptr;
}

void Manager::init(){
	int start, end, k, skip;
	KeySym *syms = nullptr;
	XSetWindowAttributes wa;
	
	// Assign this object
	this->global->man = this;
	// Create Functions and Events memory
	this->global->func = std::make_unique<Functions>();
	this->global->event = std::make_unique<Events>();

	this->global->func->init(this->global.get());
	this->global->event->init(this->global.get());

	// Create selmon memory
	this->global->selmon = std::make_unique<Monitor>();
	this->global->selmon->clients = std::make_unique<LinkedListClient>();

	// Assignment some important things
	this->global->root = DefaultRootWindow(this->global->dpy);
	
	/* Grab Keys */
	// Yang Dilakukan, untuk mengambil dan mengetahui rentang keycodes yang dapat digunakan nantinya di grab
	XUngrabKey(this->global->dpy, AnyKey, AnyModifier, this->global->root);
	XDisplayKeycodes(this->global->dpy, &start, &end);
	syms = XGetKeyboardMapping(this->global->dpy, start, end - start + 1, &skip);
	if(!syms) return;
	for(k = start; k <= end; k++)
		for(const Key &key : *this->p_keys)
			if(key.keysym == syms[ (k - start) * skip])
				XGrabKey(this->global->dpy, (KeyCode)k, key.mod, this->global->root, true, GrabModeAsync, GrabModeAsync);
	
	// Change root window attributes
	wa.event_mask = KeyPressMask|ButtonPressMask|PointerMotionMask\
			        |SubstructureRedirectMask|SubstructureNotifyMask\
	                |EnterWindowMask;
	XChangeWindowAttributes(this->global->dpy, this->global->root, CWEventMask, &wa);
	XSelectInput(this->global->dpy, this->global->root, wa.event_mask);
	focus(nullptr);
}

void Manager::cleanup(){
	// Delete clients memory 
	this->global->selmon->clients->cleanup();

	// Ungrab all keys
	XUngrabKey(this->global->dpy, AnyKey, AnyModifier, this->global->root);
}

void Manager::close(){
	XCloseDisplay(this->global->dpy);
}

void Manager::run(){
	XEvent event;
	while(this->global->running){
		XNextEvent(this->global->dpy, &event);
		switch(event.type){
		case KeyPress     : this->global->event->keypress(event.xkey);break;
		case ButtonPress  : this->global->event->buttonpress(event.xbutton);break;
		case MapRequest   : this->global->event->maprequest(event.xmaprequest);break;
		case DestroyNotify: this->global->event->destroynotify(event.xdestroywindow);break;
		}
	}
}

void Manager::grabbuttons(Window &w){
	XUngrabButton(this->global->dpy, AnyButton, AnyModifier, w);
	/* Grab buttons */
	for(const Button &button : *this->p_buttons)
		XGrabButton(this->global->dpy, button.button, button.mod, w, false, PointerMotionMask|ButtonPressMask|ButtonReleaseMask, GrabModeAsync, GrabModeAsync, None, None);
}

void Manager::manage(Window &w, XWindowAttributes &wa){
	Client *temp = this->global->selmon->clients->createNewClient();
	this->global->selmon->clients->assign(temp, w, wa);
	XMapWindow(this->global->dpy, w);
	XSync(this->global->dpy, false);
	grabbuttons(temp->win);
	focus(temp);
}

void Manager::unmanage(Client *c){
	focus(c->back ? c->back : c->next);
	this->global->selmon->clients->deleteClient(c);
	XSync(this->global->dpy, false);
}

void Manager::focus(Client *c){
	if(this->global->selmon->select == c) return;
	if(c)
		XSetInputFocus(this->global->dpy, c->win, RevertToPointerRoot, CurrentTime);
	else 
		XSetInputFocus(this->global->dpy, this->global->root, RevertToPointerRoot, CurrentTime);
	this->global->selmon->select = c;
}
