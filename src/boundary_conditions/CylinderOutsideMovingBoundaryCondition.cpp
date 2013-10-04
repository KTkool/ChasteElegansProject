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

#include "CylinderOutsideMovingBoundaryCondition.hpp"
#include "NodeBasedCellPopulation.hpp"

template<unsigned DIM>
CylinderOutsideMovingBoundaryCondition<DIM>::CylinderOutsideMovingBoundaryCondition(AbstractCellPopulation<DIM>* pCellPopulation,
											c_vector<double, DIM> leftEnd,
											double CurrentLength,
											double FinalLength,
		    								double CurrentTubeRadius,
		    								double FinalTubeRadius,
		    								double GrowthRateLinear,
		    								double GrowthRateRadial,
		                                    double distance)
    : AbstractMovingBoundaryCondition<DIM>(pCellPopulation),
      mLeftEnd(leftEnd),
      mCurrentLength(CurrentLength),
      mFinalLength(FinalLength),
      mCurrentTubeRadius(CurrentTubeRadius),
      mFinalTubeRadius(FinalTubeRadius),
      mGrowthRateLinear(GrowthRateLinear),
      mGrowthRateRadial(GrowthRateRadial),
      mMaximumDistance(distance)
{

	assert(mFinalLength >= mCurrentLength);
    assert(mFinalTubeRadius >= mCurrentTubeRadius);
    assert(mGrowthRateLinear > 0.0);
    assert(mGrowthRateRadial > 0.0);

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
double CylinderOutsideMovingBoundaryCondition<DIM>::GetCurrentLength() const
{
    return mCurrentLength;
}
template<unsigned DIM>
double CylinderOutsideMovingBoundaryCondition<DIM>::GetFinalLength() const
{
    return mFinalLength;
}
template<unsigned DIM>
double CylinderOutsideMovingBoundaryCondition<DIM>::GetCurrentTubeRadius() const
{
    return mCurrentTubeRadius;
}
template<unsigned DIM>
double CylinderOutsideMovingBoundaryCondition<DIM>::GetFinalTubeRadius() const
{
    return mFinalTubeRadius;
}
template<unsigned DIM>
double CylinderOutsideMovingBoundaryCondition<DIM>::GetGrowthRateLinear() const
{
    return mGrowthRateLinear;
}
template<unsigned DIM>
double CylinderOutsideMovingBoundaryCondition<DIM>::GetGrowthRateRadial() const
{
    return mGrowthRateRadial;
}
template<unsigned DIM>
c_vector<double, DIM> CylinderOutsideMovingBoundaryCondition<DIM>::GetLeftEnd() const
{
    return mLeftEnd;
}


/*Update function. Increases gonad arm length by mGrowthRate each timestep, up to LengthMax*/
template<unsigned DIM>
void CylinderOutsideMovingBoundaryCondition<DIM>::UpdateBoundaryCondition(){
	if(mCurrentLength<mFinalLength){
		mCurrentLength=mCurrentLength+SimulationTime::Instance()->GetTimeStep()*mGrowthRateLinear;
	}
	if(mCurrentTubeRadius<mFinalTubeRadius){
		mCurrentTubeRadius=mCurrentTubeRadius+SimulationTime::Instance()->GetTimeStep()*mGrowthRateRadial;
	}
}


/*Checks whether each cell lies outside the tube. If not, places the cell centre so that the cell just
 * touches the surface of the tube*/
template<unsigned DIM>
void CylinderOutsideMovingBoundaryCondition<DIM>::ImposeBoundaryCondition(const std::map<Node<DIM>*, c_vector<double, DIM> >& rOldLocations)
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


        // If the cell is too far from the growth path, and therefore outside the tube...
        if (R-radius< mCurrentTubeRadius- mMaximumDistance)
        {
        	// ...move the cell back onto the surface of the tube by translating away from C:
            unsigned node_index = this->mpCellPopulation->GetLocationIndexUsingCell(*cell_iter);
            Node<DIM>* p_node = this->mpCellPopulation->GetNode(node_index);
            p_node->rGetModifiableLocation() = C+(mCurrentTubeRadius+radius)*(cell_location-C)/norm_2(cell_location-C);

        }
    }
}


