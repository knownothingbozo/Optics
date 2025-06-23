#include <cmath>
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

const double pml_thickness = 2.0;

const double dt = 1.3*4;

// Multimode fiber
const double x_max = 800; //750.0;
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
const double pml_offset = 1.0 + pml_thickness;

// Lens
const double m = 1.5;
const double lens_R = m*y_max;
const double na = n0*(1.0+2.0*m*tan(asin(sqrt(pow(n0,2)-pow(n1,2)))));
const double offset = lens_R*(1-cos(asin(y_max/(2.0*lens_R)))) + pml_offset;
const double offset2 = lens_R*(1-cos(asin(line_src_h/(2.0*lens_R)))) + pml_offset;
const double b = lens_R - offset;
//const double f1 = n0*lens_R/(na-n0)-offset2;
const double f1 = y_max /2 /tan(asin(sqrt(pow(n0,2)-pow(n1,2)))) - offset2;

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
	else { return 0; }
}

double eps2(const vec &p) {
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
		return pow(n1,2);
	}
	else if ( pow(p.x() + b, 2)+pow(p.y()-y_max/2.0,2) >= pow(lens_R,2) ) {
		return pow(n0,2);
	}
	else { return 0; }
}


int main(int argc, char **argv) {
	initialize mpi(argc, argv);
	const double amicron = 20.0;
	
	//const boundary_region pml_layers = pml(pml_thickness, X, Low);

	const grid_volume gv = vol2d(x_max, y_max, amicron);
	symmetry sym = -mirror(Y, gv);

	// First generate Dielectric map with corrected values so we can visually differentiate the waveguidegrading
	structure s0(gv, eps2, pml(pml_thickness), sym);
	fields f0(&s0);

	f0.output_hdf5(Dielectric, gv.surroundings());
	
	const boundary_region pml_layers = pml(pml_thickness, X, Low) + pml(pml_thickness, Y, Low) + pml(pml_thickness, Y, High);

	// Then make a new object with the actual values
	structure s(gv, eps, pml(pml_thickness), sym); //, sym, pml_layers);
	
//	const vector3 e_diag = {12.0, 12.0, 12.0};
//	const vector3 D_con_diag = {1.0, 1.0, 1.0};
//
//	meep_geom::medium_struct lossy_absorber;
//	lossy_absorber.epsilon_diag = e_diag;			// real part of permittivity
//	lossy_absorber.D_conductivity_diag = D_con_diag;	// adds loss
//
//	// Create a geometric_object using the mediumi
//	const vector3 e1 = {1.0, 0.0, 0.0};
//	const vector3 e2 = {0.0, 1.0, 0.0};
//	const vector3 e3 = {0.0, 0.0, 1.0};
//	const vec center = {x_max-1.0, y_max/2.0};
//	const vec size = {2.0, y_max};
//	geometric_object absorber_obj[1];
//	absorber_obj[0] = make_block(lossy_absorber, center, e1, e2, e3, size);
	
//	meep_geom::absorber_list alist = 0;
//	alist = meep_geom::create_absorber_list();
//	meep_geom::add_absorbing_layer(alist, 4.0, X, High, 1e-16);
//
//	geometric_object_list g = {0, 0};
//	vector3 center = {x_max/2.0 , y_max/2.0, 0};
//	meep_geom::set_materials_from_geometry(&s, g, center,
//		true,                     // use_anisotropic_averaging,
//		DEFAULT_SUBPIXEL_TOL,     // tol
//		DEFAULT_SUBPIXEL_MAXEVAL, // maxeval
//		false,                    // ensure_periodicity
//		meep_geom::vacuum, alist);
//	meep_geom::destroy_absorber_list(alist);

	fields f(&s);

	gaussian_src_time src(freq, fwidth, 0.0, dt);

	//f.add_point_source(Ey, src, vec(1.0 , y_max/2.0));

	//volume line_src(vec(pml_offset, (y_max + line_src_h) /2.0 ), vec(pml_offset, (y_max - line_src_h) /2.0));
	volume line_src(vec(pml_offset, (y_max + line_src_h) /2.0 ), vec(pml_offset, (y_max) /2.0));
	f.add_volume_source(Ey, src, line_src);

	//const char file_name[] = "ey.h5";

	//h5file ofile(file_name, h5file::WRITE, true);

	//h5file* fileptr = &ofile;


	int count = 0;

	while (f.time() <= 1201.0) {
 		if (f.time() > 0.0 && count % 60 == 0 ) f.output_hdf5(Ey, gv.surroundings()); //, fileptr, true);
 		count++;
		f.step();
	}

	return 0;
}
