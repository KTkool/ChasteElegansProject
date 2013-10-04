/*

Copyright (c) 2005-2013, University of Oxford.
All rights reserved.

University of Oxford means the Chancellor, Masters and Scholars of the
University of Oxford, having an administrative office at Wellington
Square, Oxford OX1 2JD, UK.

This file is part of Chaste.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
 * Neither the name of the University of Oxford nor the names of its
   contributors may be used to endorse or promote products derived from this
   software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#ifndef ELLIPSOIDMOVINGBOUNDARYCONDITION_HPP_
#define ELLIPSOIDMOVINGBOUNDARYCONDITION_HPP_


#include "AbstractMovingBoundaryCondition.hpp"

#include "ChasteSerialization.hpp"
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/vector.hpp>

/*
 * File defining a moving boundary condition that confines cells to lie in an ellipsoid. The ellipsoid grows over time
 * as determined by the function UpdateBoundaryCondition, which is called at each timestep by a moving
 * boundary modifier. Inherits from AbstractMovingBoundaryCondition. 3D only.
 *
 * */
template<unsigned DIM>
class EllipsoidMovingBoundaryCondition : public AbstractMovingBoundaryCondition<DIM>
{
private:

    /** Specify ellipse as (x-Centre_x)^2/A^2+(y-Centre_y)^2/B^2+(z-Centre_z)^2/C^2 =1 */
    c_vector<double, DIM> mCentre;
    double mA;
    double mB;
    double mC;

    /** The maximum distance from the surface of the ellipsoid that cells may be. */
    double mMaximumDistance;

    /** Needed for serialization. */
    friend class boost::serialization::access;
    /**
     * Serialize the object.
     *
     * @param archive the archive
     * @param version the current version of this class
     */
    template<class Archive>
    void serialize(Archive & archive, const unsigned int version)
    {
        archive & boost::serialization::base_object<AbstractMovingBoundaryCondition<DIM> >(*this);
        archive & mMaximumDistance;
    }

public:

    /**
     * Constructor.
     *
     * @param pCellPopulation pointer to the cell population
     * @param centre the centre of the ellipse (x',y',z')
     * @param A,B,C for ellipsoid (x-Centre_x)^2/A^2+(y-Centre_y)^2/B^2+(z-Centre_z)^2/C^2 =1
     * @param distance the maximum distance from the surface of the ellipsoid that cells may be (defaults to 1e-5)
     */
    EllipsoidMovingBoundaryCondition(AbstractCellPopulation<DIM>* pCellPopulation,
                                    c_vector<double, DIM> centre,
                                    double ParamA,
                                    double ParamB,
                                    double ParamC,
                                    double distance=1e-5);


    /**
     * @return #mCentre.
     */
    const c_vector<double, DIM>& rGetCentre() const;

    /**
     * @return #mParamA,B,C.
     */
     double rGetA() const;
     double rGetB() const;
     double rGetC() const;

     /**
      * @update parameters as desired each timestep.
      */
     void UpdateBoundaryCondition();

    /**
     * Overridden ImposeBoundaryCondition() method.
     * Apply the cell population boundary conditions.
     *
     * @param rOldLocations the node locations before any boundary conditions are applied
     */
    void ImposeBoundaryCondition(const std::map<Node<DIM>*, c_vector<double, DIM> >& rOldLocations);

    /**
     * Overridden VerifyBoundaryCondition() method.
     * Verify the boundary conditions have been applied.
     * This is called after ImposeBoundaryCondition() to ensure the condition is still satisfied.
     *
     * @return whether the boundary conditions are satisfied.
     */
    bool VerifyBoundaryCondition();

    /**
     * Overridden OutputCellPopulationBoundaryConditionParameters() method.
     * Output cell population boundary condition parameters to file.
     *
     * @param rParamsFile the file stream to which the parameters are output
     */
    void OutputCellPopulationBoundaryConditionParameters(out_stream& rParamsFile);

};

#include "SerializationExportWrapper.hpp"
EXPORT_TEMPLATE_CLASS_SAME_DIMS(EllipsoidMovingBoundaryCondition)

namespace boost
{
namespace serialization
{
/**
 * Serialize information required to construct an EllipsoidMovingBoundaryCondition.
 */
template<class Archive, unsigned DIM>
inline void save_construct_data(
    Archive & ar, const EllipsoidMovingBoundaryCondition<DIM>* t, const BOOST_PFTO unsigned int file_version)
{
    // Save data required to construct instance
    const AbstractCellPopulation<DIM>* const p_cell_population = t->GetCellPopulation();
    ar << p_cell_population;

    // Archive c_vectors one component at a time
    c_vector<double, DIM> point = t->rGetCentre();
    for (unsigned i=0; i<DIM; i++)
    {
        ar << point[i];
    }

    // Archive other member variables
    double A = t->rGetA();
    ar << A;
    // Archive other member variables
    double B = t->rGetB();
    ar << B;
    // Archive other member variables
    double C = t->rGetC();
    ar << C;


}

/**
 * De-serialize constructor parameters and initialize an EllipsoidMovingBoundaryCondition.
 */
template<class Archive, unsigned DIM>
inline void load_construct_data(
    Archive & ar, EllipsoidMovingBoundaryCondition<DIM>* t, const unsigned int file_version)
{
    // Retrieve data from archive required to construct new instance
    AbstractCellPopulation<DIM>* p_cell_population;
    ar >> p_cell_population;

    // Retrieve c_vectors one component at a time
    c_vector<double, DIM> centre;
    for (unsigned i=0; i<DIM; i++)
    {
        ar >> centre[i];
    }

    // Retrieve other member variables
    double A;
    ar >> A;
    double B;
    ar >> B;
    double C;
    ar >> C;

    // Invoke inplace constructor to initialise instance
    ::new(t)EllipsoidMovingBoundaryCondition<DIM>(p_cell_population, centre, A,B,C,1e-5);
}
}
} // namespace ...

#endif /*ELLIPSOIDMOVINGBOUNDARYCONDITION_HPP_*/
