/* TODO */
// add another functions (Working) 
	// allow exitwm (DONE)
	// allow spawn window (DONE)
	// allow move window (DONE)
	// allow resize window (DONE)
	// allow change another workspace (tags)
// allow adjust focus window, even with pointer (DONE)
// Dynamic Manager (Tiling and stack)
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
