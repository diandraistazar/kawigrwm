#include "../include/main.hpp"

/* Manager class */
Manager::Manager(Variables *global) : global(global){
    // no statement for now
}

void Manager::err_msg(const std::string &massage){
	std::cerr << NAME << "-" << VERSION << ": "\
	<< massage << std::endl;
}

Display *Manager::open(){
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
	// Create Functions, Layout and Events memory
	g->func = make_unique<Functions>(g);
	g->event = make_unique<Events>(g);
	g->layout = make_unique<Layout>(g);
	
	// Create config memory
	g->config = make_unique<Configuration>();

	// Create selmon memory
	g->selmon = make_unique<Monitor>();
	g->clients = make_unique<ClientList>(g);

	// Assignment some important things
	g->root = DefaultRootWindow(g->dpy);
	g->screen = DefaultScreen(g->dpy);
	g->width_m = DisplayWidth(g->dpy, g->screen);
	g->height_m = DisplayHeight(g->dpy, g->screen);
	g->selmon->tag = g->config->default_tag; // default tag
	g->selmon->layout = g->config->default_layout; // default layout
	
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

void Manager::grabbuttons(Window w){
	auto &g = this->global;
	auto &config = g->config;
	
	/* Grab buttons */
	XUngrabButton(g->dpy, AnyButton, AnyModifier, w);
	for(const Button &button : config->buttons)
		XGrabButton(g->dpy, button.button, button.mod, w, false, PointerMotionMask|ButtonPressMask|ButtonReleaseMask, GrabModeAsync, GrabModeAsync, None, None);
}

void Manager::manage(Window w){
	auto &g = this->global;
	auto &clients = g->clients;
	
	Client *temp = clients->createNewClient(w);
	
	XMapWindow(g->dpy, temp->win);
	XSelectInput(g->dpy, w, EnterWindowMask|LeaveWindowMask); /* Agar dapat event dari child window dan dapat di proses */
	grabbuttons(temp->win);
	arrange_window();
	warp_pointer(temp->win, temp->width/2, temp->height/2);
	focus(temp);
	XSync(g->dpy, false);
}

void Manager::unmanage(Client *c){
	auto &g = this->global;
	auto &clients = g->clients;
	
	Client *next_select = c->back ? c->back : c->next;
	
	clients->deleteClient(c);
	arrange_window();
	if(next_select)
		warp_pointer(next_select->win, next_select->width/2, next_select->height/2);
	focus(next_select);
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
	selmon->prev_select = selmon->select;
	selmon->select = c;
}

void Manager::map_or_unmap(std::string opt, ClientTG *which_tag){
	auto &g = this->global;

	for(Client *current = which_tag->client_head; current; current = current->next)
		if(opt == "map")
			XMapWindow(g->dpy, current->win);
		else if(opt == "unmap")
			XUnmapWindow(g->dpy, current->win);
		else break;
}

void Manager::arrange_window(){
	auto &g = this->global;
	auto &selmon = g->selmon;
	auto &clients = g->clients;
	auto &layouts = g->layout->layouts;
	
	ClientTG *current_tag = clients->clients[selmon->tag-1];

	if(current_tag->count >= 1)
		((g->layout.get())->*layouts[selmon->layout])(current_tag);
}

void Manager::warp_pointer(Window win, int width, int height){
	auto &g = this->global;
	XWarpPointer(g->dpy, None, win, 0, 0, 0, 0, width, height);
}
