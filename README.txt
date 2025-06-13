The top level directories are project files. Each project file contains a meson build. There exists a script in the build directory build/data/rebuild.zsh that will delete all the data files, recompile the source, and generate a gif of the Ey field.

Dependencies:
	zsh
	meson
	meep
	imagemagick
	h5utils
