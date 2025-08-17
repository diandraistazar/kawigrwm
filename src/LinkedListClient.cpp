#include "../include/main.hpp"

/* LinkedListClient */
void LinkedListClient::assign(Client *c, Window w, XWindowAttributes &wa){
	if(!c) return;
	c->win = w;
	c->root = wa.root;
	c->x = wa.x;
	c->y = wa.y;
	c->width = wa.width;
	c->height = wa.height;
}

void LinkedListClient::cleanup(){
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

Client *LinkedListClient::createNewClient(){
	Client *c = new Client;
	if(!client_head) client_head = c;	
	else{
		client_tail->next = c;
		c->back = client_tail;
	}
	client_tail = c;
	count++;
	return c;
}

Client *LinkedListClient::findClient(Window w){
	Client *temp = nullptr;
	for(temp = client_head; temp; temp = temp->next)
			if(temp->win == w) return temp;
	return nullptr;
}

void LinkedListClient::deleteClient(Client *c){
	Client *temp = nullptr, *next = nullptr, *back = nullptr;
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
