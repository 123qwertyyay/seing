#define _USE_MATH_DEFINES

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <math.h>
#include <time.h>

#include "atom.h"
#include "diamondcalculator.h"
#include "utilities.h"

#define TOLERANCE pow(10,-10)

using namespace std;

DiamondCalculator::DiamondCalculator(AtomicSystem& asys,fingerprintProperties fpprop): fpproperties(fpprop),atomicsystem(asys) {

    cutoff = fpproperties.cutoff;
    inner_cutoff = fpproperties.inner_cutoff;

   // cout<<"\tInner Cutoff is:"<<inner_cutoff<<"\n";

    size   = 10; //fpproperties.size; TODO: read from input file
    lattice_constant = 5.8572; //5.431; //fpproperties.latticeconstant; TODO: read from input file

    ngaussians = new int[size];

   // int temp[6] = {1,4,4,4,4,4};

    ngaussians[0] = 1;
    for (int i=1; i<6; i++) ngaussians[i] = 4; //TODO: read from input file
    ngaussians[6] = 12;
    ngaussians[7] = 16;
    ngaussians[8] = 16;
    ngaussians[9] = 12;
}

DiamondCalculator::~DiamondCalculator(){

}

int DiamondCalculator::get_size(void) {return size;}


double* DiamondCalculator::calculate_fingerprint(int atomid, NeighborList &nlist) {

    int nneighbors = nlist.get_n_neighbors(atomid);
    int* neighbors = nlist.get_sorted_neighbors(atomid);
 
  //  double* distances = new double[nneighbors];

  //  for (int j=0; j<nneighbors; j++) {
 //       double distance = sqrt(atomicsystem.get_square_distance(atomid,neighbors[j]));
 //       distances[j] = distance;    
 //   }
 
    double* fp;
    fp = new double[size];
    for (int i=0; i<size; i++) fp[i] = 0.0;

    Atom myatom = atomicsystem.get_atom(atomid); 

    double x0 = myatom.get_x(); 
    double y0 = myatom.get_y(); 
    double z0 = myatom.get_z(); 

    vector<vector<double>> gaussians = generateGaussians(x0,y0,z0);

    vector<vector<vector<double>>> atoms_for_each_gaussian = findAtomsForEachGaussian(gaussians,atomid,nneighbors,neighbors);
 
    int total_gaussians = 0;
    for (int i=0; i<size; i++)
        total_gaussians += ngaussians[i];

 
    double* gaussian_values = new double[total_gaussians];

    for (int i=0; i<total_gaussians; i++)       
        gaussian_values[i] = calculateGaussianValue(atoms_for_each_gaussian[i],gaussians[i]);     


   // Aggregating as needed

    int n = 0;

    for (int i=0; i<size; i++){

        double value = 0.0;

        for (int j=0; j<ngaussians[i]; j++) {

            value += gaussian_values[n];
            n++;
        }

        fp[i] = value;
    }

    return fp;
}


vector<vector<double>> DiamondCalculator::generateGaussians(double x, double y, double z) {

    // each feature will be a sum of gaussians

    // TODO get details on how to generate gaussians from input file
    // For now, just assume diamond cubic structure
    // A gaussian is defined by alpha_ref, x_ref, y_ref, z_ref (for now alpha_ref = 1)


    // Calculate dx, dy, dz based on lattice constant
    double bondlength = (lattice_constant/4)*(pow(3,0.5));
    double dx = lattice_constant/4;
    double dy = lattice_constant/4;
    double dz = lattice_constant/4;

    vector<vector<double>> disp = { {1, 0, 0, 0}, 
                                    {1,-1,-1,-1}, { 1, 1, 1,-1}, {1,-1, 1,-1}, {1, 1,-1,-1},
                                    {1,-1,-1, 1}, { 1, 1, 1, 1}, {1,-1, 1, 1}, {1, 1,-1, 1},
                                    {1,-2,-2, 0}, { 1, 2, 2, 0}, {1,-2, 2, 0}, {1, 2,-2, 0},
                                    {1,-2, 0,-2}, { 1, 0,-2,-2}, {1, 2, 0,-2}, {1, 0, 2,-2},
                                    {1,-2, 0, 2}, { 1, 0,-2, 2}, {1, 2, 0, 2}, {1, 0, 2, 2},
                                    {1,-3,1,-3}, { 1,1,1,-3}, {1,-1,3,-3}, {1,-1,-1,-3}, {1,3,-1,-3}, {1,1,-3,-3}, {1, 3,1,-3}, {1, 1,3,-3}, {1, 1,-1,-3}, {1, -1,1,-3}, {1,-3,-1,-3}, {1,-1,-3,-3}, 
                                    {1,-1,1,-1}, {1,-1,-3,-1}, {1,-3,-1,-1}, {1,1,-1,-1}, {1,1,3,-1}, {1,-3,3,-1}, {1,3,1,-1}, {1,3,-3,-1}, {1,1,1,-1}, {1,-3,1,-1}, {1,-1,3,-1}, {1,-1,-1,-1}, {1,3,-1,-1}, {1,3,3,-1}, {1,1,-3,-1}, {1,-3,-3,-1}, 
                                    {1, 1,1,1}, {1,-3,-3,1}, {1,1,-3,1}, {1,-3,1,1}, {1,-1,-1,1}, {1,-1,3,1}, {1,3,-1,1}, {1,3,3,1}, {1,-3,-1,1}, {1,-3,3,1}, {1,1,-1,1}, {1,1,3,1}, {1,-1,1,1}, {1,3,1,1}, {1,-1,-3,1}, {1, 3,-3,1}, 
                                    {1,-3,-1,3}, {1,1,-1,3}, {1,-1,1,3}, {1,-1,-3,3}, {1,3,1,3}, {1,1,3,3}, {1,-1,-1,3}, {1,-1,3,3}, {1,1,1,3}, {1,-3,1,3}, {1,3,-1,3}, {1,1,-3,3}
                                  };

    vector<vector<double>> gaussians;

    double xmin = atomicsystem.get_xmin();
    double ymin = atomicsystem.get_ymin();
    double zmin = atomicsystem.get_zmin();

    double xmax = atomicsystem.get_xmax();
    double ymax = atomicsystem.get_ymax();
    double zmax = atomicsystem.get_zmax();

    double xsize = atomicsystem.get_xsize();
    double ysize = atomicsystem.get_ysize();
    double zsize = atomicsystem.get_zsize();
    
    for (int i=0; i<disp.size(); i++){

       // Accouting for PBC

        double xgaussian = x+disp[i][1]*dx;
        double ygaussian = y+disp[i][2]*dy;
        double zgaussian = z+disp[i][3]*dz;

        if (xgaussian < xmin) xgaussian += xsize;
        if (ygaussian < ymin) ygaussian += ysize;
        if (zgaussian < zmin) zgaussian += zsize;

        if (xgaussian > xmax) xgaussian -= xsize;
        if (ygaussian > ymax) ygaussian -= ysize;
        if (zgaussian > zmax) zgaussian -= zsize;

        vector<double> t = {disp[i][0], xgaussian, ygaussian, zgaussian}; 
        gaussians.push_back(t);
    }


    ofstream myfile("gaussians.xyz");

    if (myfile.is_open()) {
        myfile<<gaussians.size()<<"\n\n";
        for (int i=0; i<gaussians.size(); i++){
            for (int t=0; t<4; t++){
                myfile << gaussians[i][t]<<"\t";
            }
            myfile<<"\n";                     
        }
        myfile.close();
    }
  

    return gaussians;
}

    // Find all atoms within Rc of the center of each Gaussian

