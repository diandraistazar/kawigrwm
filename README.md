# Kawigrwm

**Kawigrwm** is a window manager based on xlib X11, which written in C++ and built for specific needs. Kawigrwm includes some features from various window managers i have ever used before.

## How to Install
To install the program, we use ```Make``` to build. 
```
git clone https://github.com/diandraistazar/kawigrwm.git    # clone this repository first
cd kawigrwm               # then, go into the directory
git tag                   # before build, you need to checkout to new version or desired version 
git checkout <tagname>    # then, checkout it
make                      # build the program (only build)
sudo make install         # or, build and install to system (requires root)
sudo make clean uninstall # to uninstall program (requires root)
kawigrwm                  # add kawigrwm to ~/.xinitrc and run startx
```

## Configuration
To costumize and configure kawigrwm, we use ```config.hpp``` in ```./include/``` directory, just modify it like dwm
#### "Config.hpp" example snippet
```
/* Gaps */
unsigned int gaps = 8;

/* Tags */
const unsigned int default_tag = 1;
const unsigned int tags = 4;

/*Layouts*/
const LayoutCODE default_layout = TILING_HORIZONTAL;
const vector<LayoutCODE> layouts = {
	TILING_HORIZONTAL,
	TILING_VERTICAL,
};

...
```
