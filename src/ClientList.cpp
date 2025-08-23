#include "../include/main.hpp"

/* ClientList */
ClientList::ClientList(Variables *global){
	auto &g = this->global;
	
	this->global = global;
	
	ClientTG *temp;
	for(unsigned int index = 0; index < g->config->tags; index++){
		temp = new ClientTG;
		clients.push_back(temp);
	}
}

void ClientList::assign(Client *c, Window w, XWindowAttributes &wa, Monitor *selmon){
	if(!c) return;
	c->win = w;
	c->root = wa.root;
	c->x = wa.x;
	c->y = wa.y;
	c->width = wa.width;
	c->height = wa.height;
	c->tag = selmon->tag;
}

void ClientList::cleanup(){
	auto &g = this->global;
	auto &config = g->config;

	for(int workspace = config->tags-1; workspace >= 0; workspace--){
		ClientTG *current_tag = clients[workspace];
		while(current_tag->client_head){
			Client *temp = current_tag->client_head->next;
			delete current_tag->client_head;
			current_tag->client_head = temp;
		}
		delete current_tag;
		clients.pop_back();
	}
}

Client *ClientList::createNewClient(){
	auto &g = this->global;

	ClientTG *current_tag = clients[g->selmon->tag-1];
	Client *new_client = new Client;
	
	if(current_tag->client_head){
		current_tag->client_tail->next = new_client;
		new_client->back = current_tag->client_tail;
	}
	else 
		current_tag->client_head = new_client;

	current_tag->client_tail = new_client;
	return new_client;
}

Client *ClientList::findClient(Window w){
	auto &g = this->global;

	ClientTG *current_tag = clients[g->selmon->tag-1];
	Client *temp = nullptr;
	
	for(temp = current_tag->client_head; temp; temp = temp->next)
			if(temp->win == w) return temp;
	return nullptr;
}

void ClientList::deleteClient(Client *c){
	auto &g = this->global;
	ClientTG *current_tag = clients[g->selmon->tag-1];
	Client *back, *next, *temp;

	if(!current_tag->client_head) return;
	
	for(temp = current_tag->client_head; temp && temp != c; temp = temp->next);
	back = temp->back; next = temp->next;
	
	if(next) next->back = back;
	else current_tag->client_tail = back;

	if(back) back->next = next;
	else current_tag->client_head = next;

	delete temp;
}

void ClientList::display(){
	auto &g = this->global;
	auto &config = g->config;

	for(unsigned int workspace = 0; workspace < config->tags; workspace++){
		ClientTG *current_tag = clients[workspace];
		Client *client;
		debugme("Workspace %d\n", workspace);
		for(client = current_tag->client_head; client; client = client->next){
			debugme("client_tag: %d\n", client->tag);
		}
	}
}
