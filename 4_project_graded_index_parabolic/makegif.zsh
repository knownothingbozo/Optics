#!/bin/zsh

count=0

DATA_DIR="build/data"

for file in $DATA_DIR/ey*.h5
do
	h5topng -Z -o $DATA_DIR/"overlay-${(l:5::0:)count}.png" -c RdBu "$file":ey.r -A $DATA_DIR/eps*.h5 -a -yellow:0.8
	((count ++))
done

ls $DATA_DIR/*.png > $DATA_DIR/filelist.txt

GIF_DIR="build/data/gifs"
if ! [ -d "$GIF_DIR" ]; then
	mkdir $GIF_DIR
fi

magick -delay 5 -loop 0 @$DATA_DIR/filelist.txt -coalesce -layers Optimize $GIF_DIR/$(date +%d.%m.%y-%H:%M:%S).gif

