void Events::init(kawigrwm *wm, Functions *func){
	this->wm = wm;
	this->func = func;
}

void Events::keypress(XKeyEvent &event){
	KeySym sym = XKeycodeToKeysym(this->wm->dpy, (KeyCode)event.keycode, 0);

	for(const Key &key : *this->wm->p_keys)
		if(key.mod == event.state && key.keysym == sym)
			switch(key.code){
				case SPAWN: this->func->spawn(key.args);break;
				case KILL : this->func->kill();break;
				case FOCUS: this->func->adjustfocus(key.args);break;
				case EXIT : this->func->exitwm();break;
			}
}

void Events::buttonpress(XButtonEvent &event){
	for(const Button &button : *this->wm->p_buttons)
		if(button.mod == event.state && button.button == event.button)
			switch(button.code){
				case MOVE : this->func->movewindow();break;
			}
}

void Events::maprequest(XMapRequestEvent &event){
	XWindowAttributes wa;
	
	if( !XGetWindowAttributes(this->wm->dpy, event.window, &wa) ) return;
	if(!this->wm->selmon->clients->findClient(event.window))
		this->wm->manage(event.window, wa);
}

void Events::destroynotify(XDestroyWindowEvent &event){
	Client *c = nullptr;
	if((c = this->wm->selmon->clients->findClient(event.window)))
		this->wm->unmanage(c);
}
