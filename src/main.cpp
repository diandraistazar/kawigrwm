/* TODO */
// allow spawn window (DONE)
// allow exitwm (DONE)
// allow focus (DONE)
// add another functions (Working) 
	// allow move window (working)
	// allow resize window
	// allow change another workspace (tags)
// allow adjust focus window (DONE)
// allow patches like dwm
// be minimalist and fast

#include "../include/main.hpp"
// Configuration
#include "../include/config.hpp"

/* Main functions */
int main(){
	Manager wm(keys, buttons);
	if (!wm.open()){
		wm.err_mass("Can't open X display");
		return EXIT_FAILURE;
	}
	wm.init();
	wm.run(); 
	wm.cleanup(); 
	wm.close();
	return EXIT_SUCCESS;
}
