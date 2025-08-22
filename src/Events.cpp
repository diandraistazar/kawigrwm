#include "../include/main.hpp"

/* Events class */
Events::Events(Variables *global) : global(global){
	// No statement for now
}

void Events::keypress(XKeyEvent &event){
	auto &g = this->global;
	auto &functions = g->func;
	
	KeySym sym = XKeycodeToKeysym(g->dpy, (KeyCode)event.keycode, 0);
	for(const Key &key : g->config->keys)
		if(key.mod == event.state && key.keysym == sym)
			switch(key.code){
				case SPAWN: functions->spawn(key.args);break;
				case KILL : functions->kill();break;
				case FOCUS: functions->adjustfocus(key.args);break;
				case EXIT : functions->exitman();break;
			}
}

void Events::buttonpress(XButtonEvent &event){
	auto &g = this->global;
	auto &functions = g->func;
	
	for(const Button &button : g->config->buttons)
		if(button.mod == event.state && button.button == event.button)
			switch(button.code){
				case MOVRESZ  : functions->movresz(button.args);break;
			}
}

void Events::maprequest(XMapRequestEvent &event){
	auto &g = this->global;
	auto &manager = g->man;
	
	if(!g->clients->findClient(event.window))
		manager->manage(event.window);
}

void Events::destroynotify(XDestroyWindowEvent &event){
	auto &g = this->global;
	auto &manager = g->man;
	
	Client *c = nullptr;
	if((c = g->clients->findClient(event.window)))
		manager->unmanage(c);
}

void Events::enternotify(XCrossingEvent &event){
	auto &g = this->global;
	auto &selmon = g->selmon;
	auto &manager = g->man;
	
	Client *c = nullptr;
	if(selmon->select && event.window == selmon->select->win) return;
	c = event.window != g->root
		? g->clients->findClient(event.window)
		: nullptr;
	manager->focus(c);
}
