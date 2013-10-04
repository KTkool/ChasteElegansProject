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

#ifndef STATECHARTCELLCYCLEMODEL_HPP_
#define STATECHARTCELLCYCLEMODEL_HPP_

#include "AbstractCellCycleModel.hpp"
#include "ChasteSerialization.hpp"
#include "Identifiable.hpp"
#include <boost/serialization/base_object.hpp>
#include "SmartPointers.hpp"

//At the moment, the statechart used as the intracellular logic by this file is changed by altering
//THIS HEADER.
//I need to find an easier way to set the statechart as an input variable to the cell cycle model...
#include "FullStatechart.hpp"


/* A class that allows a boost::statechart to determine the phase of the cell cycle (and other cell properties).
 * This class inherits from AbstractCellCycleModel because CellCycleModels basically cover all intracelular logic type classes
 * in Chaste at the moment. Also this inheritance allows us to easily allow inheritance from a parent cell.
 *
 * Basically, this class works like a normal cell cycle model, with one extra member (a pointer to a statechart)
 * that sets certain variables at each timestep when UpdateCurrentPhase is called.
*/
class StatechartCellCycleModel : public AbstractCellCycleModel
{

private:

    /** Needed for serialization. */
    friend class boost::serialization::access;
    /**
     * Archive the cell-cycle model and member variables.
     *
     * @param archive the archive
     * @param version the current version of this class
     *
     * STATECHART NOT ARCHIVED YET!
     */
    template<class Archive>
    void serialize(Archive & archive, const unsigned int version)
    {
        archive & boost::serialization::base_object<AbstractCellCycleModel>(*this);
        SerializableSingleton<RandomNumberGenerator>* p_wrapper = RandomNumberGenerator::Instance()->GetSerializationWrapper();
        archive & p_wrapper;
    }

public:

     boost::shared_ptr<CellStatechart> pStatechart;

     /*Constructor that:
     * 1) Makes an AbstractCellCycleModel
     * 2) Overwrites certain cell cycle phase durations with values more suitable for C.elegans
     * 3) Leaves the pointer to the statechart NULL
     */
     StatechartCellCycleModel();

     /* Because the cell cycle model doesn't have a pointer to its cell until AFTER construction, this is the method
      * where we make the statechart. We can't do it in the constructor because the statechart's constructor
      * requires a CellPtr.
      */
     void Initialise();

     /* Again, because the cell cycle model doesn't have a pointer to its cell until AFTER construction, this is the method
      * where we make a daughter's statechart. Can't do it any earlier because the statechart's constructor takes a CellPtr.
      */
     void InitialiseDaughterCell();

    /**
     * @return whether the cell is ready to divide (enter M phase).
     */
     bool ReadyToDivide();

    /**
     * This method updates the statechart, and the statechart will in turn update the current phase and
     * set the flag ReadyToDivide if appropriate
     */
     void UpdateCellCyclePhase();


    /**
     * Builder method to create new instances of the cell-cycle model.
     */
     AbstractCellCycleModel* CreateCellCycleModel();

     //Only a slight change to this method from the normal one - we don't need to reset the phase to
     //M after division because the statechart is handling it.
     void ResetForDivision();

     //2 new setter methods - these expose two protected variables to the statechart;
     //otherwise it can't access them and can't set the phase/ReadyToDivide flags
     void SetCellCyclePhase(CellCyclePhase_ Phase);
     void SetReadyToDivide(bool Ready);

     void OutputCellCycleModelParameters(out_stream& rParamsFile);

};

#include "SerializationExportWrapper.hpp"
// Declare identifier for the serializer
CHASTE_CLASS_EXPORT(StatechartCellCycleModel)

#endif  /*STATECHARTCELLCYCLEMODEL_HPP_*/
