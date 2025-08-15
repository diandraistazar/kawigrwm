kawigrwm::kawigrwm(std::vector<Key> &keys){
	this->p_keys = &keys;
}

void kawigrwm::err_mass(std::string massage){
	std::cerr << NAME << "-" << VERSION << ": "\
	<< massage << std::endl;
}

Display *kawigrwm::open(){
	this->dpy = XOpenDisplay(NULL);
	debugme(0, "Window manager opened\n");
	if(this->dpy)
		return this->dpy;
	return nullptr;
}

void kawigrwm::cleanup(){
	// Delete Functions and Events class memory
	delete this->func; delete this->event;
	// Delete selmon and it is members
	this->selmon->clients->cleanup(); delete this->selmon->clients;
	delete this->selmon;

	// Ungrab all keys
	XUngrabKey(this->dpy, AnyKey, AnyModifier, this->root);
}

void kawigrwm::close(){
	XCloseDisplay(this->dpy);
	debugme(0, "Window manager closed\n");
}

void kawigrwm::init(){
	unsigned int event_mask = KeyPressMask|ButtonPressMask|PointerMotionMask\
			                    |SubstructureRedirectMask|SubstructureNotifyMask;
	
	// Create Functions and Events memory
	this->func = new Functions; this->event = new Events;
	this->func->init(this, this->event); this->event->init(this, this->func);
	// Create selmon memory
	this->selmon = new Monitor;
	this->selmon->clients = new LinkedListClient;

	// Assignment some important things
	this->root = DefaultRootWindow(this->dpy);
	
	/* Grab Keys */
	// Yang Dilakukan, untuk mengambil dan mengetahui rentang keycodes yang dapat digunakan nantinya di grab
	int start, end, k, skip;
	KeySym *syms = nullptr;
	XUngrabKey(this->dpy, AnyKey, AnyModifier, this->root);
	XDisplayKeycodes(this->dpy, &start, &end);
	syms = XGetKeyboardMapping(this->dpy, start, end - start + 1, &skip);
	if(!syms) return;
	
	// Grab the keys
	for(k = start; k <= end; k++)
		for(const Key &key : *this->p_keys)
			if(key.keysym == syms[ (k - start) * skip])
				XGrabKey(this->dpy, (KeyCode)k, key.mod, this->root, true, GrabModeAsync, GrabModeAsync);

	XSelectInput(this->dpy, this->root, event_mask);
}

void kawigrwm::run(){
	XEvent event;
	while(this->running){
		XNextEvent(this->dpy, &event);
		switch(event.type){
		case KeyPress     : this->event->keypress(event);debugme(0, "keypress event\n");break;
		case MapRequest   : this->event->maprequest(event);debugme(0, "maprequest event\n");break;
		case DestroyNotify: this->event->destroynotify(event);debugme(0, "destorynotify\n");break;
		}
	}
}

void kawigrwm::manage(Window &w, XWindowAttributes &wa){
	Client *c = this->selmon->clients->createNewClient();
	this->selmon->clients->assign(c, w, wa);
	XMapWindow(this->dpy, w);
	focus(c);
	debugme(0, "manage a client\n");
}

void kawigrwm::unmanage(Client *c){
	this->selmon->clients->deleteClient(c);
	XSync(this->dpy, false);
	debugme(0, "unmanage a client\n");
}

void kawigrwm::focus(Client *c){
	if(c) XSetInputFocus(this->dpy, c->win, RevertToPointerRoot, CurrentTime);
}
