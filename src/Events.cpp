#include "../include/main.hpp"

/* Events class */
Events::Events(Variables *global) : global(global){
	// No statement for now
}

void Events::keypress(XKeyEvent &event){
	auto &g = this->global;
	auto &functions = g->func;
	auto &config = g->config;
	
	KeySym sym = XKeycodeToKeysym(g->dpy, (KeyCode)event.keycode, 0);
	for(const Key &key : config->keys)
		if(key.mod == event.state && key.keysym == sym)
			switch(key.code){
				case SPAWN   : functions->spawn(key.args);break;
				case FOCUS   : functions->adjust_focus(key.args);break;
				case MOVWIN  : functions->move_win_to_another_workspace(key.args);break;
				case CHGWORK : functions->change_workspace(key.args);break;
				case CHGLAY  : functions->change_layout(key.args);break;
				case KILL    : functions->kill();break;
				case EXIT    : functions->exitman();break;
			}
}

void Events::buttonpress(XButtonEvent &event){
	auto &g = this->global;
	auto &functions = g->func;
	auto &config = g->config;
	
	for(const Button &button : config->buttons)
		if(button.mod == event.state && button.button == event.button)
			switch(button.code){
				case MOVRESZ  : functions->move_and_resize_window(button.args);break;
			}
}

void Events::maprequest(XMapRequestEvent &event){
	auto &g = this->global;
	auto &manager = g->man;
	auto &clients = g->clients;
	
	bool isThere = false;

	for(int select = clients->clients.size(); select > 0; select--){
		if(g->clients->findClient(event.window, clients->clients[select-1])){
			isThere = true;
			break;
		}
	}

	if(!isThere) 
		manager->manage(event.window);
}

void Events::destroynotify(XDestroyWindowEvent &event){
	auto &g = this->global;
	auto &manager = g->man;
	auto &clients = g->clients;
	
	Client *c = nullptr;
	if((c = g->clients->findClient(event.window, clients->clients[g->selmon->tag-1])))
		manager->unmanage(c);
}

void Events::enternotify(XCrossingEvent &event){
	auto &g = this->global;
	auto &selmon = g->selmon;
	auto &manager = g->man;
	auto &clients = g->clients;
	
	Client *c;
	if(selmon->select && (event.window == selmon->select->win)) return;
	c = event.window != g->root
		? g->clients->findClient(event.window, clients->clients[g->selmon->tag-1])
		: nullptr;
	manager->focus(c);
}
