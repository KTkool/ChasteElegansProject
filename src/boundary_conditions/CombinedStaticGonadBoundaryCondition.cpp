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

#include "CombinedStaticGonadBoundaryCondition.hpp"
#include "NodeBasedCellPopulation.hpp"

template<unsigned DIM>
CombinedStaticGonadBoundaryCondition<DIM>::CombinedStaticGonadBoundaryCondition(AbstractCellPopulation<DIM>* pCellPopulation,
		    								double StraightLengthLower,
		    								double StraightLengthUpper,
		    								double TurnRadius,
		    								double CurrentTubeRadius,
		                                    double distance)
    : AbstractCellPopulationBoundaryCondition<DIM>(pCellPopulation),
      mStraightLengthLower(StraightLengthLower),
      mStraightLengthUpper(StraightLengthUpper),
      mTurnRadius(TurnRadius),
      mCurrentTubeRadius(CurrentTubeRadius),
      mMaximumDistance(distance)
{
    assert(mStraightLengthLower > 0.0);
    assert(mStraightLengthUpper > 0.0);
    assert(mCurrentTubeRadius > 0.0);
    assert(mMaximumDistance > 0.0);
    assert(mTurnRadius > mCurrentTubeRadius); //avoid top tube merging with bottom tube
    assert(mStraightLengthUpper <= mStraightLengthLower-mCurrentTubeRadius); //avoid distal tip colliding with cell killer

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
double CombinedStaticGonadBoundaryCondition<DIM>::GetStraightLengthLower() const
{
    return mStraightLengthLower;
}
template<unsigned DIM>
double CombinedStaticGonadBoundaryCondition<DIM>::GetStraightLengthUpper() const
{
    return mStraightLengthUpper;
}
template<unsigned DIM>
double CombinedStaticGonadBoundaryCondition<DIM>::GetTurnRadius() const
{
    return mTurnRadius;
}
template<unsigned DIM>
double CombinedStaticGonadBoundaryCondition<DIM>::GetCurrentTubeRadius() const
{
    return mCurrentTubeRadius;
}



/*Checks whether each cell lies in the gonad arm. If not, places the cell on the closest surface point*/
template<unsigned DIM>
void CombinedStaticGonadBoundaryCondition<DIM>::ImposeBoundaryCondition(const std::map<Node<DIM>*, c_vector<double, DIM> >& rOldLocations)
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

        //If all three parts of the path exist, measure distances to the lower+upper straights and the loop
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

        double distance;
        HowFarAlongAreYou(C,distance);
        double SyncytiumRadius = GetSyncytiumRadius(distance);

        if(distance>mStraightLengthLower){

        	//Prevent a cell moving back down the tube to somewhere it doesn't fit
			if(radius>(mCurrentTubeRadius-SyncytiumRadius)/2){
				unsigned node_index = this->mpCellPopulation->GetLocationIndexUsingCell(*cell_iter);
				Node<DIM>* p_node = this->mpCellPopulation->GetNode(node_index);
				c_vector<double,DIM>& p_force = p_node->rGetAppliedForce();
				double damping_const = dynamic_cast<NodeBasedCellPopulation<DIM>*>(this->mpCellPopulation)->GetDampingConstant(node_index);
				p_node->rGetModifiableLocation() = cell_location-SimulationTime::Instance()->GetTimeStep()*(p_force)/damping_const;
			}else{
				// If the cell is too far inside the growth path, and therefore in the syncytium...
				if (R-radius<SyncytiumRadius-mMaximumDistance)
				{
					// ...move the cell back onto the surface of the tube by translating away from C:
					unsigned node_index = this->mpCellPopulation->GetLocationIndexUsingCell(*cell_iter);
					Node<DIM>* p_node = this->mpCellPopulation->GetNode(node_index);
					p_node->rGetModifiableLocation() = C+(SyncytiumRadius+radius)*(cell_location-C)/norm_2(cell_location-C);
				}
				// If the cell is too far from the growth path, and therefore outside the tube...
				if (R+radius-mCurrentTubeRadius > mMaximumDistance)
				{
					// ...move the cell back onto the surface of the tube by translating toward C:
					unsigned node_index = this->mpCellPopulation->GetLocationIndexUsingCell(*cell_iter);
					Node<DIM>* p_node = this->mpCellPopulation->GetNode(node_index);
					p_node->rGetModifiableLocation() = C+(mCurrentTubeRadius-radius)*(cell_location-C)/norm_2(cell_location-C);
				}
			}

        }else{
			// If the cell is too far from the growth path, and therefore outside the tube...
			if (R+radius-mCurrentTubeRadius > mMaximumDistance)
			{
				// ...move the cell back onto the surface of the tube by translating toward C:
				unsigned node_index = this->mpCellPopulation->GetLocationIndexUsingCell(*cell_iter);
				Node<DIM>* p_node = this->mpCellPopulation->GetNode(node_index);
				p_node->rGetModifiableLocation() = C+(mCurrentTubeRadius-radius)*(cell_location-C)/norm_2(cell_location-C);
			}
        }

        /*Assuming all is now well, update the cell data to record how far along the gonad arm this cell is.
         * Use the vector C, which stores the closest point on the growth path to this cell.*/
        HowFarAlongAreYou(C,distance);
        cell_iter->GetCellData()->SetItem("DistanceAwayFromDTC", mStraightLengthLower+mStraightLengthUpper+mTurnRadius*M_PI-distance);
        if(distance<(mStraightLengthLower-1)){
        	cell_iter->GetCellData()->SetItem("MaxRadius",mCurrentTubeRadius);
        }else{
        	cell_iter->GetCellData()->SetItem("MaxRadius",(mCurrentTubeRadius-SyncytiumRadius-0.1)/2);
        }
    }

}


