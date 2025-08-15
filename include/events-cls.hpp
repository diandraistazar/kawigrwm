void Events::init(kawigrwm *wm, Functions *func){
	this->wm = wm;
	this->func = func;
}

void Events::keypress(XEvent &event){
	XKeyPressedEvent *e = &event.xkey;
	KeySym sym = XKeycodeToKeysym(this->wm->dpy, (KeyCode)e->keycode, 0);

	for(const Key &key : *this->wm->p_keys)
		if(key.mod == e->state && key.keysym == sym)
			switch(key.code){
				case SPAWN: this->func->spawn(key.args);break;
				case KILL : this->func->kill();break;
				case EXIT : this->func->exitwm();break;
			}
}

void Events::maprequest(XEvent &event){
	XMapRequestEvent *e = &event.xmaprequest;
	XWindowAttributes wa;
	
	if( !XGetWindowAttributes(this->wm->dpy, e->window, &wa) ) return;
	if(!this->wm->selmon->clients->findClient(e->window))
		this->wm->manage(e->window, wa);
}

void Events::destroynotify(XEvent &event){
	XDestroyWindowEvent *e = &event.xdestroywindow;
	Client *c = nullptr;
	
	if((c = this->wm->selmon->clients->findClient(e->window)))
		this->wm->unmanage(c);
}
