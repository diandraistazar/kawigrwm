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
