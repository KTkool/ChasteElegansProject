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

#include "GonadArmForcedBoundaryCondition.hpp"
#include "NodeBasedCellPopulation.hpp"
#define PI 3.1415926

template<unsigned DIM>
GonadArmForcedBoundaryCondition<DIM>::GonadArmForcedBoundaryCondition(AbstractCellPopulation<DIM>* pCellPopulation,
											double CurrentLength,
		    								double StraightLengthLower,
		    								double StraightLengthUpper,
		    								double TurnRadius,
		    								double CurrentTubeRadius,
		    								double FinalTubeRadius,
		    								double GrowthRateLinear,
		    								double GrowthRateRadial,
		    								double DampingConstant)
    : AbstractCellPopulationBoundaryCondition<DIM>(pCellPopulation),
      mCurrentLength(CurrentLength),
      mFinalLength(StraightLengthLower+StraightLengthUpper+TurnRadius*PI),
      mStraightLengthLower(StraightLengthLower),
      mStraightLengthUpper(StraightLengthUpper),
      mTurnRadius(TurnRadius),
      mCurrentTubeRadius(CurrentTubeRadius),
      mFinalTubeRadius(FinalTubeRadius),
      mGrowthRateLinear(GrowthRateLinear),
      mGrowthRateRadial(GrowthRateRadial),
      mDampingConstant(DampingConstant),
      mNetForceOnDT(0) //Used for storing the amount by which the gonad is being stretched
{
	if(mCurrentLength>mFinalLength){
		mCurrentLength=mFinalLength;
	}
    assert(mCurrentLength > 0.0);
    assert(mStraightLengthLower > 0.0);
    assert(mStraightLengthUpper > 0.0);
    assert(mCurrentTubeRadius > 0.0);
    assert(mFinalTubeRadius >= mCurrentTubeRadius);
    assert(mGrowthRateLinear >= 0.0);
    assert(mGrowthRateRadial >= 0.0);
    assert(mDampingConstant >= 0.0);
    assert(mTurnRadius > mFinalTubeRadius); //avoid top tube merging with bottom tube
    assert(mStraightLengthUpper <= mStraightLengthLower-mFinalTubeRadius); //avoid distal tip colliding with cell killer

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
double GonadArmForcedBoundaryCondition<DIM>::GetCurrentLength() const
{
    return mCurrentLength;
}
template<unsigned DIM>
double GonadArmForcedBoundaryCondition<DIM>::GetFinalLength() const
{
    return mFinalLength;
}
template<unsigned DIM>
double GonadArmForcedBoundaryCondition<DIM>::GetStraightLengthLower() const
{
    return mStraightLengthLower;
}
template<unsigned DIM>
double GonadArmForcedBoundaryCondition<DIM>::GetStraightLengthUpper() const
{
    return mStraightLengthUpper;
}
template<unsigned DIM>
double GonadArmForcedBoundaryCondition<DIM>::GetTurnRadius() const
{
    return mTurnRadius;
}
template<unsigned DIM>
double GonadArmForcedBoundaryCondition<DIM>::GetCurrentTubeRadius() const
{
    return mCurrentTubeRadius;
}
template<unsigned DIM>
double GonadArmForcedBoundaryCondition<DIM>::GetFinalTubeRadius() const
{
    return mFinalTubeRadius;
}
template<unsigned DIM>
double GonadArmForcedBoundaryCondition<DIM>::GetGrowthRateLinear() const
{
    return mGrowthRateLinear;
}
template<unsigned DIM>
double GonadArmForcedBoundaryCondition<DIM>::GetGrowthRateRadial() const
{
    return mGrowthRateRadial;
}
template<unsigned DIM>
double GonadArmForcedBoundaryCondition<DIM>::GetDampingConstant() const
{
    return mDampingConstant;
}






/*Update function. Increases gonad arm length by mGrowthRateLinear each timestep, up to FinalLength and by
 * mGrowthRateRadial each timestep, up to FinalTubeRadius
 * */
template<unsigned DIM>
void GonadArmForcedBoundaryCondition<DIM>::GrowBoundary(){
	if(mCurrentLength<mFinalLength){
		mCurrentLength=mCurrentLength+SimulationTime::Instance()->GetTimeStep()*mGrowthRateLinear;
	}
	if(mCurrentTubeRadius<mFinalTubeRadius){
		mCurrentTubeRadius=mCurrentTubeRadius+SimulationTime::Instance()->GetTimeStep()*mGrowthRateRadial;
	}
}

/*Update function. Increases gonad arm length in response to the force exerted by cells. This effect is
 * ignored on reaching a prescribed max length.*/
template<unsigned DIM>
void GonadArmForcedBoundaryCondition<DIM>::StretchBoundary(){
	if(mCurrentLength<mFinalLength){
		mCurrentLength=mCurrentLength+mNetForceOnDT*SimulationTime::Instance()->GetTimeStep()/mDampingConstant;
		mNetForceOnDT=0;
	}
}







/*Checks whether each cell lies in the gonad arm. If not, allow the cell to move some distance outside by stretching.*/
template<unsigned DIM>
void GonadArmForcedBoundaryCondition<DIM>::ImposeBoundaryCondition(const std::map<Node<DIM>*, c_vector<double, DIM> >& rOldLocations)
{

	//First, grow up to the size given by prescribed growth
	GrowBoundary();

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
        GetClosestPointOnGrowthPath(C,cell_location,R);


        // If the cell is too far from the growth path, and therefore outside the tube...
        if (R+radius-mCurrentTubeRadius > 1e-5)
        {

        	//If cell is in the distal end, record force exerted on the gonad boundary
            double distance=0;
            HowFarAlongAreYou(C,distance);

        	if(mCurrentLength-distance<=1e-5){

				//get the force on the cell
				unsigned node_index = this->mpCellPopulation->GetLocationIndexUsingCell(*cell_iter);
				Node<DIM>* p_node = this->mpCellPopulation->GetNode(node_index);
				c_vector<double,DIM>& p_force = p_node->rGetAppliedForce();

				//Get force magnitude in the direction tangent to the growth path
				c_vector<double,DIM> tangent=GetTangentToGrowthPath();
				double forceApplied=0;
				for (int d =0; d<3; d++){
					forceApplied=forceApplied+tangent[d]*p_force[d];
				}

				if(forceApplied>0){ //If the force is pushing the endcap outward, record it
					mNetForceOnDT=mNetForceOnDT+forceApplied;
				}
        	}

            //set new location for the cell, inside the current tube boundary.
            unsigned node_index = this->mpCellPopulation->GetLocationIndexUsingCell(*cell_iter);
            Node<DIM>* p_node = this->mpCellPopulation->GetNode(node_index);
            p_node->rGetModifiableLocation() = C+(mCurrentTubeRadius-radius)*(cell_location-C)/norm_2(cell_location-C);

        }

        /*Assuming all is well, update the cell data to record how far along the gonad arm this cell is.
        * Use the vector C, which stores the closest point on the growth path to this cell.*/
        double distance;
        HowFarAlongAreYou(C,distance);
        cell_iter->GetCellData()->SetItem("DistanceAwayFromDTC", mCurrentLength-distance);

    }

    StretchBoundary();
}





