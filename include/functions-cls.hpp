void Functions::init(kawigrwm *wm, Events *event){
	this->wm = wm;
	this->event = event;
}

void Functions::spawn(const Arg &args){
	pid_t pid = fork();
	if(pid == 0){
		execvp((char*)args.v[0], (char**)args.v);
		this->wm->err_mass("Can't open the program");
	}
}

void Functions::exitwm(){
	this->wm->running = false;
}

void Functions::kill(){
	if(!this->wm->selmon->select) return;
	
	XKillClient(this->wm->dpy, this->wm->selmon->select->win);
	this->wm->unmanage(this->wm->selmon->select);
}

void Functions::movewindow(){
	int x, y, lasttime = 0;
	XEvent event;
	
	XGrabPointer(this->wm->dpy, this->wm->root, false,\
				    PointerMotionMask|ButtonPressMask|ButtonReleaseMask,\
				    GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
	do{
	XMaskEvent(this->wm->dpy, PointerMotionMask|ButtonPressMask|ButtonReleaseMask, &event);
	switch(event.type){
	case MotionNotify:
		if((event.xmotion.time - lasttime) <= 12)
			continue;
		lasttime = event.xmotion.time;
		x = event.xmotion.x_root - this->wm->selmon->select->x;
		y = event.xmotion.y_root - this->wm->selmon->select->y;
		this->wm->selmon->select->x += x - (this->wm->selmon->select->width / 2);
		this->wm->selmon->select->y += y - (this->wm->selmon->select->height / 2);
		XMoveWindow(this->wm->dpy, this->wm->selmon->select->win,\
			       this->wm->selmon->select->x,\
			       this->wm->selmon->select->y);
		break;
	}
	}while(event.type != ButtonRelease);
	XUngrabPointer(this->wm->dpy, CurrentTime);
}
