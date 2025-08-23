#include "../include/main.hpp"

/* Manager class */
Manager::Manager(){
	using std::make_unique;
	auto &g = this->global;
	
	g = make_unique<Variables>();
}

void Manager::err_mass(const std::string &massage){
	std::cerr << NAME << "-" << VERSION << ": "\
	<< massage << std::endl;
}

Display *Manager::open(){
	using std::make_unique;
	auto &g = this->global;
	
	g->dpy = XOpenDisplay(nullptr);
	return g->dpy;
}

void Manager::init(){
	using std::make_unique;
	auto &g = this->global;
	
	int start, end, k, skip;
	KeySym *syms = nullptr;
	XSetWindowAttributes wa;
	
	// Assign this object
	g->man = this;
	// Create Functions and Events memory
	g->func = make_unique<Functions>(g.get());
	g->event = make_unique<Events>(g.get());
	
	// Create config memory
	g->config = make_unique<Configuration>();

	// Create selmon memory
	g->selmon = make_unique<Monitor>();
	g->clients = make_unique<ClientList>(g.get());

	// Assignment some important things
	g->root = DefaultRootWindow(g->dpy);
	g->selmon->tag = 1; // default tag
	
	/* Grab Keys */
	// Yang Dilakukan, untuk mengambil dan mengetahui rentang keycodes yang dapat digunakan nantinya di grab
	XUngrabKey(g->dpy, AnyKey, AnyModifier, g->root);
	XDisplayKeycodes(g->dpy, &start, &end);
	syms = XGetKeyboardMapping(g->dpy, start, end - start + 1, &skip);
	if(!syms) return;
	for(k = start; k <= end; k++)
		for(const Key &key : g->config->keys)
			if(key.keysym == syms[ (k - start) * skip])
				XGrabKey(g->dpy, (KeyCode)k, key.mod, g->root, true, GrabModeAsync, GrabModeAsync);
	
	// Change root window attributes
	wa.event_mask = KeyPressMask|ButtonPressMask|PointerMotionMask\
			        |SubstructureRedirectMask|SubstructureNotifyMask\
	                |EnterWindowMask;
	XChangeWindowAttributes(g->dpy, g->root, CWEventMask, &wa);
	XSelectInput(g->dpy, g->root, wa.event_mask);
	focus(nullptr);
}

void Manager::cleanup(){
	auto &g = this->global;
	
	// Delete clients memory 
	g->clients->cleanup();

	// Ungrab all keys
	XUngrabKey(g->dpy, AnyKey, AnyModifier, g->root);
}

void Manager::close(){
	auto &g = this->global;
	
	XCloseDisplay(g->dpy);
}

void Manager::run(){
	auto &g = this->global;
	auto &events = g->event;
	
	XEvent event;
	while(g->running){
		XNextEvent(g->dpy, &event);
		switch(event.type){
		case KeyPress     : events->keypress(event.xkey);break;
		case ButtonPress  : events->buttonpress(event.xbutton);break;
		case MapRequest   : events->maprequest(event.xmaprequest);break;
		case DestroyNotify: events->destroynotify(event.xdestroywindow);break;
		case EnterNotify  : events->enternotify(event.xcrossing);break;
		}
	}
}

void Manager::grabbuttons(Window &w){
	auto &g = this->global;
	
	/* Grab buttons */
	XUngrabButton(g->dpy, AnyButton, AnyModifier, w);
	for(const Button &button : g->config->buttons)
		XGrabButton(g->dpy, button.button, button.mod, w, false, PointerMotionMask|ButtonPressMask|ButtonReleaseMask, GrabModeAsync, GrabModeAsync, None, None);
}

void Manager::manage(Window &w){
	auto &g = this->global;
	
	XWindowAttributes wa;
	Client *temp = g->clients->createNewClient();
	
	XGetWindowAttributes(g->dpy, w, &wa);
	g->clients->assign(temp, w, wa, g->selmon.get());
	XSelectInput(g->dpy, w, EnterWindowMask|LeaveWindowMask); /* Agar dapat event dari child window dan dapat di proses */
	XMapWindow(g->dpy, w);
	XSync(g->dpy, false);
	grabbuttons(temp->win);
	focus(temp);
}

void Manager::unmanage(Client *c){
	auto &g = this->global;
	
	focus(c->back ? c->back : c->next);
	g->clients->deleteClient(c);
	XSync(g->dpy, false);
}

void Manager::focus(Client *c){
	auto &g = this->global;
	auto &selmon = this->global->selmon;
	
	if(selmon->select == c) return;
	if(c){
		XSetInputFocus(g->dpy, c->win, RevertToPointerRoot, CurrentTime);
		XRaiseWindow(g->dpy, c->win);
	}
	else{
		XSetInputFocus(g->dpy, g->root, RevertToPointerRoot, CurrentTime);
	}
	selmon->select = c;
}
