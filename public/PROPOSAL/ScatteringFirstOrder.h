/*! \file   Scattering.h
*   \brief  Header file for the Scattering routines.
*
*   For more details see the class documentation.
*
*   \date   2013.06.13
*   \author Tomasz Fuchs
*/


#ifndef SCATTERING_FIRSTORDER_H
#define SCATTERING_FIRSTORDER_H
#include "vector"
#include <string>
#include "PROPOSAL/PROPOSALParticle.h"
#include "PROPOSAL/MathModel.h"
#include "PROPOSAL/Medium.h"

namespace PROPOSAL{

/**
  * \brief This class provides the scattering routine provided by moliere.
  *
  * More precise scattering angles will be added soon.
  */


class ScatteringFirstOrder : public MathModel
{

//----------------------------------------------------------------------------//

public:

    /**
     * \brief Default Constructor
     *
     * Constructor which sets "default" settings.
     */
    ScatteringFirstOrder();


//----------------------------------------------------------------------------//



//----------------------------------------------------------------------------//
    // Memberfunctions

    double  CalculateTheta0(double dr, PROPOSALParticle* part, Medium* med);
    void    Scatter(double dr, PROPOSALParticle* part, Medium* med);


//----------------------------------------------------------------------------//
    // destructors
    ~ScatteringFirstOrder() {}


};
};


#endif //SCATTERING_FIRSTORDER_H