//Check boundary condition is now satisfied
template<unsigned DIM>
bool GonadArmForcedBoundaryCondition<DIM>::VerifyBoundaryCondition()
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
        GetClosestPointOnGrowthPath(C,cell_location,R);

        // If the cell is too far from the surface of the tube...
        if (R+radius-mCurrentTubeRadius > 1e-5)
        {
            // ...then the boundary condition is not satisfied :-(
            condition_satisfied = false;
            break;
        }

    }
    return condition_satisfied;
}



template<unsigned DIM>
c_vector<double,DIM> GonadArmForcedBoundaryCondition<DIM>::GetTangentToGrowthPath(){
	c_vector<double, DIM> tangent;
	if(mCurrentLength<mStraightLengthLower){
		//if on the lower straight
		tangent[0]=-1; tangent[1]=0; tangent[2]=0;
	}else if(mCurrentLength>=mStraightLengthLower+PI*mTurnRadius){
		//upper straight
		tangent[0]=1; tangent[1]=0; tangent[2]=0;
	}else{
		//loop: use trig.
		double angleRound=(PI*(mCurrentLength-mStraightLengthLower)/(PI*mTurnRadius))+PI/2;
		tangent[0]=cos(angleRound); tangent[1]=sin(angleRound); tangent[2]=0;
	}
	return tangent;
}

