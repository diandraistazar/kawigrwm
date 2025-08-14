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
	return this->dpy;
}

void kawigrwm::close(){
	delete func; delete event;
	this->client.cleanup();
	XUngrabKey(this->dpy, AnyKey, AnyModifier, this->root);

	XCloseDisplay(this->dpy);
	debugme(0, "Window manager closed\n");
}

void kawigrwm::init(){
	unsigned int event_mask = KeyPressMask|ButtonPressMask|PointerMotionMask\
			                    |SubstructureRedirectMask;
	
	this->func = new Functions; this->event = new Events;
	this->func->init(this, this->event); this->event->init(this, this->func);
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
		case KeyPress  : this->event->keypress(event);debugme(0, "keypress event\n");break;
		case MapRequest: this->event->maprequest(event);debugme(0, "maprequest event\n");break;
		}
	}
}

void kawigrwm::manageClient(Window &w, XWindowAttributes &wa){
	Client *c = this->client.createNewClient();
	this->client.assign(c, w, wa);
	XMapWindow(this->dpy, w);
}
