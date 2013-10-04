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

#include "EllipsoidOutsideMovingBoundaryCondition.hpp"
#include "NodeBasedCellPopulation.hpp"

/*Constructor. Require cell population and parameters defining the ellipse*/
template<unsigned DIM>
EllipsoidOutsideMovingBoundaryCondition<DIM>::EllipsoidOutsideMovingBoundaryCondition(AbstractCellPopulation<DIM>* pCellPopulation,
                                                                      c_vector<double, DIM> centre,
                                                                      double A,
                                                                      double B,
                                                                      double C,
                                                                      double distance)
    : AbstractMovingBoundaryCondition<DIM>(pCellPopulation),
      mCentre(centre),
      mA(A),
      mB(B),
      mC(C),
      mMaximumDistance(distance)
{
	/*Assert all three ellipse Radii are positive*/
    assert(mA > 0.0);
    assert(mB > 0.0);
    assert(mC > 0.0);
    assert(mMaximumDistance > 0.0);

    if (dynamic_cast<NodeBasedCellPopulation<DIM>*>(this->mpCellPopulation) == NULL)
    {
        EXCEPTION("A NodeBasedCellPopulation must be used with this boundary condition object.");
    }
    if (DIM != 3)
    {
        EXCEPTION("This boundary condition is only implemented in 3D.");
    }
}


/*Methods returning parameter values*/
template<unsigned DIM>
const c_vector<double, DIM>& EllipsoidOutsideMovingBoundaryCondition<DIM>::rGetCentre() const
{
    return mCentre;
}

template<unsigned DIM>
double EllipsoidOutsideMovingBoundaryCondition<DIM>::rGetA() const
{
    return mA;
}

template<unsigned DIM>
double EllipsoidOutsideMovingBoundaryCondition<DIM>::rGetB() const
{
    return mB;
}

template<unsigned DIM>
double EllipsoidOutsideMovingBoundaryCondition<DIM>::rGetC() const
{
    return mC;
}


/*Example update function. The radius C is increased by Dt each timestep.*/
template<unsigned DIM>
void EllipsoidOutsideMovingBoundaryCondition<DIM>::UpdateBoundaryCondition(){
	mC=mC+SimulationTime::Instance()->GetTimeStep()*0.01;
}


/*Check whether a cell's location is outside the ellipsoid. If not, scale its position away from the ellipsoid centre
 * until it is on surface. Note, this is not the same as correcting to the closest point on the ellipsoid's surface
 * but for ellipsoids that are not too eccentric and for small displacements off the surface results will be
 * comparable.
 * */
template<unsigned DIM>
void EllipsoidOutsideMovingBoundaryCondition<DIM>::ImposeBoundaryCondition(const std::map<Node<DIM>*, c_vector<double, DIM> >& rOldLocations)
{
    // Iterate over the cell population
    for (typename AbstractCellPopulation<DIM>::Iterator cell_iter = this->mpCellPopulation->Begin();
         cell_iter != this->mpCellPopulation->End();
         ++cell_iter)
    {
        // Is it in the ellipsoid?
        c_vector<double,DIM> cell_location = this->mpCellPopulation->GetLocationOfCellCentre(*cell_iter);
        double R = pow((cell_location[0]-mCentre[0]),2)/(mA*mA)
               		+pow((cell_location[1]-mCentre[1]),2)/(mB*mB)
               		+pow((cell_location[2]-mCentre[2]),2)/(mC*mC);
        assert(R != 0.0); //Can't project the centre to anywhere sensible

        // If the cell is inside the surface of the ellipsoid...
        if (R < 1-mMaximumDistance)
        {

            // ...move the cell back onto the surface of the ellipsoid:
            c_vector<double, DIM> location_on_Ellipsoid;
            location_on_Ellipsoid=mCentre+(cell_location-mCentre)/sqrt(R);

            unsigned node_index = this->mpCellPopulation->GetLocationIndexUsingCell(*cell_iter);
            Node<DIM>* p_node = this->mpCellPopulation->GetNode(node_index);

            p_node->rGetModifiableLocation() = location_on_Ellipsoid;
        }
    }
}


/*Check that the boundary condition enforcement worked. All cells should now be in or on the
 * ellipsoid*/
template<unsigned DIM>
bool EllipsoidOutsideMovingBoundaryCondition<DIM>::VerifyBoundaryCondition()
{
    bool condition_satisfied = true;

    // Iterate over the cell population
    for (typename AbstractCellPopulation<DIM>::Iterator cell_iter = this->mpCellPopulation->Begin();
         cell_iter != this->mpCellPopulation->End();
         ++cell_iter)
    {
        // Find R=x^2/A^2+y^2/B^2+z^2/C^2 for this cell
        c_vector<double,DIM> cell_location = this->mpCellPopulation->GetLocationOfCellCentre(*cell_iter);
        double R = pow(cell_location[0]-mCentre[0],2)/(mA*mA)
        		+pow(cell_location[1]-mCentre[1],2)/(mB*mB)
        		+pow(cell_location[2]-mCentre[2],2)/(mC*mC);

        // If the cell is too far from the surface of the ellipsoid...
        if (R < 1-mMaximumDistance)
        {
            // ...then the boundary condition is not satisfied. Panic.
            condition_satisfied = false;
            break;
        }
    }
    return condition_satisfied;
}


template<unsigned DIM>
void EllipsoidOutsideMovingBoundaryCondition<DIM>::OutputCellPopulationBoundaryConditionParameters(out_stream& rParamsFile)
{
    *rParamsFile << "\t\t\t<CentreOfSphere>";
    for (unsigned index=0; index != DIM-1U; index++) // Note: inequality avoids testing index < 0U when DIM=1
    {
        *rParamsFile << mCentre[index] << ",";
    }
    *rParamsFile << mCentre[DIM-1] << "</CentreOfEllipse>\n";

    *rParamsFile << "\t\t\t<RadiusA>" << mA << "</RadiusA>\n";
    *rParamsFile << "\t\t\t<RadiusB>" << mB << "</RadiusB>\n";
    *rParamsFile << "\t\t\t<RadiusC>" << mC << "</RadiusC>\n";

    *rParamsFile << "\t\t\t<MaximumDistance>" << mMaximumDistance << "</MaximumDistance>\n";

    // Call method on direct parent class
    AbstractMovingBoundaryCondition<DIM>::OutputCellPopulationBoundaryConditionParameters(rParamsFile);
}

/////////////////////////////////////////////////////////////////////////////
// Explicit instantiation
/////////////////////////////////////////////////////////////////////////////

template class EllipsoidOutsideMovingBoundaryCondition<1>;
template class EllipsoidOutsideMovingBoundaryCondition<2>;
template class EllipsoidOutsideMovingBoundaryCondition<3>;

// Serialization for Boost >= 1.36
#include "SerializationExportWrapperForCpp.hpp"
EXPORT_TEMPLATE_CLASS_SAME_DIMS(EllipsoidOutsideMovingBoundaryCondition)
