#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/IO/Polyhedron_iostream.h>
#include <CGAL/mesh_segmentation.h>

#include <boost/property_map/property_map.hpp>

#include <iostream>
#include <fstream>

typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef CGAL::Polyhedron_3<Kernel> Polyhedron;

int main(int argc, char **argv)
{
    if (argc !=2){
	std::cerr << "Usage: " << argv[0] << " input.off\n";
	return 1;
    }
  
    // create and read Polyhedron
    Polyhedron mesh;
    std::ifstream input(argv[1]);
    
    if ( !input || !(input >> mesh) || mesh.empty() ){
	std::cerr << argv[1] << " is not a valid off file.\n";
	return 1;
    }

    // create a property-map (it is an adaptor for this case)
    typedef std::map<Polyhedron::Facet_const_handle, double> Facet_double_map;
    Facet_double_map internal_map;
    boost::associative_property_map<Facet_double_map> sdf_property_map(internal_map);

    // compute sdf values using default parameters for number of rays, and cone angle
    std::pair<double, double> min_max_sdf = CGAL::sdf_values_computation(mesh, sdf_property_map);
    // print minimum & maximum sdf values
    std::cout << "minimum sdf: " << min_max_sdf.first << " maximum sdf: " << min_max_sdf.second << std::endl;
    // print sdf values
    for(Polyhedron::Facet_const_iterator facet_it = mesh.facets_begin(); 
        facet_it != mesh.facets_end(); ++facet_it)   
    {
        std::cout << sdf_property_map[facet_it] << std::endl;                                 
    }

    int number_of_rays = 30;                   // cast 30 rays per facet
    double cone_angle = (1.0 / 2.0) * CGAL_PI; // use 90 degrees for cone opening-angle

    // create another property-map
    Facet_double_map internal_map_2;
    boost::associative_property_map<Facet_double_map> sdf_property_map_2(internal_map_2);

    // use custom parameters for number of rays, and cone angle.
    CGAL::sdf_values_computation(mesh, sdf_property_map_2, cone_angle, number_of_rays);

    // print differences 
    for(Polyhedron::Facet_const_iterator facet_it = mesh.facets_begin(); 
        facet_it != mesh.facets_end(); ++facet_it)   
    {
        std::cout << sdf_property_map[facet_it] - sdf_property_map_2[facet_it] << std::endl;                                 
    }
}