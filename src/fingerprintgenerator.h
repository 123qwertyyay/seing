#ifndef FINGERPRINTGENERATOR_H
#define FINGERPRINTGENERATOR_H

#include <string>

#include "atomicsystem.h"
#include "neighborlist.h"
#include "inputs.h"


using namespace std;

//! Class that handles fingerprint generation
class FingerprintGenerator {

    AtomicSystem atomicsystem;
    double **fingerprints;

    int fsize, natoms, fp_natomtypes, natompairs;

    public:
        /*! Constructor that will instantiate the fingerprint calculator and
            perform the fingerprint generation 
        */
        FingerprintGenerator (AtomicSystem&,fingerprintProperties);
        ~FingerprintGenerator(void);

        /*! Writes the fingerprint to a file in the same order as the atoms were given
            in the coordinates file
        */
        bool write2file(string,string);
};

#endif
