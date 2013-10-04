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

#ifndef CylinderOutsideMovingBoundaryCondition_HPP_
#define CylinderOutsideMovingBoundaryCondition_HPP_

#include "AbstractMovingBoundaryCondition.hpp"

#include "ChasteSerialization.hpp"
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/vector.hpp>


/**
 * A boundary condition restricting cells to lie outside a tube of length mCurrentLength and radius mCurrentTubeRadius
 * with rounded endcaps. The left hand end of the cylinder is specified by an input vector to place it spatially.
 * Originally used to represent the syncytium of the C.elegans gonad arm, before being replaed by SyncytiumBoundaryCondition
 * The length of tube grows by mGrowthRateLinear*Dt linearly and by mGrowthRateRadial*Dt radially each timestep
 */
template<unsigned DIM>
class CylinderOutsideMovingBoundaryCondition : public AbstractMovingBoundaryCondition<DIM>
{
private:

	c_vector<double, DIM> mLeftEnd; //Position of the left hand end of the cylinder
    double mCurrentLength;
    double mFinalLength;
    double mCurrentTubeRadius;
    double mFinalTubeRadius;
    double mGrowthRateLinear;
    double mGrowthRateRadial;

    /** The maximum distance inside the surface of the tube that cells may be. */
    double mMaximumDistance;

    /** Needed for serialization. */
    friend class boost::serialization::access;
    /**
     * Serialize the object.
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
     * @param LeftEnd Vector pointing to start of cylinder
     * @param CurrentLength Current tube length
     * @param FinalLength Final tube length
     * @param TubeRadius Radius of the tube
     * @param GrowthRateLinear Length added = GrowthRateLinear*Dt per timestep
     * @param GrowthRateRadial Increase in radius = GrowthRateRadial*Dt per timestep
     * @param distance the maximum distance inside the surface that cells may be (defaults to 1e-5)
     */
    CylinderOutsideMovingBoundaryCondition(AbstractCellPopulation<DIM>* pCellPopulation,
    								c_vector<double, DIM> leftEnd,
    								double CurrentLength,
    								double FinalLength,
    								double CurrentTubeRadius,
    								double FinalTubeRadius,
    								double GrowthRateLinear,
    								double GrowthRateRadial,
                                    double distance=1e-5);


     /**
      *
      * @update boundary condition at each timestep by increasing the length along the growth path CurrentLength
      */
     void UpdateBoundaryCondition();

    /**
     * Overridden ImposeBoundaryCondition() method.
     *
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

    /*Functions returning the current parameter values*/
    c_vector<double, DIM> GetLeftEnd() const;
    double GetCurrentLength() const;
    double GetFinalLength() const;
    double GetCurrentTubeRadius() const;
    double GetFinalTubeRadius() const;
    double GetGrowthRateLinear() const;
    double GetGrowthRateRadial() const;

    /*Utility function for calculating closest points on the cylinder's long axis to a cell centre*/
    void GetClosestOnStraight(c_vector<double, DIM>& point, c_vector<double, DIM> cell_location, double& dist);

};

#include "SerializationExportWrapper.hpp"
EXPORT_TEMPLATE_CLASS_SAME_DIMS(CylinderOutsideMovingBoundaryCondition)

namespace boost
{
namespace serialization
{
/**
 * Serialize information required to construct an CylinderOutsideMovingBoundaryCondition.
 */
template<class Archive, unsigned DIM>
inline void save_construct_data(
    Archive & ar, const CylinderOutsideMovingBoundaryCondition<DIM>* t, const BOOST_PFTO unsigned int file_version)
{
    // Save data required to construct instance
    const AbstractCellPopulation<DIM>* const p_cell_population = t->GetCellPopulation();
    ar << p_cell_population;

    c_vector<double, DIM> leftEnd = t->GetLeftEnd();
    for (unsigned i=0; i<DIM; i++)
    {
        ar << leftEnd[i];
    }


    // Archive other member variables
    double CurrentLength = t->GetCurrentLength();
    ar << CurrentLength;
    double FinalLength = t->GetFinalLength();
    ar << FinalLength;
    double CurrentTubeRadius = t->GetCurrentTubeRadius();
    ar << CurrentTubeRadius;
    double FinalTubeRadius = t->GetFinalTubeRadius();
    ar << FinalTubeRadius;
    double GrowthRateLinear = t->GetGrowthRateLinear();
    ar << GrowthRateLinear;
    double GrowthRateRadial = t->GetGrowthRateRadial();
    ar << GrowthRateRadial;
}

/**
 * De-serialize constructor parameters and initialize a CylinderOutsideMovingBoundaryCondition.
 */
template<class Archive, unsigned DIM>
inline void load_construct_data(
    Archive & ar, CylinderOutsideMovingBoundaryCondition<DIM>* t, const unsigned int file_version)
{
    // Retrieve data from archive required to construct new instance
    AbstractCellPopulation<DIM>* p_cell_population;
    ar >> p_cell_population;

    c_vector<double, DIM> leftEnd;
    for (unsigned i=0; i<DIM; i++)
    {
        ar >> leftEnd[i];
    }

    // Retrieve other member variables
    double CurrentLength;
    ar >> CurrentLength;
    double FinalLength;
    ar >> FinalLength;
    double CurrentTubeRadius;
    ar >> CurrentTubeRadius;
    double FinalTubeRadius;
    ar >> FinalTubeRadius;
    double GrowthRateLinear;
    ar >> GrowthRateLinear;
    double GrowthRateRadial;
    ar >> GrowthRateRadial;

    // Invoke inplace constructor to initialise instance
    ::new(t)CylinderOutsideMovingBoundaryCondition<DIM>(p_cell_population,leftEnd,CurrentLength,FinalLength, CurrentTubeRadius,FinalTubeRadius,GrowthRateLinear,GrowthRateRadial,1e-5);
}
}
} // namespace ...

#endif /*CylinderOutsideMovingBoundaryCondition_HPP_*/