//Check boundary condition is now satisfied
template<unsigned DIM>
bool CombinedStaticGonadBoundaryCondition<DIM>::VerifyBoundaryCondition()
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
        c_vector<double, DIM> C1=zero_vector<double>(3);
        c_vector<double, DIM> C2=zero_vector<double>(3);
        c_vector<double, DIM> C3=zero_vector<double>(3);
        double R1=0;double R2=0;double R3=0;
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

        double distance;
        HowFarAlongAreYou(C,distance);
        double SyncytiumRadius = GetSyncytiumRadius(distance);

        // If the cell is too far from the surface of the tube...
        if(distance>mStraightLengthLower){
			if (R+radius-mCurrentTubeRadius > mMaximumDistance || R-radius<SyncytiumRadius-mMaximumDistance){
				// ...then the boundary condition is not satisfied :-(
				condition_satisfied = false;
				break;
			}
        }else{
        	if (R+radius-mCurrentTubeRadius > mMaximumDistance){
        	     // ...then the boundary condition is not satisfied :-(
        	     condition_satisfied = false;
        	     break;
             }
        }
    }
    return condition_satisfied;
}


/*Calculate how far along the gonad arm some point on the growth path is*/
template<unsigned DIM>
void CombinedStaticGonadBoundaryCondition<DIM>::HowFarAlongAreYou(c_vector<double, DIM> point, double& dist){
	if(point[0]>=0 && point[1]==-mTurnRadius){
		//if on the lower straight, look at x coord
		dist=mStraightLengthLower-point[0];
	}else if(point[0]>=0 && point[1]==mTurnRadius){
		//upper straight - also look at the x coord
		dist=mStraightLengthLower+M_PI*mTurnRadius+point[0];
	}else{
		//loop: use trig.
		if(point[1]<0){
			dist=mStraightLengthLower+mTurnRadius*atan(fabs(point[0]/point[1]));
		}else{
			dist=mStraightLengthLower+mTurnRadius*(-atan(fabs(point[0]/point[1]))+M_PI);
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
void CombinedStaticGonadBoundaryCondition<DIM>::GetClosestOnLowerStraight(c_vector<double, DIM>& point, c_vector<double, DIM> cell_location, double& dist){
	c_vector<double, DIM> end1=zero_vector<double>(3);
	c_vector<double, DIM> end2=zero_vector<double>(3);
	c_vector<double, DIM> mid=zero_vector<double>(3);
	end1[0]=0; end1[1]=-mTurnRadius; end1[2]=0;
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
void CombinedStaticGonadBoundaryCondition<DIM>::GetClosestOnUpperStraight(c_vector<double, DIM>& point, c_vector<double, DIM> cell_location, double& dist){
	c_vector<double, DIM> end1=zero_vector<double>(3);
	c_vector<double, DIM> end2=zero_vector<double>(3);
	c_vector<double, DIM> mid=zero_vector<double>(3);
	end1[0]=mStraightLengthUpper; end1[1]=mTurnRadius; end1[2]=0;
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
void CombinedStaticGonadBoundaryCondition<DIM>::GetClosestOnLoop(c_vector<double, DIM>& point, c_vector<double, DIM> cell_location, double& dist){
	c_vector<double, DIM> end1=zero_vector<double>(3);
	c_vector<double, DIM> end2=zero_vector<double>(3);
	c_vector<double, DIM> mid=zero_vector<double>(3);
	end1[0]=0; end1[1]=mTurnRadius; end1[2]=0;
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
double CombinedStaticGonadBoundaryCondition<DIM>::GetSyncytiumRadius(double distance){
	if(distance>mStraightLengthLower+mTurnRadius*M_PI){
		return (mCurrentTubeRadius-6);
	}else if(distance>mStraightLengthLower){
		return (mCurrentTubeRadius-6)*(distance-mStraightLengthLower)/(mTurnRadius*M_PI);
	}else{
		return 0;
	}
};

template<unsigned DIM>
void CombinedStaticGonadBoundaryCondition<DIM>::OutputCellPopulationBoundaryConditionParameters(out_stream& rParamsFile)
{

    *rParamsFile << "\t\t\t<LengthOfLowerStraightSection>" << mStraightLengthLower << "</LengthOfLowerStraightSection>\n";
    *rParamsFile << "\t\t\t<LengthOfUpperStraightSection>" << mStraightLengthUpper << "</LengthOfUpperStraightSection>\n";
    *rParamsFile << "\t\t\t<RadiusOfTurn>" << mTurnRadius << "</RadiusOfTurn>\n";
    *rParamsFile << "\t\t\t<RadiusOfTube>" << mCurrentTubeRadius << "</RadiusOfTube>\n";
    *rParamsFile << "\t\t\t<MaximumDistance>" << mMaximumDistance << "</MaximumDistance>\n";

    // Call method on direct parent class
    AbstractCellPopulationBoundaryCondition<DIM>::OutputCellPopulationBoundaryConditionParameters(rParamsFile);
}


/////////////////////////////////////////////////////////////////////////////
// Explicit instantiation
/////////////////////////////////////////////////////////////////////////////

template class CombinedStaticGonadBoundaryCondition<3>;

// Serialization for Boost >= 1.36
#include "SerializationExportWrapperForCpp.hpp"
EXPORT_TEMPLATE_CLASS_SAME_DIMS(CombinedStaticGonadBoundaryCondition)
