#include "../include/main.hpp"

/* Main functions */
int main(){
	std::unique_ptr<Variables> global = std::make_unique<Variables>();
	
	Manager wm(global.get());
	if (!wm.open()){
		wm.err_msg("Can't open X display");
		return EXIT_FAILURE;
	}
	wm.init();
	wm.run(); 
	wm.cleanup(); 
	wm.close();
	debugme("Program closed\n");
	return EXIT_SUCCESS;
}
