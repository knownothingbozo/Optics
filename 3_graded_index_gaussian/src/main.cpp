#include <cmath>
#include <cstdio>
#include <iostream>
#include <meep.hpp>
#include <meep/vec.hpp>
#include <math.h>
using namespace meep;

const double x_max = 32.0;
const double y_max = 2.0;
const double freq = 1.0;

const double pml_thickness = 0.5;


double eps(const vec &p) {
	if (p.y() < 0.5) return 1.0;
	else if (p.y() > 1.5) return 1.0;
	else return exp(-pow(p.y()-1,2)/0.08) + 1;
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

	while (f.time() < 2.0) {
 		if (f.time() > 0.0) f.output_hdf5(Ey, gv.surroundings());
 		f.step();
	}

	return 0;
}