vector<vector<vector<double>>> DiamondCalculator::findAtomsForEachGaussian(vector<vector<double>> gaussians, int atomid, int nneighbors, int* neighbors) {

   // returns the atomids as well as distances relevant to each gaussian basis

    vector<vector<vector<double>>> atoms_dist_for_each_gaussian;

    for (int g=0; g<gaussians.size(); g++){
        vector<vector<double>> placeholder;
        atoms_dist_for_each_gaussian.push_back(placeholder);
        double sq_dist = calculateDistanceToGaussian(atomid,gaussians[g]);

        if (sq_dist <= pow(inner_cutoff,2)) {
            vector<double> temp = {(double)atomid,sq_dist};
            atoms_dist_for_each_gaussian[g].push_back(temp);
        }
    }

 //   cout<<"\tThis atom has "<<nneighbors<<" neighbors";

    for (int i=0; i< nneighbors; i++) {
      //  vector<double> distances_to_gaussians;
        for (int g=0; g<gaussians.size(); g++){

            double sq_dist = calculateDistanceToGaussian(neighbors[i],gaussians[g]);
        //    distances_to_gaussians.push_back(pow(sq_dist,0.5));

            if (sq_dist <= pow(inner_cutoff,2)){
                vector<double> temp = {(double)neighbors[i],sq_dist};
                atoms_dist_for_each_gaussian[g].push_back(temp);
                }
        }
      //  cout<<"\tDistances of neighbor "<<i+1<<" to gaussians: ";
      //  for (int d=0; d<gaussians.size(); d++) cout<<distances_to_gaussians[d]<<" ";
      //  cout<<"\n";
    }

    return atoms_dist_for_each_gaussian;
} 

double DiamondCalculator::calculateDistanceToGaussian(int atomid,vector<double >gaussian) {

    Atom atom1 = atomicsystem.get_atom(atomid);

    // Moves gaussian inside box, if outside (takes care of PBC)

    double x = gaussian[1];
    double y = gaussian[2];
    double z = gaussian[3];

//    double xmin = atomicsystem.get_xmin();
//   double ymin = atomicsystem.get_ymin();
//    double zmin = atomicsystem.get_zmin();

//    double xsize = atomicsystem.get_xsize();
//    double ysize = atomicsystem.get_ysize();
//    double zsize = atomicsystem.get_zsize();

//    if (x < xmin) x += xsize;
//    if (y < ymin) x += ysize;
//    if (z < zmin) x += zsize;

//    if (x > xmin+xsize) x -= xsize;
//    if (y > ymin+ysize) y -= ysize;
//    if (z > zmin+zsize) z -= zsize;

    Atom atom2 = Atom("",x,y,z); // This is a fictive atom at the location of the gaussian

    double sq_distance = atomicsystem.get_square_distance(atom1, atom2);
   
    return sq_distance;
}


double DiamondCalculator::calculateGaussianValue(vector<vector<double>> atoms_sq_distances, vector<double> gaussian) {

    double value     = 0.0;
    double alpha_ref = gaussian[0];


    //loop over all atoms within Rc of the center of this gaussian
   // cout<<"\tThis guassian has "<<atoms_sq_distances.size()<<" atoms associated with it...distance(s): ";

    for (int i=0; i<atoms_sq_distances.size(); i++) {

        Atom myatom = atomicsystem.get_atom((int)atoms_sq_distances[i][0]); 
        double alpha = 1.0; //TODO determine alpha based on atom type

        double dist_squared = atoms_sq_distances[i][1]; 
    //    cout<<" "<<pow(dist_squared,0.5)<<" ";

        value += (alpha/alpha_ref)*exp(-dist_squared/pow(cutoff,2))*cutoff_func(pow(dist_squared,0.5),cutoff);
    }
   // cout<<"\n";

    return value;
}