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

#include"StatechartCellCycleModelSerializable.hpp"


StatechartCellCycleModelSerializable::StatechartCellCycleModelSerializable(bool LoadingFromArchive): AbstractCellCycleModel(){
	
	mLoadingFromArchive=LoadingFromArchive;
	TempVariableStorage=std::vector<double>();
	TempStateStorage=0;

	//Set some sensible C.Elegans germ cell defaults
	mSDuration=8.33;
	mG2Duration=6.66;
	mMDuration=1.66;
	mG1Duration=3.33;
	mDimension=3;
	mCurrentCellCyclePhase=G_ONE_PHASE;
    
    MAKE_PTR(CellStatechart,newStatechart);
    pStatechart=newStatechart;
};


AbstractCellCycleModel* StatechartCellCycleModelSerializable::CreateCellCycleModel(){
	//Create a new cell cycle model
	StatechartCellCycleModelSerializable* newStatechartCellCycleModelSerializable = new StatechartCellCycleModelSerializable();
	//Ensure values are inhereted from parent as appropriate
	newStatechartCellCycleModelSerializable->SetBirthTime(mBirthTime);
    newStatechartCellCycleModelSerializable->SetMinimumGapDuration(mMinimumGapDuration);
    newStatechartCellCycleModelSerializable->SetStemCellG1Duration(mStemCellG1Duration);
    newStatechartCellCycleModelSerializable->SetTransitCellG1Duration(mTransitCellG1Duration);
    newStatechartCellCycleModelSerializable->SetSDuration(mSDuration);
    newStatechartCellCycleModelSerializable->SetG2Duration(mG2Duration);
    newStatechartCellCycleModelSerializable->SetMDuration(mMDuration);
	newStatechartCellCycleModelSerializable->SetDimension(mDimension);
	newStatechartCellCycleModelSerializable->mG1Duration=mG1Duration;
	newStatechartCellCycleModelSerializable->mLoadingFromArchive=mLoadingFromArchive;
	//Create a new statechart.
	MAKE_PTR(CellStatechart, newStatechart);
	//Set its cell pointer to the parent cell to avoid it being null when constructors are called.
	newStatechart->SetCell(mpCell);
	//Copy the state of the parent. Give result to the daughter cell cycle model.
	newStatechartCellCycleModelSerializable->pStatechart=pStatechart->Copy(newStatechart);
	//Return the new cell cycle model. The cell pointer will be made to point to the daughter when SetCell is called.
	return newStatechartCellCycleModelSerializable;
 };


void StatechartCellCycleModelSerializable::SetCell(CellPtr pCell){
    mpCell = pCell;
	//Switch the statechart's cell pointer to point to this cell.
 	pStatechart->SetCell(mpCell);
 	//If we're loading from an archive, now is an appropriate time to initiate the statechart and set the stored state
	//and variables. 
	if(mLoadingFromArchive==true){
		pStatechart->initiate();
		pStatechart->SetState(TempStateStorage);
		pStatechart->SetVariables(TempVariableStorage);
		mLoadingFromArchive=false;
	}
 };


void StatechartCellCycleModelSerializable::Initialise(){
	pStatechart->initiate();
 	//Handles advancing the cell to some point in the cell cycle so that the population
 	//DOES NOT start out synchronised.

 	//1) Get the cell's randomly generated age and work out what phase it's in and how long it's been there.
	double randStartingAge = mpCell->GetAge();
	double intPart;
	modf(randStartingAge/(mG1Duration+mG2Duration+mMDuration+mSDuration),&intPart);
	double remainder=randStartingAge-intPart*(mG1Duration+mG2Duration+mMDuration+mSDuration);

	//Fire an event to force the statechart to the appropriate phase
	CellCyclePhase_ phase;
	if(remainder<mG1Duration){
		pStatechart->process_event(EvGoToCellStateChart_CellCycle_Mitosis_G1());
		pStatechart->TimeInPhase=remainder;
	}else if(remainder>mG1Duration && remainder<mG1Duration+mSDuration){
		pStatechart->process_event(EvGoToCellStateChart_CellCycle_Mitosis_S());
		pStatechart->TimeInPhase=remainder-mG1Duration;
	}
	else if(remainder>mG1Duration+mSDuration && remainder<mG1Duration+mSDuration+mG2Duration){
		pStatechart->process_event(EvGoToCellStateChart_CellCycle_Mitosis_G2());		
		pStatechart->TimeInPhase=remainder-(mG1Duration+mSDuration);
	}else{
		pStatechart->process_event(EvGoToCellStateChart_CellCycle_Mitosis_M());		
		pStatechart->TimeInPhase=remainder-(mG1Duration+mSDuration+mG2Duration);
	}
};


bool StatechartCellCycleModelSerializable::ReadyToDivide(){
	if (!mReadyToDivide)	//If not dividing, update the statechart
	{
	    UpdateCellCyclePhase();
	}
	return mReadyToDivide;  //Return whether ready to divide now
};

void StatechartCellCycleModelSerializable::UpdateCellCyclePhase(){
	//To update the phase, just update the statechart
	pStatechart->process_event(EvCheckCellData());
};

void StatechartCellCycleModelSerializable::ResetForDivision(){
	//To reset, change the mReadyToDivide flag to false: the message has been received
	mReadyToDivide=false;
};

//Setter methods that allow the statechart to influence the cell cycle model
void StatechartCellCycleModelSerializable::SetCellCyclePhase(CellCyclePhase_ Phase){
	mCurrentCellCyclePhase=Phase;
};
void StatechartCellCycleModelSerializable::SetReadyToDivide(bool Ready){
	mReadyToDivide=Ready;
};

//Standard outputting of parameters associated with the cell cycle model to a params file for storage
//Haven't added any new parameters in particular.
void StatechartCellCycleModelSerializable::OutputCellCycleModelParameters(out_stream& rParamsFile)
{
	AbstractCellCycleModel::OutputCellCycleModelParameters( rParamsFile);
}

// Serialization for Boost >= 1.36
#include "SerializationExportWrapperForCpp.hpp"
CHASTE_CLASS_EXPORT(StatechartCellCycleModelSerializable)
