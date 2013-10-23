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

#ifndef TESTCONTINUATION_HPP_
#define TESTCONTINUATION_HPP_
/*Tests cell growth and division constrained to a growing C elegans gonad arm; this time with more accurate
 *length and time parameters (in microns and hours respectively)*/

#include <cxxtest/TestSuite.h>
#include "CellBasedSimulationArchiver.hpp"

#include "CheckpointArchiveTypes.hpp"
#include "AbstractCellBasedTestSuite.hpp"
#include "PetscSetupAndFinalize.hpp"
#include "CellsGenerator.hpp"
#include "TransitCellProliferativeType.hpp"
#include "HoneycombMeshGenerator.hpp"
#include "OffLatticeSimulation.hpp"
#include "SmartPointers.hpp"
#include "NodesOnlyMesh.hpp"

/* The next header files define moving boundary conditions - an outer one confining cells to lie inside the gonad arm
 * and a tube boundary condition confining cells to lie outside the syncytium of the distal gonad.
 * The moving boundary modifier keeps a list of moving boundary conditions and can be used to update them
 * at each timestep - in this case it does not because the boundary conditions are at their full size from the start.
 * */
#include "CombinedStaticGonadBoundaryCondition.hpp"

//Cell cycle model, controlled by a statechart
#include "StatechartCellCycleModelSerializable.hpp"


//Kills certain number of cells in the loop region as they apoptose
#include "RandomCellKillerInCuboid.hpp"
//removes eggs as they are laid
#include "TimedPlaneBasedCellKiller.hpp"

//Buske forces. NOTE: MY LOCAL COPY OF THE TRUNK HAS TWO ALTERED FILES TO MAKE THESE WORK:
//1) IN LinearSystem.cpp: Hard code a row preallocation of 30.
//2) IN BuskeCompressionForce.cpp: Set target volume = current cell volume.
#include "NodeBasedCellPopulation.hpp"
#include "RepulsionForce.hpp"
#include "RepulsionForceMassCorrected.hpp"

#include "BuskeCompressionForce.hpp"
#include "BuskeAdhesiveForce.hpp"
#include "BuskeElasticForce.hpp"

#include "EggLayingForce.hpp"


class TestContinuation : public AbstractCellBasedTestSuite
{
public:

  void Testcontinuation() throw(Exception)
  {
    /** The next line is needed because we cannot currently run node based simulations in parallel. */
    EXIT_IF_PARALLEL;

    /*TEST LOAD*/
    OffLatticeSimulation<3>* p_simulator
        = CellBasedSimulationArchiver<3, OffLatticeSimulation<3> >::Load("TEST_REFACTORED_CODE", 5.0);
  
    p_simulator->SetEndTime(10.0);
    p_simulator->Solve();

    delete p_simulator;
  }

};

#endif /* TESTCONTINUATION_HPP_ */
