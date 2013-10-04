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

#include "DistalArmBoundaryCondition.hpp"
#include "NodeBasedCellPopulation.hpp"
#define PI 3.1415926

template<unsigned DIM>
DistalArmBoundaryCondition<DIM>::DistalArmBoundaryCondition(AbstractCellPopulation<DIM>* pCellPopulation,
											c_vector<double, DIM> leftEnd,
											double CurrentLength,
		    								double CurrentTubeRadius,
		                                    double distance)
    : AbstractCellPopulationBoundaryCondition<DIM>(pCellPopulation),
      mLeftEnd(leftEnd),
      mCurrentLength(CurrentLength),
      mCurrentTubeRadius(CurrentTubeRadius),
      mMaximumDistance(distance)
{
    assert(mCurrentLength > 0.0);
    assert(mCurrentTubeRadius > 0.0);
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


/*Getter method for each parameter*/
template<unsigned DIM>
double DistalArmBoundaryCondition<DIM>::GetCurrentLength() const
{
    return mCurrentLength;
}
template<unsigned DIM>
double DistalArmBoundaryCondition<DIM>::GetCurrentTubeRadius() const
{
    return mCurrentTubeRadius;
}
template<unsigned DIM>
c_vector<double, DIM> DistalArmBoundaryCondition<DIM>::GetLeftEnd() const
{
    return mLeftEnd;
}



/*Checks whether each cell lies outside the tube. If so, places the cell just touching the tube's surface*/
template<unsigned DIM>
void DistalArmBoundaryCondition<DIM>::ImposeBoundaryCondition(const std::map<Node<DIM>*, c_vector<double, DIM> >& rOldLocations)
{

    // Iterate over the cell population and get cell location
    for (typename AbstractCellPopulation<DIM>::Iterator cell_iter = this->mpCellPopulation->Begin();
         cell_iter != this->mpCellPopulation->End();
         ++cell_iter)
    {
    	 c_vector<double,DIM> cell_location = this->mpCellPopulation->GetLocationOfCellCentre(*cell_iter);
    	 Node<DIM>* cell_centre_node = this->mpCellPopulation->GetNode(this->mpCellPopulation->GetLocationIndexUsingCell(*cell_iter));
    	 double radius = cell_centre_node->GetRadius();

    	 // Find C, the closest point on the growth path for this cell, and R the distance to it.
        double R=0;
        c_vector<double, DIM> C= zero_vector<double>(3);
        GetClosestOnStraight(C,cell_location,R);

        //set distance from DTC
        cell_iter->GetCellData()->SetItem("DistanceAwayFromDTC",mCurrentLength+mCurrentTubeRadius-cell_location[0]);

        // If the cell is too far from the growth path, and therefore outside the tube...
        if (R+radius> mCurrentTubeRadius+ mMaximumDistance)
        {
        	// ...move the cell back onto the surface of the tube by translating away from C:
            unsigned node_index = this->mpCellPopulation->GetLocationIndexUsingCell(*cell_iter);
            Node<DIM>* p_node = this->mpCellPopulation->GetNode(node_index);
            c_vector<double,3> new_location = C+(mCurrentTubeRadius-radius)*(cell_location-C)/norm_2(cell_location-C);
            p_node->rGetModifiableLocation() = new_location;

            //If position has changed; reset distance from DTC
            cell_iter->GetCellData()->SetItem("DistanceAwayFromDTC",mCurrentLength+mCurrentTubeRadius-new_location[0]);
        }

    }
}


//Check boundary condition is now satisfied
template<unsigned DIM>
bool DistalArmBoundaryCondition<DIM>::VerifyBoundaryCondition()
{
    bool condition_satisfied = true;

    // Iterate over the cell population
    for (typename AbstractCellPopulation<DIM>::Iterator cell_iter = this->mpCellPopulation->Begin();
         cell_iter != this->mpCellPopulation->End();
         ++cell_iter)
    {
    	c_vector<double,DIM> cell_location = this->mpCellPopulation->GetLocationOfCellCentre(*cell_iter);
   	    Node<DIM>* cell_centre_node = this->mpCellPopulation->GetNode(this->mpCellPopulation->GetLocationIndexUsingCell(*cell_iter));
   	    double radius = cell_centre_node->GetRadius();

        // Find R=minimum distance to the growth path for this cell, as in ImposeBoundaryCondition
        double R=0;
        c_vector<double, DIM> C=zero_vector<double>(3);
        GetClosestOnStraight(C,cell_location,R);
        // If the cell is too far from the surface of the tube...
        if (R+radius> mCurrentTubeRadius+ mMaximumDistance)
        {
            // ...then the boundary condition is not satisfied :-(
            condition_satisfied = false;
            break;
        }
    }
    return condition_satisfied;
}


/*Utility functions for determining the closest point on the long axis to your cell centre.
 * The strategy is always to measure:
 * 1) Mid: The distance along a normal to the path, if that part of the path exists (otherwise set to DBL_MAX)
 * 2) End1: The distance to one end of the path that has grown so far
 * 3) End2: The distance to the other end of the path that has grown so far
 * Then take the minimum and output the closest point on the path to your cell.
 * */
template<unsigned DIM>
void DistalArmBoundaryCondition<DIM>::GetClosestOnStraight(c_vector<double, DIM>& point, c_vector<double, DIM> cell_location, double& dist){
	c_vector<double, DIM> end1=zero_vector<double>(3);
	c_vector<double, DIM> end2=zero_vector<double>(3);
	c_vector<double, DIM> mid=zero_vector<double>(3);
	if(cell_location[0]<mLeftEnd[0]+mCurrentLength && cell_location[0]>mLeftEnd[0]){
		mid[0]=cell_location[0]; mid[1]=mLeftEnd[1]; mid[2]=mLeftEnd[2];
	}else{
		mid[0]=DBL_MAX; mid[1]=DBL_MAX; mid[2]=DBL_MAX;
	}
	end1[0]=mLeftEnd[0]; end1[1]=mLeftEnd[1]; end1[2]=mLeftEnd[2];
	end2[0]=mLeftEnd[0]+mCurrentLength; end2[1]=mLeftEnd[1]; end2[2]=mLeftEnd[2];
	double R1=norm_2(end1-cell_location);
	double R2=norm_2(end2-cell_location);
	double R3=norm_2(mid-cell_location);
	dist=std::min(R1,std::min(R2,R3));
	if(dist==R1){
		point=end1;
	}else if(dist==R2){
		point=end2;
	}else{
		point=mid;
	}
}


template<unsigned DIM>
void DistalArmBoundaryCondition<DIM>::OutputCellPopulationBoundaryConditionParameters(out_stream& rParamsFile)
{

	*rParamsFile << "\t\t\t<LeftEndOfCylinderx>" << mLeftEnd[0] << "</LeftEndOfCylinderx>\n";
	*rParamsFile << "\t\t\t<LeftEndOfCylindery>" << mLeftEnd[1] << "</LeftEndOfCylindery>\n";
	*rParamsFile << "\t\t\t<LeftEndOfCylinderz>" << mLeftEnd[2] << "</LeftEndOfCylinderz>\n";
    *rParamsFile << "\t\t\t<CurrentGonadLength>" << mCurrentLength << "</CurrentGonadLength>\n";
    *rParamsFile << "\t\t\t<RadiusOfTube>" << mCurrentTubeRadius << "</RadiusOfTube>\n";
    *rParamsFile << "\t\t\t<MaximumDistance>" << mMaximumDistance << "</MaximumDistance>\n";

    // Call method on direct parent class
    AbstractCellPopulationBoundaryCondition<DIM>::OutputCellPopulationBoundaryConditionParameters(rParamsFile);
}


/////////////////////////////////////////////////////////////////////////////
// Explicit instantiation
/////////////////////////////////////////////////////////////////////////////

template class DistalArmBoundaryCondition<3>;

// Serialization for Boost >= 1.36
#include "SerializationExportWrapperForCpp.hpp"
EXPORT_TEMPLATE_CLASS_SAME_DIMS(DistalArmBoundaryCondition)
