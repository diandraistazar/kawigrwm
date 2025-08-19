#include "../include/main.hpp"

/* ClientList */
void ClientList::assign(Client *c, Window w, XWindowAttributes &wa){
	if(!c) return;
	c->win = w;
	c->root = wa.root;
	c->x = wa.x;
	c->y = wa.y;
	c->width = wa.width;
	c->height = wa.height;
}

void ClientList::cleanup(){
	Client *temp = nullptr;
	while(client_head){
		temp = client_head->next;
		delete client_head;
		client_head = temp;
		count--;
	}
	client_head = nullptr;
	client_head = nullptr;
}

Client *ClientList::createNewClient(){
	Client *c = new Client;
	
	if(client_head){
		client_tail->next = c;
		c->back = client_tail;
	}
	else 
		client_head = c;
	
	client_tail = c;
	count++;
	return c;
}

Client *ClientList::findClient(Window w){
	Client *temp = nullptr;
	for(temp = client_head; temp; temp = temp->next)
			if(temp->win == w) return temp;
	return nullptr;
}

void ClientList::deleteClient(Client *c){
	Client *temp = nullptr;
	Client *back = nullptr, *next = nullptr;
	// Checking first
	if(!client_head) return;
	
	for(temp = client_head; temp && temp != c; temp = temp->next);
	back = temp->back; next = temp->next;
	
	if(next) next->back = back;
	else client_tail = back;

	if(back) back->next = next;
	else client_head = next;
	
	delete temp;
	count--;
}
