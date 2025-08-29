#include "../include/main.hpp"

Layout::Layout(Variables *global) : global(global){
	layouts[TILING_HORIZONTAL] = &Layout::horizontal_tiling;
	layouts[TILING_VERTICAL] = &Layout::vertical_tiling;
}

void Layout::horizontal_tiling(ClientTG *tagsel){
	auto &g = this->global;
	auto &config = g->config;

	int base = g->width_m / tagsel->count;
	int remains = g->width_m % tagsel->count;
	int i = 0;

	for(Client *selected = tagsel->client_head; selected; selected = selected->next){
		selected->width = base + ((int)tagsel->count == i+1 ? remains : 0) - (config->gaps * 2);
		selected->height = g->height_m - (config->gaps * 2);
		selected->x = base * i + config->gaps;
		selected->y = config->gaps;

		XMoveResizeWindow(g->dpy, selected->win, selected->x, selected->y, selected->width, selected->height);
		i++;
	}
}

void Layout::vertical_tiling(ClientTG *tagsel){
	auto &g = this->global;
	auto &config = g->config;

	int base = g->height_m / tagsel->count;
	int remains = g->height_m % tagsel->count;
	int i = tagsel->count;

	for(Client *selected = tagsel->client_head; selected; selected = selected->next){
		selected->width = g->width_m - (config->gaps * 2);
		selected->height = base + ((int)tagsel->count == i ? remains : 0) - (config->gaps * 2);
		selected->x = config->gaps;
		selected->y = base * (i-1) + config->gaps;
		XMoveResizeWindow(g->dpy, selected->win, selected->x, selected->y, selected->width, selected->height);
		i--;
	}
}
