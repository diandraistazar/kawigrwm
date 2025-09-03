#include "../include/main.hpp"

/* Functions class */
Functions::Functions(Variables *global) : global(global){
	// No statement for now
}

void Functions::spawn(const Arg &args){
	auto &manager = this->global->man;
	
	const char *string[] = { "sh", "-c", (char*)args.s, nullptr };
	pid_t pid = fork();
	if(pid == 0){
		execvp(string[0], (char**)string);
		manager->err_msg("Can't open the program");
	}
}

void Functions::exitman(){
	auto &g = this->global;
	g->running = false;
}

void Functions::kill(){
	auto &g = this->global;
	auto &selmon = g->selmon;
	auto &manager = g->man;
	
	if(!selmon->select) return;

	XKillClient(g->dpy, selmon->select->win);
	manager->unmanage(selmon->select);
}

void Functions::move_and_resize_window(const Arg &args){
	auto &g = this->global;
	auto &selmon = g->selmon;
	
	if(!selmon->select) return;

	// deklerasi variabel selected tipe std::functions, yang berfungsi menyimpan berbagai object yang dapat dipanggil,
	// seperti functions, lambda, method, atau pointer fungsi
	// Intinya, dengan ini ente dapat menyimpan berbagai callable , sehingga dapat mencapai polymorphism
	int prev_x_root = 0, prev_y_root = 0;
	int width_temp, height_temp;
	int lasttime = 0;
	int dummy_i;
	unsigned int maskval;
	XEvent event;
	Window dummy_w;
	std::function<void()> selected;
	
	switch(args.i){
		case -1: // Move Window
			selected = [&g, &selmon, &prev_x_root, &prev_y_root, &event](){
			selmon->select->x += event.xmotion.x_root - prev_x_root;
			selmon->select->y += event.xmotion.y_root - prev_y_root;
			prev_x_root = event.xmotion.x_root;
			prev_y_root = event.xmotion.y_root;
			XMoveWindow(g->dpy, selmon->select->win, selmon->select->x, selmon->select->y);
			}; break;
		case 1: // Resize Window
			selected = [&g, &selmon, &event, &prev_x_root, &prev_y_root, &width_temp, &height_temp](){
			width_temp = selmon->select->width;
			height_temp = selmon->select->height;
	
			width_temp +=  event.xmotion.x_root - prev_x_root;
			height_temp += event.xmotion.y_root - prev_y_root;

			prev_x_root = event.xmotion.x_root;
			prev_y_root = event.xmotion.y_root;
					
			if(width_temp > 100) selmon->select->width = width_temp;
			if(height_temp > 100) selmon->select->height = height_temp;
		
			XResizeWindow(g->dpy, selmon->select->win, selmon->select->width, selmon->select->height);
			}; break;
	}
	 
	XQueryPointer(g->dpy, g->root, &dummy_w, &dummy_w, &prev_x_root, &prev_y_root, &dummy_i, &dummy_i, &maskval);
	XGrabPointer(g->dpy, g->root, false,
				 PointerMotionMask|ButtonPressMask|ButtonReleaseMask,
				 GrabModeAsync, GrabModeAsync, None, None, CurrentTime);

	do{
	XMaskEvent(g->dpy, PointerMotionMask|ButtonPressMask|ButtonReleaseMask, &event);
	switch(event.type){
	case MotionNotify:
		if((event.xmotion.time - lasttime) <= 12)
			continue;
		lasttime = event.xmotion.time;
		selected();
		break;
	}
	}while(event.type != ButtonRelease);
	XUngrabPointer(g->dpy, CurrentTime);	
}

void Functions::adjust_focus(const Arg &args){
	auto &g = this->global;
	auto &selmon = g->selmon;
	auto &manager = g->man;
	auto &clients = g->clients;
	
	ClientTG *current_tag = clients->clients[selmon->tag-1];
	Client *temp = nullptr;

	if(selmon->select){
		if(args.i == -1)
			temp = selmon->select->back ? selmon->select->back : current_tag->client_tail;
		else if(args.i == 1 )
			temp = selmon->select->next ? selmon->select->next : current_tag->client_head;
	}
	else
		temp = current_tag->client_head;

	if(!temp || temp == selmon->select) return;
	// Membungkus pointer dengan area, jika mencapai batas area, kita dapat memindahkan posisi pointer ke dst_x & dst_y
	// Pokoknya, pointer di bungkus dalam suatu area, dan dapat memanipulasi pergerakan pointer di area tersebut
	manager->warp_pointer(temp->win, temp->width/2, temp->height/2);
	manager->focus(temp);
}

void Functions::change_workspace(const Arg &args){
	auto &g = this->global;
	auto &selmon = g->selmon;
	auto &manager = g->man;
	auto &clients = g->clients;

	int tag_temp = selmon->tag;
	
	switch(args.i){
		case -2: tag_temp -= 1;
			break;
		case -1: tag_temp += 1;
			break;
		default: tag_temp = args.i;
			break;
	}

	if(tag_temp == (int)selmon->tag
	   || tag_temp < 1
	   || tag_temp > (int)clients->clients.size()) return;

	manager->map_or_unmap("unmap", clients->clients[selmon->tag-1]);
	manager->map_or_unmap("map", clients->clients[tag_temp-1]);
	selmon->tag = tag_temp;
	manager->arrange_window();
}

void Functions::move_win_to_another_workspace(const Arg &args){
	auto &g = this->global;
	auto &manager = g->man;
	auto &selmon = g->selmon;
	auto &clients = g->clients;

	// checking
	if(!selmon->select
	   || args.i < 1
	   || args.i > (int)clients->clients.size())
		return;

	g->clients->moveClientToAnotherTag(selmon->select, args.i);
	XUnmapWindow(g->dpy, selmon->select->win);
	manager->arrange_window();
}

void Functions::change_layout(const Arg &args){
	auto &g = this->global;
	auto &manager = g->man;
	auto &config = g->config;
	auto &selmon = g->selmon;

	int code = selmon->layout + args.i;
	if(code == selmon->layout
	   || code < 0
	   || code > (int)config->layouts.size()-1 )
		return;
	selmon->layout = config->layouts[code];
	manager->arrange_window();
}

void Functions::adjust_gaps_size(const Arg &args){
	auto &g = this->global;
	auto &manager = g->man;
	auto &config = g->config;

	config->gaps += args.i;

	if(config->gaps < config->minmax_gaps[0])
		config->gaps = config->minmax_gaps[0];
	else if(config->gaps > config->minmax_gaps[1])
		config->gaps = config->minmax_gaps[1];

	manager->arrange_window();
}
