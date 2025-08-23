#include "../include/main.hpp"

/* Functions class */
Functions::Functions(Variables *global) : global(global){
	// No statement for now
}

void Functions::spawn(const Arg &args){
	auto &manager = this->global->man;
	
	pid_t pid = fork();
	if(pid == 0){
		execvp((char*)args.v[0], (char**)args.v);
		manager->err_mass("Can't open the program");
	}
}

void Functions::exitman(){
	this->global->running = false;
}

void Functions::kill(){
	auto &g = this->global;
	auto &selmon = g->selmon;
	auto &manager = g->man;
	
	if(!selmon->select) return;

	XKillClient(g->dpy, selmon->select->win);
	manager->unmanage(selmon->select);
}

void Functions::movresz(const Arg &args){
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
	
	/*move window*/
	if(args.i == -1){
		selected = [&g, &selmon, &x_temp, &y_temp, &x_root_temp, &y_root_temp, &event](){
		x_temp = event.xmotion.x_root - x_root_temp;
		y_temp = event.xmotion.y_root - y_root_temp;
		
		x_root_temp = event.xmotion.x_root;
		y_root_temp = event.xmotion.y_root;

		selmon->select->x += x_temp;
		selmon->select->y += y_temp;
		XMoveWindow(g->dpy, selmon->select->win,
		            selmon->select->x,
		            selmon->select->y);
 	 	};
	 }
	 /*resize window*/
	 else if(args.i == 1){
		selected = [&g, &selmon, &event, &x_temp, &y_temp, &x_root_temp, &y_root_temp, &width_temp, &height_temp](){
		x_temp = event.xmotion.x_root - x_root_temp;
		y_temp = event.xmotion.y_root - y_root_temp;

		x_root_temp = event.xmotion.x_root;
		y_root_temp = event.xmotion.y_root;

		width_temp = selmon->select->width;
		height_temp = selmon->select->height;
	
		/*below are ternary operator*/
		((width_temp += x_temp) > 100) ? selmon->select->width = width_temp : 0;
		((height_temp += y_temp) > 100) ? selmon->select->height = height_temp : 0;
		
		XResizeWindow(g->dpy, selmon->select->win,
		              selmon->select->width,
		              selmon->select->height);
		};
	 }
	 else return;
	 
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

void Functions::adjustfocus(const Arg &args){
	auto &g = this->global;
	auto &selmon = g->selmon;
	auto &manager = g->man;
	
	Client *temp = nullptr;
	
	if(selmon->select){
		if(args.i == -1)
			temp = selmon->select->back ? selmon->select->back : g->clients->clients[selmon->tag-1]->client_tail;
		else if(args.i == 1)
			temp = selmon->select->next ? selmon->select->next : g->clients->clients[selmon->tag-1]->client_head;
		else return;
	}
	else if(!selmon->select && (args.i == -1 || args.i == 1))
		temp = g->clients->clients[selmon->tag-1]->client_head;

	// Membungkus pointer dengan area, jika mencapai batas area, kita dapat memindahkan posisi pointer ke dst_x & dst_y
	// Pokoknya, pointer di bungkus dalam suatu area, dan dapat memanipulasi pergerakan pointer di area tersebut
	XWarpPointer(g->dpy, None, temp->win,
				0, 0, 0, 0,
				temp->width / 2, temp->height /2);
	manager->focus(temp);
}

void Functions::changeworkspace(const Arg &args){
	auto &g = this->global;
	auto &selmon = g->selmon;

	if(selmon->tag == (unsigned int)args.i) return;

	ClientTG *current_tag;
	Client *temp;
	auto map_or_unmap = [&temp, &current_tag, &g](const std::string &arg){
		for(temp = current_tag->client_head; temp; temp = temp->next)
			if(arg == "map")
				XMapWindow(g->dpy, temp->win);
			else if(arg == "unmap")
				XUnmapWindow(g->dpy, temp->win);
	};

	current_tag = g->clients->clients[selmon->tag-1];
	map_or_unmap("unmap");

	// Update selmon tag status
	selmon->tag = args.i;
	current_tag = g->clients->clients[selmon->tag-1];
	map_or_unmap("map");
}
