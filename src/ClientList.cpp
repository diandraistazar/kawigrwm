#include "../include/main.hpp"

/* ClientList */
ClientList::ClientList(Variables *global) : global(global){
	auto &g = this->global;
	
	for(int index = g->config->tags; index; index--)
		clients.push_back(new ClientTG);
}

void ClientList::cleanup(){
	auto &g = this->global;
	auto &config = g->config;

	for(int workspace = config->tags-1; workspace >= 0; workspace--){
		ClientTG *current_tag = clients[workspace];
		Client *current = current_tag->client_head;
		
		while(current){
			Client *temp = current;
			current = current->next;
			delete temp;
		}
		
		delete current_tag;
		clients.pop_back();
	}
}

Client *ClientList::createNewClient(Window w){
	auto &g = this->global;

	ClientTG *current_tag = clients[g->selmon->tag-1];
	Client *new_client = new Client;
	
	addClientToTag(new_client, current_tag);
	assign(new_client, w);
	return new_client;
}

Client *ClientList::findClient(Window w, ClientTG *current_tag){
	for(Client *temp = current_tag->client_head; temp; temp = temp->next)
			if(temp->win == w) return temp;
	return nullptr;
}

void ClientList::deleteClient(Client *c){
	auto &g = this->global;
	
	ClientTG *current_tag = clients[g->selmon->tag-1];
	removeClientFromTag(c, current_tag);
	delete c;
}

void ClientList::moveClientToAnotherTag(Client *select, int i_dest_tag){
	ClientTG *source_tag = clients[select->tag-1];
	ClientTG *dest_tag = clients[i_dest_tag-1];

	removeClientFromTag(select, source_tag);
	addClientToTag(select, dest_tag);
	select->tag = i_dest_tag;
}

/* Private */
void ClientList::removeClientFromTag(Client *select, ClientTG *tagsel){
	Client *back, *next;

	if(!tagsel || !tagsel->client_head) return;

	back = select->back;
	next = select->next;

	if(back) back->next = next;
	else tagsel->client_head = next;

	if(next) next->back = back;
	else tagsel->client_tail = back;

	select->back = nullptr;
	select->next = nullptr;
	tagsel->count--;
}

void ClientList::addClientToTag(Client *select, ClientTG *tagsel){
	if(!tagsel) return;

	if(tagsel->client_head){
		tagsel->client_tail->next = select;
		select->back = tagsel->client_tail;
	}
	else
		tagsel->client_head = select;

	tagsel->client_tail = select;
	tagsel->count++;
}

void ClientList::assign(Client *c, Window w){
	auto &g = this->global;

	if(!c) return;

	XWindowAttributes wa;
	XGetWindowAttributes(g->dpy, w, &wa);
	
	c->win = w;
	c->root = wa.root;
	c->tag = g->selmon->tag;
	c->x = wa.x;
	c->y = wa.y;
	c->width = g->width_m;
	c->height = g->height_m;
}
