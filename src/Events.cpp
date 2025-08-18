#include "../include/main.hpp"

/* Events class */
Events::Events(std::unique_ptr<Variables> &global) : global(global){
	// No statement for now
}

void Events::keypress(XKeyEvent &event){
	KeySym sym = XKeycodeToKeysym(this->global->dpy, (KeyCode)event.keycode, 0);

	for(const Key &key : this->global->man->p_keys)
		if(key.mod == event.state && key.keysym == sym)
			switch(key.code){
				case SPAWN: this->global->func->spawn(key.args);break;
				case KILL : this->global->func->kill();break;
				case FOCUS: this->global->func->adjustfocus(key.args);break;
				case EXIT : this->global->func->exitman();break;
			}
}

void Events::buttonpress(XButtonEvent &event){
	for(const Button &button : this->global->man->p_buttons)
		if(button.mod == event.state && button.button == event.button)
			switch(button.code){
				case MOVRESZ  : this->global->func->movresz(button.args);break;
			}
}

void Events::maprequest(XMapRequestEvent &event){
	XWindowAttributes wa;
	
	if( !XGetWindowAttributes(this->global->dpy, event.window, &wa) ) return;
	if(!this->global->selmon->clients->findClient(event.window))
		this->global->man->manage(event.window, wa);
}

void Events::destroynotify(XDestroyWindowEvent &event){
	Client *c = nullptr;
	if((c = this->global->selmon->clients->findClient(event.window)))
		this->global->man->unmanage(c);
}
