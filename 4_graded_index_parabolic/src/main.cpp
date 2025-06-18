#include <cmath>
#include <cstdio>
#include <iostream>
#include <meep.hpp>
#include <meep/vec.hpp>
#include <math.h>
using namespace meep;

const double c = 29979245;
const double t = 1.0 / c;
const double ns = 1e-9 / t;

// Multimode fiber
const double x_max = 750.0; //500.0;
const double y_max = 125.0; //125.0;
const double coreD = 50.0; // 50.0;
const double core_max = (y_max+coreD)/2;
const double core_min = (y_max-coreD)/2;

const double n0 = 1.46;
const double n1 = 1.44;

// Ge LED
const double freq = 1.0 / 1.3; // 1300 nm
const double fwidth = 1.0 / 0.07; // 70 nm
const double line_src_h = 0.6*y_max;

// Lens
const double m = 2.0;
const double lens_R = m*y_max;
const double na = n0*(1.0+2.0*m*tan(asin(sqrt(pow(n0,2)-pow(n1,2)))));
const double offset = lens_R*(1-cos(asin(y_max/(2.0*lens_R)))) + 2.0;
const double offset2 = lens_R*(1-cos(asin(line_src_h/(2.0*lens_R)))) + 2.0;
const double b = lens_R - offset;
const double f1 = n0*lens_R/(na-n0)-offset2;

const double pml_thickness = 0.5;

double eps(const vec &p) {
//	if ( p.x() < f1 + offset + 0.1 && p.x() > f1 + offset - 0.1 ) {
//		return pow(na,2);
//	}
	if ( p.y() <= core_max && p.y() >= core_min && p.x() >= f1 + offset ) {
		return pow(n0,2)-(pow(n0,2)-pow(n1,2))*pow(p.y()- y_max/2.0,2)/pow(coreD/2.0,2);
	}
	else if ( p.x() >= f1 + offset ) {
		return pow(n1, 2);
	}
	if ( pow(p.x() + b, 2)+pow(p.y()-y_max/2.0,2) < pow(lens_R,2) ) {
		return pow(na,2);
	}
	else if ( pow(p.x() + b, 2)+pow(p.y()-y_max/2.0,2) >= pow(lens_R,2) ) {
		return pow(n0,2);
	}
}


int main(int argc, char **argv) {
	initialize mpi(argc, argv);
	const double amicron = 20.0;
	
	//const boundary_region pml_layers = pml(pml_thickness, X, Low);

	const grid_volume gv = vol2d(x_max, y_max, amicron);
	//symmetry sym = mirror(Y, gv);

	structure s(gv, eps, pml(pml_thickness)); //, sym, pml_layers);
	fields f(&s);

	

	gaussian_src_time src(freq, fwidth, 0.0, ns);

	//f.add_point_source(Ey, src, vec(1.0 , y_max/2.0));

	volume line_src(vec(1.0, (y_max + line_src_h) /2.0 ), vec(1.0, (y_max - line_src_h) /2.0));
	f.add_volume_source(Ey, src, line_src);

	//const char file_name[] = "ey.h5";

	//h5file ofile(file_name, h5file::WRITE, true);

	//h5file* fileptr = &ofile;

	f.output_hdf5(Dielectric, gv.surroundings());

	int count = 0;

	while (f.time() <= 1201.0) {
 		if (f.time() > 0.0 && count % 100 == 0 ) f.output_hdf5(Ey, gv.surroundings()); //, fileptr, true);
 		count++;
		f.step();
	}

	return 0;
}
