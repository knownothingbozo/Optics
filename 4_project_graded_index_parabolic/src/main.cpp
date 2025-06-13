#include <cmath>
#include <cstdio>
#include <iostream>
#include <meep.hpp>
#include <meep/vec.hpp>
#include <math.h>
using namespace meep;

const double x_max = 32.0;
const double y_max = 2.0;
const double n0 = 1.55;
const double n1 = 1.0;
const double freq = 1.0;

const double pml_thickness = 0.5;

double eps(const vec &p) {
	return sqrt(pow(n0,2)-(pow(n0,2)-pow(n1,2))*pow(p.y()-1.0,2)/pow(y_max/2.0,2));
}


int main(int argc, char **argv) {
	initialize mpi(argc, argv);
	
	const double amicron = 100.0;
	
	const boundary_region pml_layers = pml(pml_thickness, X, Low);

	const grid_volume gv = vol2d(x_max, y_max, amicron);

	structure s(gv, eps, pml_layers);
	fields f(&s);

	gaussian_src_time src(freq, 5.0, 0.0, 1.0);
	f.add_point_source(Ey, src, vec(1.0, 1.0));

	f.output_hdf5(Dielectric, gv.surroundings());

	while (f.time() < 35.0) {
 		if (f.time() > 34.5) f.output_hdf5(Ey, gv.surroundings());
 		f.step();
	}

	return 0;
}
