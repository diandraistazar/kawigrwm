#include "../include/main.hpp"

Layout::Layout(Variables *global) : global(global){
	// no statement for now
}

void Layout::horizontal_tiling(Monitor *selmon, ClientTG *tagsel){
	auto &g = this->global;

	int base = g->width_m / tagsel->count;
	int remains = g->width_m % tagsel->count;
	int i = 0;

	for(Client *selected = tagsel->client_head; selected; selected = selected->next){
		selected->width = base + ((int)tagsel->count == i+1 ? remains : 0);
		selected->height = g->height_m;
		
		selected->x = base * i;
		selected->y = 0;

		i++;
		XMoveResizeWindow(g->dpy, selected->win, selected->x, selected->y, selected->width, selected->height);
	}
}
