#!/bin/zsh

count=0

POL="y"

DATA_DIR="build/data"

FILES=($DATA_DIR/e$POL*.h5)

python makegif.py

ls $DATA_DIR/*.png > $DATA_DIR/filelist.txt

GIF_DIR="build/data/gifs"
if ! [ -d "$GIF_DIR" ]; then
	mkdir $GIF_DIR
fi


echo "Making GIF"
magick -delay 5 -loop 0 @$DATA_DIR/filelist.txt $GIF_DIR/$(date +%d.%m.%y-%H:%M:%S).gif
