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


StatechartCellCycleModelSerializable::StatechartCellCycleModelSerializable(): AbstractCellCycleModel(){
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
	//Copy the parent's statechart.
	boost::shared_ptr<CellStatechart> newStatechart=pStatechart->Copy();
	//Give the new statechart to the daughter. Its cell pointer will be set when SetCell is called by the 
    //daughter cell's constructor 
	newStatechartCellCycleModelSerializable->pStatechart=newStatechart;
	return newStatechartCellCycleModelSerializable;
 };


void StatechartCellCycleModelSerializable::SetCell(CellPtr pCell){
    mpCell = pCell;
	//NEW BIT: Switch the statechart's cell pointer to point to this cell.
 	pStatechart->SetCell(mpCell);
 	printf("%s\n","SettingStatechartCellPointer");
 };


void StatechartCellCycleModelSerializable::Initialise(){
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
		pStatechart->TimeInPhase=remainder;
		pStatechart->process_event(EvGoToG1());
	}else if(remainder>mG1Duration && remainder<mG1Duration+mSDuration){
		pStatechart->TimeInPhase=remainder-mG1Duration;
		pStatechart->process_event(EvGoToS());
	}
	else if(remainder>mG1Duration+mSDuration && remainder<mG1Duration+mSDuration+mG2Duration){
		pStatechart->TimeInPhase=remainder-(mG1Duration+mSDuration);
		pStatechart->process_event(EvGoToG2());
	}else{
		pStatechart->TimeInPhase=remainder-(mG1Duration+mSDuration+mG2Duration);
		pStatechart->process_event(EvGoToM());
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