/*Calculate how far along the gonad arm some point on the growth path is*/
template<unsigned DIM>
void GonadArmForcedBoundaryCondition<DIM>::HowFarAlongAreYou(c_vector<double, DIM> point, double& dist){
	if(point[0]>=0 && point[1]==-mTurnRadius){
		//if on the lower straight, look at x coord
		dist=mStraightLengthLower-point[0];
	}else if(point[0]>=0 && point[1]==mTurnRadius){
		//upper straight - also look at the x coord
		dist=mStraightLengthLower+PI*mTurnRadius+point[0];
	}else{
		//loop: use trig.
		if(point[1]<0){
			dist=mStraightLengthLower+mTurnRadius*atan(fabs(point[0]/point[1]));
		}else{
			dist=mStraightLengthLower+mTurnRadius*(-atan(fabs(point[0]/point[1]))+PI);
		}
	}
}




/*Utility functions for determining the closest point on each section of the growth path to your cell centre.
 * The strategy is always to measure:
 * 1) Mid: The distance along a normal to the path, if that part of the path exists (otherwise set to DBL_MAX)
 * 2) End1: The distance to one end of the path that has grown so far
 * 3) End2: The distance to the other end of the path that has grown so far
 * Then take the minimum and output the closest point on the path to your cell.
 * */

template<unsigned DIM>
void GonadArmForcedBoundaryCondition<DIM>::GetClosestPointOnGrowthPath(c_vector<double,DIM>& C, c_vector<double,DIM> cell_location, double& R){
	//If all three parts of the path exist, measure distances to the lower+upper straights and the loop
	        if(mCurrentLength>mStraightLengthLower+PI*mTurnRadius){
	        	//Find min distance to each of the three parts of the path
	        	double R1=0; double R2=0; double R3=0;
	        	c_vector<double, DIM> C1=zero_vector<double>(3);
	        	c_vector<double, DIM> C2=zero_vector<double>(3);
	        	c_vector<double, DIM> C3=zero_vector<double>(3);
	        	GetClosestOnLowerStraight(C1,cell_location,R1);
	        	GetClosestOnLoop(C2,cell_location,R2);
	        	GetClosestOnUpperStraight(C3,cell_location,R3);
	        	//Take min and record closest point
	        	R=std::min(R1,std::min(R2,R3));
	        	if(R==R1){
	        		C=C1;
	        	}else if(R==R2){
	        		C=C2;
	        	}else{
	        		C=C3;
	        	}
	        }else if(mCurrentLength>mStraightLengthLower && mCurrentLength<=mStraightLengthLower+PI*mTurnRadius){
	        	//If only the lower straight and the loop exist, similar process
	        	double R1=0; double R2=0;
	        	c_vector<double, DIM> C1=zero_vector<double>(3);
	        	c_vector<double, DIM> C2=zero_vector<double>(3);
	        	GetClosestOnLowerStraight(C1,cell_location,R1);
	        	GetClosestOnLoop(C2,cell_location,R2);
	        	R=std::min(R1,R2);
	        	if(R==R1){
	        		C=C1;
	        	}else{
	        		C=C2;
	        	}
	        }else if(mCurrentLength<=mStraightLengthLower){
	        	//Distance to the lower straight of the gonad arm
	        	GetClosestOnLowerStraight(C,cell_location,R);
	        }else{
	        	NEVER_REACHED;
	        }
}

