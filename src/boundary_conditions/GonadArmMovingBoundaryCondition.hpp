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

#ifndef GONADARMMOVINGBOUNDARYCONDITION_HPP_
#define GONADARMMOVINGBOUNDARYCONDITION_HPP_

#include "AbstractMovingBoundaryCondition.hpp"

#include "ChasteSerialization.hpp"
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/vector.hpp>


/**
 * A boundary condition restricting cells to lie inside a tube, which grows in the -x direction for "StraightLengthLower",
 * turns through a semicircle of radius "TurnRadius" then grows in the +x direction for "StraightLengthUpper".
 * The private double CurrentLength tracks how far the gonad has grown along its specified path and is updated by
 * UpdateBoundaryCondition. The radius of the tube is "CurrentTubeRadius", and grows up to "FinalRadius".
 * "GrowthRateLinear" specifies how far along the path the arm grows per timestep, and "GrowthRateRadial" gives
 * the rate of radial growth.
 *
 * As with many boundary conditions for the C. elegans gonad arm project, this boundary condition automatically
 * calculates the distance a cell is along the tube from the upper end, and records it in cell data as
 * DistanceAwayFromDTC.
 */
template<unsigned DIM>
class GonadArmMovingBoundaryCondition : public AbstractMovingBoundaryCondition<DIM>
{
private:

    double mCurrentLength;
    double mFinalLength;
    double mStraightLengthLower;
    double mStraightLengthUpper;
    double mTurnRadius;
    double mCurrentTubeRadius;
    double mFinalTubeRadius;
    double mGrowthRateLinear;
    double mGrowthRateRadial;

    /** The maximum distance from the surface of the tube that cells may be. */
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
     * @param CurrentLength current length of gonad arm
     * @param FinalLength final length of gonad arm
     * @param StraightLengthLower Length of the proximal part of the adult arm
     * @param StraightLengthUpper Length of the distal part of the adult arm
     * @param TurnRadius Radius of the semicircle the gonad arm turns through
     * @param CurrentTubeRadius current radius of the tube
     * @param FinalTubeRadius final radius of the tube
     * @param GrowthRateLinear Length added = GrowthRateLinear*Dt per timestep
     * @param GrowthRateRadial Increase in Radius = GrowthRateRadial*Dt per timestep
     * @param distance the maximum distance from the surface that cells may be (defaults to 1e-5)
     */
    GonadArmMovingBoundaryCondition(AbstractCellPopulation<DIM>* pCellPopulation,
    								double CurrentLength,
    								double StraightLengthLower,
    								double StraightLengthUpper,
    								double TurnRadius,
    								double CurrentTubeRadius,
    								double FinalTubeRadius,
    								double GrowthRateLinear,
    								double GrowthRateRadial,
                                    double distance=1e-5);


     /**
      *
      * @update boundary condition at each timestep by increasing the length along the growth path (CurrentLength) and the current radius
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
    double GetCurrentLength() const;
    double GetFinalLength() const;
    double GetStraightLengthLower() const;
    double GetStraightLengthUpper() const;
    double GetTurnRadius() const;
    double GetCurrentTubeRadius() const;
    double GetFinalTubeRadius() const;
    double GetGrowthRateLinear() const;
    double GetGrowthRateRadial() const;

    /*Utility functions for calculating closest points on different parts of the gonad arm to a cell centre*/
    void GetClosestOnLowerStraight(c_vector<double, DIM>& point, c_vector<double, DIM> cell_location, double& dist);
    void GetClosestOnUpperStraight(c_vector<double, DIM>& point, c_vector<double, DIM> cell_location, double& dist);
    void GetClosestOnLoop(c_vector<double, DIM>& point, c_vector<double, DIM> cell_location, double& dist);
    /*And for calculating how far along the gonad arm a point on the growth path is */
    void HowFarAlongAreYou(c_vector<double, DIM> point, double& dist);

};

#include "SerializationExportWrapper.hpp"
EXPORT_TEMPLATE_CLASS_SAME_DIMS(GonadArmMovingBoundaryCondition)

namespace boost
{
namespace serialization
{
/**
 * Serialize information required to construct an GonadArmMovingBoundaryCondition.
 */
template<class Archive, unsigned DIM>
inline void save_construct_data(
    Archive & ar, const GonadArmMovingBoundaryCondition<DIM>* t, const BOOST_PFTO unsigned int file_version)
{
    // Save data required to construct instance
    const AbstractCellPopulation<DIM>* const p_cell_population = t->GetCellPopulation();
    ar << p_cell_population;

    // Archive other member variables
    double CurrentLength = t->GetCurrentLength();
    ar << CurrentLength;
    double FinalLength = t->GetFinalLength();
    ar << FinalLength;
    double StraightLengthLower = t->GetStraightLengthLower();
    ar << StraightLengthLower;
    double StraightLengthUpper = t->GetStraightLengthUpper();
    ar << StraightLengthUpper;
    double TurnRadius = t->GetTurnRadius();
    ar << TurnRadius;
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
 * De-serialize constructor parameters and initialize a GonadArmMovingBoundaryCondition.
 */
template<class Archive, unsigned DIM>
inline void load_construct_data(
    Archive & ar, GonadArmMovingBoundaryCondition<DIM>* t, const unsigned int file_version)
{
    // Retrieve data from archive required to construct new instance
    AbstractCellPopulation<DIM>* p_cell_population;
    ar >> p_cell_population;

    // Retrieve other member variables
    double CurrentLength;
    ar >> CurrentLength;
    double FinalLength;
    ar >> FinalLength;
    double StraightLengthLower;
    ar >> StraightLengthLower;
    double StraightLengthUpper;
    ar >> StraightLengthUpper;
    double TurnRadius;
    ar >> TurnRadius;
    double CurrentTubeRadius;
    ar >> CurrentTubeRadius;
    double FinalTubeRadius;
    ar >> FinalTubeRadius;
    double GrowthRateLinear;
    ar >> GrowthRateLinear;
    double GrowthRateRadial;
    ar >> GrowthRateRadial;

    // Invoke inplace constructor to initialise instance
    ::new(t)GonadArmMovingBoundaryCondition<DIM>(p_cell_population,CurrentLength,StraightLengthLower,StraightLengthUpper,TurnRadius,CurrentTubeRadius,FinalTubeRadius,GrowthRateLinear,GrowthRateRadial,1e-5);
}
}
} // namespace ...

#endif /*GONADARMMOVINGBOUNDARYCONDITION_HPP_*/
