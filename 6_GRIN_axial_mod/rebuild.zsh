#!/bin/zsh

export LIBRARY_PATH=/usr/local/lib/:$LIBRARY_PATH
export LD_LIBRARY_PATH=/usr/local/lib/:$LD_LIBRARY_PATH

DATA_DIR="build/data"
if [ -d "$DATA_DIR" ]; then
	rm $DATA_DIR/*.h5
	rm $DATA_DIR/*.png
	rm $DATA_DIR/demo
else
	mkdir $DATA_DIR
fi

meson compile -C build

mv build/demo $DATA_DIR

cd $DATA_DIR

mpirun -np 8 demo
#./demo

cd ../..

./makegif.zsh