template<unsigned DIM>
void GonadArmForcedBoundaryCondition<DIM>::GetClosestOnLowerStraight(c_vector<double, DIM>& point, c_vector<double, DIM> cell_location, double& dist){
	c_vector<double, DIM> end1=zero_vector<double>(3);
	c_vector<double, DIM> end2=zero_vector<double>(3);
	c_vector<double, DIM> mid=zero_vector<double>(3);
	if(mCurrentLength<mStraightLengthLower){
		end1[0]=mStraightLengthLower-mCurrentLength; end1[1]=-mTurnRadius; end1[2]=0;
	}else{
		end1[0]=0; end1[1]=-mTurnRadius; end1[2]=0;
	}
	end2[0]=mStraightLengthLower; end2[1]=-mTurnRadius; end2[2]=0;
	if(cell_location[0]<end2[0] && cell_location[0]>end1[0]){
		mid[0]=cell_location[0]; mid[1]=-mTurnRadius; mid[2]=0;
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
void GonadArmForcedBoundaryCondition<DIM>::GetClosestOnUpperStraight(c_vector<double, DIM>& point, c_vector<double, DIM> cell_location, double& dist){
	c_vector<double, DIM> end1=zero_vector<double>(3);
	c_vector<double, DIM> end2=zero_vector<double>(3);
	c_vector<double, DIM> mid=zero_vector<double>(3);
	if(mCurrentLength<mFinalLength){
		end1[0]=mStraightLengthUpper+mCurrentLength-mFinalLength; end1[1]=mTurnRadius; end1[2]=0;
	}else{
		end1[0]=mStraightLengthUpper; end1[1]=mTurnRadius; end1[2]=0;
	}
	end2[0]=0; end2[1]=mTurnRadius; end2[2]=0;
	if(cell_location[0]<end1[0] && cell_location[0]>end2[0]){
		mid[0]=cell_location[0]; mid[1]=mTurnRadius; mid[2]=0;
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
void GonadArmForcedBoundaryCondition<DIM>::GetClosestOnLoop(c_vector<double, DIM>& point, c_vector<double, DIM> cell_location, double& dist){
	c_vector<double, DIM> end1=zero_vector<double>(3);
	c_vector<double, DIM> end2=zero_vector<double>(3);
	c_vector<double, DIM> mid=zero_vector<double>(3);
	if(mCurrentLength<mStraightLengthLower+PI*mTurnRadius){
		double angleRoundLoop=0.5*PI+PI*((-mStraightLengthLower+mCurrentLength)/(PI*mTurnRadius));
		end1[0]=mTurnRadius*cos(angleRoundLoop); end1[1]=-mTurnRadius*sin(angleRoundLoop); end1[2]=0;
	}else{
		end1[0]=0; end1[1]=mTurnRadius; end1[2]=0;
	}
	end2[0]=0; end2[1]=-mTurnRadius; end2[2]=0;
	mid[0]=-fabs(mTurnRadius*cell_location[0]/pow(pow(cell_location[0],2)+pow(cell_location[1],2),0.5));
	if(cell_location[1]<0){
		mid[1]=-fabs(mTurnRadius*cell_location[1]/pow(pow(cell_location[0],2)+pow(cell_location[1],2),0.5));
	}else{
		mid[1]=fabs(mTurnRadius*cell_location[1]/pow(pow(cell_location[0],2)+pow(cell_location[1],2),0.5));
	}
	mid[2]=0;
	double d1; double d2;
	HowFarAlongAreYou(end1,d1);
	HowFarAlongAreYou(mid,d2);
	if(d2>d1){mid[0]=DBL_MAX;mid[1]=DBL_MAX;mid[2]=DBL_MAX;}
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
void GonadArmForcedBoundaryCondition<DIM>::OutputCellPopulationBoundaryConditionParameters(out_stream& rParamsFile)
{

    *rParamsFile << "\t\t\t<FinalGonadLength>" << mFinalLength << "</FinalGonadLength>\n";
    *rParamsFile << "\t\t\t<CurrentGonadLength>" << mCurrentLength << "</CurrentGonadLength>\n";
    *rParamsFile << "\t\t\t<LengthOfLowerStraightSection>" << mStraightLengthLower << "</LengthOfLowerStraightSection>\n";
    *rParamsFile << "\t\t\t<LengthOfUpperStraightSection>" << mStraightLengthUpper << "</LengthOfUpperStraightSection>\n";
    *rParamsFile << "\t\t\t<RadiusOfTurn>" << mTurnRadius << "</RadiusOfTurn>\n";
    *rParamsFile << "\t\t\t<RadiusOfTube>" << mCurrentTubeRadius << "</RadiusOfTube>\n";
    *rParamsFile << "\t\t\t<FinalRadiusOfTube>" << mFinalTubeRadius << "</FinalRadiusOfTube>\n";
    *rParamsFile << "\t\t\t<GonadLinearGrowthRate>" << mGrowthRateLinear << "</GonadLinearGrowthRate>\n";
    *rParamsFile << "\t\t\t<GonadRadialGrowthRate>" << mGrowthRateRadial << "</GonadRadialGrowthRate>\n";

    // Call method on direct parent class
    AbstractCellPopulationBoundaryCondition<DIM>::OutputCellPopulationBoundaryConditionParameters(rParamsFile);
}


/////////////////////////////////////////////////////////////////////////////
// Explicit instantiation
/////////////////////////////////////////////////////////////////////////////

template class GonadArmForcedBoundaryCondition<3>;

// Serialization for Boost >= 1.36
#include "SerializationExportWrapperForCpp.hpp"
EXPORT_TEMPLATE_CLASS_SAME_DIMS(GonadArmForcedBoundaryCondition)
