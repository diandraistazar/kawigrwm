#include "../include/main.hpp"

/* Functions class */
Functions::Functions(std::unique_ptr<Variables> &global) : global(global){
	// No statement for now
}

void Functions::spawn(const Arg &args){
	pid_t pid = fork();
	if(pid == 0){
		execvp((char*)args.v[0], (char**)args.v);
		this->global->man->err_mass("Can't open the program");
	}
}

void Functions::exitman(){
	this->global->running = false;
}

void Functions::kill(){
	if(!this->global->selmon->select) return;
	
	XKillClient(this->global->dpy, this->global->selmon->select->win);
	this->global->man->unmanage(this->global->selmon->select);
}

void Functions::movewindow(){
	int x, y, lasttime = 0;
	XEvent event;
	auto move = [this, &x, &y, &event](){
		x = event.xmotion.x_root - this->global->selmon->select->x;
		y = event.xmotion.y_root - this->global->selmon->select->y;
		this->global->selmon->select->x += x - (this->global->selmon->select->width / 2);
		this->global->selmon->select->y += y - (this->global->selmon->select->height / 2);
		XMoveWindow(this->global->dpy, this->global->selmon->select->win,\
			       this->global->selmon->select->x,\
			       this->global->selmon->select->y);

	};
	
	XGrabPointer(this->global->dpy, this->global->root, false,\
				    PointerMotionMask|ButtonPressMask|ButtonReleaseMask,\
				    GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
	do{
	XMaskEvent(this->global->dpy, PointerMotionMask|ButtonPressMask|ButtonReleaseMask, &event);
	switch(event.type){
	case MotionNotify:
		if((event.xmotion.time - lasttime) <= 12)
			continue;
		lasttime = event.xmotion.time;
		move();
		break;
	}
	}while(event.type != ButtonRelease);
	XUngrabPointer(this->global->dpy, CurrentTime);
}

void Functions::resizewindow(){
	int x, y, width_t, height_t, lasttime = 0;
	XEvent event;
	// Ini lambda, memungkinkan mendefinisikan fungsi langsung di tempat penggunaannya, tanpa harus membuatnya secara terpisah
	auto resize = [this, &event, &x, &y, &width_t, &height_t](){
		x = event.xmotion.x_root - this->global->selmon->select->x;
		y = event.xmotion.y_root - this->global->selmon->select->y;
		width_t = this->global->selmon->select->width;
		height_t = this->global->selmon->select->height;
		
		((width_t += x - (width_t / 2)) > 100)\
		? this->global->selmon->select->width = width_t\
		: 0;
		
		((height_t += y - (height_t / 2)) > 100)\
		? this->global->selmon->select->height = height_t\
		: 0;
		XResizeWindow(this->global->dpy, this->global->selmon->select->win,\
			       this->global->selmon->select->width,\
			       this->global->selmon->select->height);

	};
	
	XGrabPointer(this->global->dpy, this->global->root, false,\
				    PointerMotionMask|ButtonPressMask|ButtonReleaseMask,\
				    GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
	do{
	XMaskEvent(this->global->dpy, PointerMotionMask|ButtonPressMask|ButtonReleaseMask, &event);
	switch(event.type){
	case MotionNotify:
		if((event.xmotion.time - lasttime) <= 12)
			continue;
		lasttime = event.xmotion.time;
		resize();
		break;
	}
	}while(event.type != ButtonRelease);
	XUngrabPointer(this->global->dpy, CurrentTime);
}

void Functions::adjustfocus(const Arg &args){
	Client *temp = nullptr;
	if(!this->global->selmon->select) return;
	if(args.i == -1)
		temp = this->global->selmon->select->back\
	           ? this->global->selmon->select->back\
		       : this->global->selmon->clients->client_tail;
	else if(args.i == 1)
		temp = this->global->selmon->select->next\
			   ? this->global->selmon->select->next\
		       : this->global->selmon->clients->client_head;
	else return;	
	// Membungkus pointer dengan area, jika mencapai batas area, kita dapat memindahkan posisi pointer ke dst_x & dst_y
	// Pokoknya, pointer di bungkus dalam suatu area, dan dapat memanipulasi pergerakan pointer di area tersebut
	XWarpPointer(this->global->dpy, None, temp->win,\
			     0, 0, 0, 0,\
			     temp->width / 2, temp->height /2);
	this->global->man->focus(temp);
}
