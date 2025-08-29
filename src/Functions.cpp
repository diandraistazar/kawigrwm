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
	int x_temp = 0, y_temp = 0, x_root_temp = 0, y_root_temp = 0;
	int width_temp, height_temp;
	int lasttime = 0;
	int dummy_i;
	unsigned int maskval;
	XEvent event;
	Window dummy_w;
	std::function<void()> selected;
	
	switch(args.i){
		case -1: // Move Window
			selected = [&g, &selmon, &x_temp, &y_temp, &x_root_temp, &y_root_temp, &event](){
			x_temp = event.xmotion.x_root - x_root_temp;
			y_temp = event.xmotion.y_root - y_root_temp;
		
			x_root_temp = event.xmotion.x_root;
			y_root_temp = event.xmotion.y_root;

			selmon->select->x += x_temp;
			selmon->select->y += y_temp;
			XMoveWindow(g->dpy, selmon->select->win, selmon->select->x, selmon->select->y);
			}; break;
		case 1: // Resize Window
			selected = [&g, &selmon, &event, &x_temp, &y_temp, &x_root_temp, &y_root_temp, &width_temp, &height_temp](){
			x_temp = event.xmotion.x_root - x_root_temp;
			y_temp = event.xmotion.y_root - y_root_temp;

			x_root_temp = event.xmotion.x_root;
			y_root_temp = event.xmotion.y_root;

			width_temp = selmon->select->width;
			height_temp = selmon->select->height;
	
			width_temp += x_temp;
			height_temp += y_temp;
			
			if(width_temp > 100) selmon->select->width = width_temp;
			if(height_temp > 100) selmon->select->height = height_temp;
		
			XResizeWindow(g->dpy, selmon->select->win, selmon->select->width, selmon->select->height);
			}; break;
	}
	 
	XQueryPointer(g->dpy, g->root, &dummy_w, &dummy_w, &x_root_temp, &y_root_temp, &dummy_i, &dummy_i, &maskval);
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
	
	Client *temp = nullptr;
	
	if(!selmon->select && (args.i == -1 || args.i == 1))
		temp = g->clients->clients[selmon->tag-1]->client_head;
	else
		switch(args.i){
			case -1:
				temp = selmon->select->back ? selmon->select->back : g->clients->clients[selmon->tag-1]->client_tail;
				break;
			case 1:
				temp = selmon->select->next ? selmon->select->next : g->clients->clients[selmon->tag-1]->client_head;
				break;
			default: return;
		}

	// Membungkus pointer dengan area, jika mencapai batas area, kita dapat memindahkan posisi pointer ke dst_x & dst_y
	// Pokoknya, pointer di bungkus dalam suatu area, dan dapat memanipulasi pergerakan pointer di area tersebut
	XWarpPointer(g->dpy, None, temp->win, 0, 0, 0, 0, temp->width / 2, temp->height /2);
	manager->focus(temp);
}

void Functions::change_workspace(const Arg &args){
	auto &g = this->global;
	auto &selmon = g->selmon;
	auto &manager = g->man;
	auto &clients = g->clients;
	auto &config = g->config;

	unsigned int tag_temp = selmon->tag;
	
	switch(args.i){
		case -2: tag_temp -= 1;
			break;
		case -1: tag_temp += 1;
			break;
		default: tag_temp = args.i;
			break;
	}

	if(tag_temp == selmon->tag
	   || tag_temp < 1
	   || tag_temp > config->tags) return;

	manager->map_or_unmap("unmap", clients->clients[selmon->tag-1]);
	manager->map_or_unmap("map", clients->clients[tag_temp-1]);
	selmon->tag = tag_temp;
	manager->arrange_window();
}

void Functions::move_win_to_another_workspace(const Arg &args){
	auto &g = this->global;
	auto &manager = g->man;
	auto &selmon = g->selmon;
	auto &config = g->config;
	auto &clients = g->clients;

	// checking
	if((unsigned int)args.i == selmon->tag
	   || (unsigned int)args.i > config->tags
	   || (unsigned int)args.i < 1
	   || (unsigned int)args.i-1 > clients->clients.size()
	   || !selmon->select)
		return;

	g->clients->moveClientToAnotherTag(selmon->select, (unsigned int)args.i);
	XUnmapWindow(g->dpy, selmon->select->win);
	manager->arrange_window();
}

void Functions::change_layout(const Arg &args){
	auto &g = this->global;
	auto &manager = g->man;
	auto &config = g->config;
	auto &selmon = g->selmon;

	unsigned int code = selmon->layout + args.i;
	if(code == selmon->layout || code > config->layouts.size()-1 ) return;
	selmon->layout = config->layouts[code];
	manager->arrange_window();
}
