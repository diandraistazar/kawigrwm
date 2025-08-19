#include "../include/main.hpp"

/* Functions class */
Functions::Functions(std::unique_ptr<Variables> &global) : global(global){
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
	int x = 0, y = 0;
	int width_temp = 0, height_temp = 0;
	int lasttime = 0;
	std::function<void()> selected;
	XEvent event;
	
	/*move window*/
	if(args.i == -1){
		selected = [&g, &selmon, &x, &y, &event](){
		x = event.xmotion.x_root - selmon->select->x;
		y = event.xmotion.y_root - selmon->select->y;
		selmon->select->x += x - (selmon->select->width / 2);
		selmon->select->y += y - (selmon->select->height / 2);
		XMoveWindow(g->dpy, selmon->select->win,
		            selmon->select->x,
		            selmon->select->y);
 	 	};
	 }
	 /*resize window*/
	 else if(args.i == 1){
		selected = [&g, &selmon, &event, &x, &y, &width_temp, &height_temp](){
		x = event.xmotion.x_root - selmon->select->x;
		y = event.xmotion.y_root - selmon->select->y;
		width_temp = selmon->select->width;
		height_temp = selmon->select->height;
	
		/*below are ternary operator*/
		((width_temp += x - (width_temp / 2)) > 100)
		? selmon->select->width = width_temp
		: 0;
		((height_temp += y - (height_temp / 2)) > 100)
		? selmon->select->height = height_temp
		: 0;
		
		XResizeWindow(g->dpy, selmon->select->win,
		              selmon->select->width,
		              selmon->select->height);
		};
	 }
	 
	XGrabPointer(g->dpy, g->root, false,\
				 PointerMotionMask|ButtonPressMask|ButtonReleaseMask,\
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
	
	if(!selmon->select) return;
	
	Client *temp = nullptr;
	if(args.i == -1)
		temp = selmon->select->back
	           ? selmon->select->back
		       : selmon->clients->client_tail;
	else if(args.i == 1)
		temp = selmon->select->next
			   ? selmon->select->next
		       : selmon->clients->client_head;
	else return;	
	// Membungkus pointer dengan area, jika mencapai batas area, kita dapat memindahkan posisi pointer ke dst_x & dst_y
	// Pokoknya, pointer di bungkus dalam suatu area, dan dapat memanipulasi pergerakan pointer di area tersebut
	XWarpPointer(g->dpy, None, temp->win,
			     0, 0, 0, 0,
			     temp->width / 2, temp->height /2);
	manager->focus(temp);
}
