#include <cmath>
#include <complex>
#include <cstdio>
#include <ctlgeom.h>
#include "ctl-math.h"
#include <iostream>
#include <meep.hpp>
#include <meep/material_data.hpp>
#include <meep/vec.hpp>
#include <math.h>
#include <meep/meepgeom.hpp>
using namespace meep;

const double pml_thickness = 0.2;

const double dt = 1.3*4;

// Multimode fiber
const double x_max = 10.0; //750.0;
const double y_max = 2.0; //125.0;
const double coreD = 1.0; // 50.0;
const double core_max = (y_max+coreD)/2;
const double core_min = (y_max-coreD)/2;

const double n0 = 2.0;
const double n1 = 1.0;

// Ge LED
const double freq = 1.0 / 0.5; // 500 nm, greenish
const double fwidth = 1.0 / 0.07; // 70 nm
const double line_src_h = 0.25*y_max;
const double pml_offset = 0.1 + pml_thickness;

const double end_time = 20.0;


double eps(const vec &p) {
	if ( p.y() <= core_max && p.y() >= core_min ) {
		return pow(n0,2)-(pow(n0,2)-pow(n1,2))*pow(p.y()- y_max/2.0,2)/pow(coreD/2.0,2);
	}
	else {
		return pow(n1, 2);
	}
}

int main(int argc, char **argv) {
	initialize mpi(argc, argv);

	// set resolution
	const double amicron = 251.0;
	
	// set up grid
	const grid_volume gv = vol2d(x_max, y_max, amicron);
	symmetry sym = -mirror(Y, gv);

	structure s(gv, eps, pml(pml_thickness), sym); //, sym, pml_layers);
	
	fields f(&s);
	
	// output dielectric layer
	f.output_hdf5(Dielectric, gv.surroundings());
	
	// create source
	continuous_src_time src ( freq );

	std::complex<double> E0[3] = { 0.0, 1.0, 0.0};
	gaussianbeam beam( vec(0.0, 0.0 ), Y, coreD/2.0, freq, n0, 1.0, E0);

	volume pnt_src(vec(pml_offset, (y_max + line_src_h) /2.0 ), vec(pml_offset, (y_max - line_src_h) /2.0));
	f.add_volume_source(src, pnt_src, beam);
	
	// step through time
	int count = 0;

	while (f.time() <= end_time) {
 		if (f.time() > 0.0 && count % 25 == 0 ) f.output_hdf5(Ey, gv.surroundings()); //, fileptr, true);
 		count++;
		f.step();
	}

	return 0;
}
