#include "../include/main.hpp"

Layout::Layout(Variables *global) : global(global){
	// no statement for now
}

// Tiling like i3-wm
void Layout::tiling_1(Monitor *selmon, ClientTG *tagsel){
	auto &g = this->global;

	int i = 0, point = 0;

	for(Client *selected = tagsel->client_head; selected; selected = selected->next){
		selected->width = g->width_m / tagsel->count + g->width_m % tagsel->count;
		selected->height = g->height_m;

		point = selected->width * i;
		selected->x = point;
		selected->y = 0;
		i += 1;
		XMoveResizeWindow(g->dpy, selected->win, selected->x, selected->y, selected->width, selected->height);
	}
}
