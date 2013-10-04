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

#include"StatechartCellCycleModel.hpp"

     StatechartCellCycleModel::StatechartCellCycleModel(): AbstractCellCycleModel(){
    	 //Set some sensible C.Elegans larval Defaults
    	 mSDuration=8.33;
    	 mG2Duration=6.67;
    	 mMDuration=1.67;
    	 mG1Duration=3.33;

    	 mDimension=3;
    	 mCurrentCellCyclePhase=G_ONE_PHASE;
     };

     AbstractCellCycleModel* StatechartCellCycleModel::CreateCellCycleModel(){
    	 //Create a new model
    	 StatechartCellCycleModel* newStatechartCellCycleModel = new StatechartCellCycleModel();
    	 //Copy the parent's statechart. At this point the new statechart still holds a pointer to the
    	 //parent's cell
    	 boost::shared_ptr<CellStatechart> newStatechartOldCell=pStatechart->Copy(mpCell);
    	 //Give the new statechart to the duaghter
    	 newStatechartCellCycleModel->pStatechart=newStatechartOldCell;
    	 return newStatechartCellCycleModel;
     };


    void StatechartCellCycleModel::InitialiseDaughterCell(){
    	//Switch the statechart's cell pointer to point to the daughter cell.
     	pStatechart->SetCell(mpCell);
     };


    void StatechartCellCycleModel::Initialise(){
    	//Make a statechart for this starting cell
     	MAKE_PTR_ARGS(CellStatechart,newStatechart,(mpCell));
     	pStatechart=newStatechart;

     	//Handles advancing the cell to some random point in the cell cycle so that the population
     	//DOES NOT start out synchronised.

     	//1) Get the cell's randomly generated age and work out what phase it's in and how long it's been there.
     	double randStartingAge = mpCell->GetAge();
     	double intPart;
     	modf(randStartingAge/(mG1Duration+mG2Duration+mMDuration+mSDuration),&intPart);
     	double remainder=randStartingAge-intPart;
     	double timeInState;

     	//Fire an event to force the statechart to the appropriate phase
     	CellCyclePhase_ phase;
     	if(remainder<mG1Duration){
     		timeInState=remainder;
         	pStatechart->process_event(EvGoToG1()/*EvGoToCellStateChart_Abstract_Mitosis_G_ONE()*/);
     	}else if(remainder>mG1Duration && remainder<mG1Duration+mSDuration){
     		timeInState=remainder-mG1Duration;
     		pStatechart->process_event(EvGoToS()/*EvGoToCellStateChart_Abstract_Mitosis_S()*/);
     	}
     	else if(remainder>mG1Duration+mSDuration && remainder<mG1Duration+mSDuration+mG2Duration){
     		timeInState=remainder-(mG1Duration+mSDuration);
     		pStatechart->process_event(EvGoToG2()/*EvGoToCellStateChart_Abstract_Mitosis_G_TWO()*/);
     	}else{
     		timeInState=remainder-(mG1Duration+mSDuration+mG2Duration);
     		pStatechart->process_event(EvGoToM()/*EvGoToCellStateChart_Abstract_Mitosis_M()*/);
     	}

     	//Fire another event to set the time the cell has been in this phase
     	pStatechart->process_event(EvGoToTime(timeInState));
     };


     bool StatechartCellCycleModel::ReadyToDivide(){
    	    assert(mpCell != NULL);
    	    if (!mReadyToDivide)	//If not dividing, update the statechart
    	    {
    	        UpdateCellCyclePhase();
    	    }
    	    return mReadyToDivide;
     };


    void StatechartCellCycleModel::UpdateCellCyclePhase(){
    	//To update phase, update the statechart
    	pStatechart->process_event(EvCheckCellData());
    };

    void StatechartCellCycleModel::ResetForDivision(){
    	//To reset, change the mReadyToDivide flag to false - message has been received
    	mReadyToDivide=false;
    };


    //Setter methods that allow the statechart to update the cell cycle model
    void StatechartCellCycleModel::SetCellCyclePhase(CellCyclePhase_ Phase){
    	mCurrentCellCyclePhase=Phase;
    };
    void StatechartCellCycleModel::SetReadyToDivide(bool Ready){
    	mReadyToDivide=Ready;
    };

    void StatechartCellCycleModel::OutputCellCycleModelParameters(out_stream& rParamsFile)
    {
    	AbstractCellCycleModel::OutputCellCycleModelParameters( rParamsFile);
    }


// Serialization for Boost >= 1.36
#include "SerializationExportWrapperForCpp.hpp"
CHASTE_CLASS_EXPORT(StatechartCellCycleModel)
