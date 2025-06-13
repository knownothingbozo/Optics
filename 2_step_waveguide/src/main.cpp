#include <meep.hpp>
#include <meep/vec.hpp>
using namespace meep;

const double x_max = 4.0;
const double y_max = 1.0;
const double freq = 1.0;

const double pml_thickness = 0.1;


double eps(const vec &p) {
	if (p.x() < x_max/2.0) return 1.0;
	else {
		return 1.1;
	}
}

int main(int argc, char **argv) {
	initialize mpi(argc, argv);
	
	const double amicron = 100.0;
	const grid_volume gv = vol2d(x_max, y_max, amicron);

	structure s(gv, eps, pml(pml_thickness));
	fields f(&s);

	gaussian_src_time src(freq, 5.0, 0.0, 1.0);
	f.add_point_source(Ey, src, vec(0.5 + pml_thickness, 0.5));

	f.output_hdf5(Dielectric, gv.surroundings());

	while (f.time() < 2.0) {
 		f.output_hdf5(Ey, gv.surroundings());
 		f.step();
	}

	return 0;
}
