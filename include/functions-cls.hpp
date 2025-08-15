void Functions::init(kawigrwm *wm, Events *event){
	this->wm = wm;
	this->event = event;
}

void Functions::spawn(const Arg &args){
	debugme(0, "spawn\n");
	pid_t pid = fork();
	if(pid == 0){
		execvp((char*)args.v[0], (char**)args.v);
		this->wm->err_mass("Can't open the program");
	}
}

void Functions::exitwm(){
	debugme(0, "exitwm\n");
	this->wm->running = false;
}

void Functions::kill(){
	if(!this->wm->selmon->select) return;
	
	XKillClient(this->wm->dpy, this->wm->selmon->select->win);
	XSync(this->wm->dpy, false);

	this->wm->unmanage(this->wm->selmon->select);
	debugme(0, "killed a window\n");
}
