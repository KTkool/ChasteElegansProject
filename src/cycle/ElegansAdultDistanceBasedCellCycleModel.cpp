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

#include "ElegansAdultDistanceBasedCellCycleModel.hpp"
#include "Exception.hpp"
#include "StemCellProliferativeType.hpp"
#include "TransitCellProliferativeType.hpp"
#include "DifferentiatedCellProliferativeType.hpp"

ElegansAdultDistanceBasedCellCycleModel::ElegansAdultDistanceBasedCellCycleModel()
    : AbstractSimpleCellCycleModel(),
    maxG1Duration(1.66), //makes maximum cell cycle length in proliferative zone 5 hours :-/
    ProlifZoneLength(100)
    {
}

AbstractCellCycleModel* ElegansAdultDistanceBasedCellCycleModel::CreateCellCycleModel()
{
    // Create a new cell-cycle model
    ElegansAdultDistanceBasedCellCycleModel* p_model = new ElegansAdultDistanceBasedCellCycleModel();

    /*Set durations; manually give number for most variables because C. elegans G1 duration
     */
    p_model->SetBirthTime(mBirthTime);
    p_model->SetMinimumGapDuration(mMinimumGapDuration);
    p_model->SetStemCellG1Duration(0.66);
    p_model->SetTransitCellG1Duration(0.66);
    p_model->SetSDuration(1.66);
    p_model->SetG2Duration(1.33);
    p_model->SetMDuration(0.33);

    return p_model;
}

void ElegansAdultDistanceBasedCellCycleModel::SetG1Duration()
{

	/*Loop through possible properties of a cell (proliferative or non proliferative) and set G1 duration accordingly
	 * given this info and distance from DTC.
	*/
	RandomNumberGenerator* p_gen = RandomNumberGenerator::Instance();
	double proliferative = mpCell->GetCellData()->GetItem("Proliferating");

    if (proliferative==0.0)
    {
    	mG1Duration = DBL_MAX;
    }
    else if (proliferative==1.0)
    {
    	double distanceAwayFromDTC = mpCell->GetCellData()->GetItem("DistanceAwayFromDTC");
    	double deterministicDuration=mTransitCellG1Duration+(maxG1Duration-mTransitCellG1Duration)*distanceAwayFromDTC/ProlifZoneLength;
    	mG1Duration = deterministicDuration + deterministicDuration*(0.2*p_gen->ranf()-0.1); // allow 10% uniform variation about deterministic G1 value
    }
    else
    {
        NEVER_REACHED;
    }
}


void ElegansAdultDistanceBasedCellCycleModel::OutputCellCycleModelParameters(out_stream& rParamsFile)
{
    // No new parameters to output, so just call method on direct parent class
    AbstractSimpleCellCycleModel::OutputCellCycleModelParameters(rParamsFile);
}

// Serialization for Boost >= 1.36
#include "SerializationExportWrapperForCpp.hpp"
CHASTE_CLASS_EXPORT(ElegansAdultDistanceBasedCellCycleModel)