//Check boundary condition is now satisfied
template<unsigned DIM>
bool CylinderOutsideMovingBoundaryCondition<DIM>::VerifyBoundaryCondition()
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
        if (R-radius< mCurrentTubeRadius-mMaximumDistance)
        {
            // ...then the boundary condition is not satisfied :-(
            condition_satisfied = false;
            break;
        }
    }
    return condition_satisfied;
}


/*Utility function for determining the closest point on the long axis to a cell centre.
 * The strategy is always to measure:
 * 1) Mid: The distance along a normal to the axis, if that part of the cylinder exists (otherwise set to DBL_MAX)
 * 2) End1: The distance to one end of the cylinder that has grown so far
 * 3) End2: The distance to the other end of the cylinder that has grown so far
 * Then take the minimum and output the closest point on the path to your cell.
 * */
template<unsigned DIM>
void CylinderOutsideMovingBoundaryCondition<DIM>::GetClosestOnStraight(c_vector<double, DIM>& point, c_vector<double, DIM> cell_location, double& dist){
	c_vector<double, DIM> end1=zero_vector<double>(3);
	c_vector<double, DIM> end2=zero_vector<double>(3);
	c_vector<double, DIM> mid=zero_vector<double>(3);
	if(mCurrentLength<mFinalLength){
		end1[0]=mCurrentLength; end1[1]=mLeftEnd[1]; end1[2]=0;
	}else{
		end1[0]=mFinalLength; end1[1]=mLeftEnd[1]; end1[2]=0;
	}
	end2[0]=mLeftEnd[0]; end2[1]=mLeftEnd[1]; end2[2]=0;
	if(cell_location[0]>end2[0] && cell_location[0]<end1[0]){
		mid[0]=cell_location[0]; mid[1]=mLeftEnd[1]; mid[2]=0;
	}else{
		mid[0]=DBL_MAX; mid[1]=DBL_MAX; mid[2]=DBL_MAX;
	}
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
void CylinderOutsideMovingBoundaryCondition<DIM>::OutputCellPopulationBoundaryConditionParameters(out_stream& rParamsFile)
{

	*rParamsFile << "\t\t\t<LeftEndOfCylinderx>" << mLeftEnd[0] << "</LeftEndOfCylinderx>\n";
	*rParamsFile << "\t\t\t<LeftEndOfCylindery>" << mLeftEnd[1] << "</LeftEndOfCylindery>\n";
	*rParamsFile << "\t\t\t<LeftEndOfCylinderz>" << mLeftEnd[2] << "</LeftEndOfCylinderz>\n";
    *rParamsFile << "\t\t\t<CurrentGonadLength>" << mCurrentLength << "</CurrentGonadLength>\n";
    *rParamsFile << "\t\t\t<FinalGonadLength>" << mFinalLength << "</FinalGonadLength>\n";
    *rParamsFile << "\t\t\t<RadiusOfTube>" << mCurrentTubeRadius << "</RadiusOfTube>\n";
    *rParamsFile << "\t\t\t<FinalRadiusOfTube>" << mFinalTubeRadius << "</FinalRadiusOfTube>\n";
    *rParamsFile << "\t\t\t<GonadLinearGrowthRate>" << mGrowthRateLinear << "</GonadLinearGrowthRate>\n";
    *rParamsFile << "\t\t\t<GonadRadialGrowthRate>" << mGrowthRateRadial << "</GonadRadialGrowthRate>\n";
    *rParamsFile << "\t\t\t<MaximumDistance>" << mMaximumDistance << "</MaximumDistance>\n";

    // Call method on direct parent class
    AbstractMovingBoundaryCondition<DIM>::OutputCellPopulationBoundaryConditionParameters(rParamsFile);
}


/////////////////////////////////////////////////////////////////////////////
// Explicit instantiation
/////////////////////////////////////////////////////////////////////////////

template class CylinderOutsideMovingBoundaryCondition<3>;

// Serialization for Boost >= 1.36
#include "SerializationExportWrapperForCpp.hpp"
EXPORT_TEMPLATE_CLASS_SAME_DIMS(CylinderOutsideMovingBoundaryCondition)
