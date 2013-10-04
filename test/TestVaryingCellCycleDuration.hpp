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

#ifndef TESTELEGANSADULT_HPP_
#define TESTELEGANSADULT_HPP_
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

#include "RandomNumberGenerator.hpp"

/* The next header files define moving boundary conditions - an outer one confining cells to lie inside the gonad arm
 * and a tube boundary condition confining cells to lie outside the syncytium of the distal gonad.
 * The moving boundary modifier keeps a list of moving boundary conditions and can be used to update them
 * at each timestep - in this case it does not because the boundary conditions are at their full size from the start.
 * */
#include "DistalArmBoundaryCondition.hpp"

//Cell cycle model, controlled by a statechart
#include "StatechartCellCycleModel.hpp"
//#include "StatechartCellCycleModelSerializable.hpp"


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

#include "GonadArmPositionTrackerModifier.hpp"

class TestElegansAdult : public AbstractCellBasedTestSuite
{
public:

void TestGonadArmAdult() throw(Exception)
    {
        /** The next line is needed because we cannot currently run node based simulations in parallel. */
        EXIT_IF_PARALLEL;

        /* DEFINE PARAMETERS GOVERNING THE ADULT GONAD ARM.*/
        double InitialLength=400;
        double LengthOfStraightLower=176;
        double LengthOfStraightUpper=161;
        double RadiusOfTurn=20;
        double InitialRadiusOfTube=15;
        double FinalRadiusOfTube=15;
        double GrowthRateLinear=12;
        double GrowthRateRadial=0.3;


        /*
         * CREATE A CELL POPULATION
         * Seed cells in a cylinder all along the distal gonad arm, 12 cells to each circle.
         */

        std::vector<Node<3>*> nodes;
        for(double i=0; i<50; i+=5){
        	for(double j=0; j<12; j++){
                	double xcoord= LengthOfStraightUpper+FinalRadiusOfTube-i;
                    Node<3>* my_node;
        			my_node=new Node<3>(12*(i/5)+j,  false,  xcoord, RadiusOfTurn+(FinalRadiusOfTube-2.5)*sin(2*3.14159*(j/12)), (FinalRadiusOfTube-2.5)*cos(2*3.14159*(j/12)));
                	nodes.push_back(my_node);
        	}
        }


        //MAKE CELL POPULATION
        NodesOnlyMesh<3>* p_mesh = new NodesOnlyMesh<3>;
        /* Define a cut off length for the connectivity of nodes by setting a radius of interaction.
         * Must be greater than 2 times the max cell radius*/

        p_mesh->ConstructNodesWithoutMesh(nodes, 35.0);
        std::vector<CellPtr> cells;
        MAKE_PTR(TransitCellProliferativeType, p_transit_type);
        CellsGenerator<StatechartCellCycleModel, 3> cells_generator;
        cells_generator.GenerateBasicRandom(cells, p_mesh->GetNumNodes(), p_transit_type);
        //make a node based cell population
        NodeBasedCellPopulation<3> cell_population(*p_mesh, cells);


        /*INITIALISE CELL PROPERTIES NEEDED IN MODEL: RADIUS, MAXRADIUS, PROLIFERATION FLAG, DISTANCE FROM DTC*/

        cell_population.SetUseVariableRadii(true);
        for (AbstractCellPopulation<3>::Iterator cell_iter = cell_population.Begin();
				 cell_iter != cell_population.End();
				 ++cell_iter)
			{
			 	 c_vector<double,3> cell_location = cell_population.GetLocationOfCellCentre(*cell_iter);
				 cell_iter->GetCellData()->SetItem("DistanceAwayFromDTC",LengthOfStraightUpper+FinalRadiusOfTube-cell_location[0]);
				 cell_iter->GetCellData()->SetItem("Proliferating",1.0);
				 cell_iter->GetCellData()->SetItem("Radius",2.95);
				 cell_iter->GetCellData()->SetItem("MaxRadius",2.95);
			}
		cell_population.Update();
		cell_population.SetCellAncestorsToLocationIndices();
		cell_population.SetOutputCellAncestors(true);

		//SET SIMULATION PROPERTIES, ADD FORCES

        OffLatticeSimulation<3> simulator(cell_population);
        simulator.SetOutputDirectory("DistalArmVaryingCellCycle");
        simulator.SetSamplingTimestepMultiple(12);
        simulator.SetDt(1.0/250.0);
        simulator.SetEndTime(600);
        MAKE_PTR(RepulsionForceMassCorrected<3>, p_force1);


		//RESCALE SPRING CONSTANTS
        cell_population.SetDampingConstantNormal(0.1);
		cell_population.SetAbsoluteMovementThreshold(20);
		p_force1->SetMeinekeSpringStiffness(1.5);
        simulator.AddForce(p_force1);


        /*BOUNDARY CONDITION*/
        MAKE_PTR_ARGS(DistalArmBoundaryCondition<3>, p_boundary_condition, (&cell_population,LengthOfStraightLower,LengthOfStraightUpper,RadiusOfTurn,InitialRadiusOfTube,1e-5));
        simulator.AddCellPopulationBoundaryCondition(p_boundary_condition);


        //ADD TWO CELL KILLERS
        //Stochastic in loop, narrows the field by apoptosis as the eggs grow

        c_vector<double,3> top_right_killing_region=zero_vector<double>(3);
        c_vector<double,3> bottom_left_killing_region=zero_vector<double>(3);
        top_right_killing_region[0]=0;
        top_right_killing_region[1]=RadiusOfTurn;
        top_right_killing_region[2]=FinalRadiusOfTube;
        bottom_left_killing_region[0]=-RadiusOfTurn;
        bottom_left_killing_region[1]=-RadiusOfTurn;
        bottom_left_killing_region[2]=-FinalRadiusOfTube;
        double probability_of_death_hour=0.5;
        MAKE_PTR_ARGS(RandomCellKillerInCuboid<3>, cell_killer1, (&cell_population, probability_of_death_hour, top_right_killing_region, bottom_left_killing_region));
        simulator.AddCellKiller(cell_killer1);

        //Plane based, corresponds to egg laying

        c_vector<double,3> pointOnKillerPlane=zero_vector<double>(3);
        c_vector<double,3> normalToKillerPlane=zero_vector<double>(3);
        pointOnKillerPlane[0]=LengthOfStraightLower;
        pointOnKillerPlane[1]=-RadiusOfTurn;
        normalToKillerPlane[0]=1;
        MAKE_PTR_ARGS(TimedPlaneBasedCellKiller<3>, cell_killer2, (&cell_population, pointOnKillerPlane, normalToKillerPlane,0.0));
        simulator.AddCellKiller(cell_killer2);

        MAKE_PTR(GonadArmPositionTrackerModifier<3>,mod);
        simulator.AddSimulationModifier(mod);

        /* RUN + SAVE*/
        simulator.Solve();
        CellBasedSimulationArchiver<3, OffLatticeSimulation<3> >::Save(&simulator);

        /* GARBAGE COLLECTION*/
        for (unsigned i=0; i<nodes.size(); i++)
        {
            delete nodes[i];
        }
        delete p_mesh;

        /*TEST LOAD*/
        //OffLatticeSimulation<3>* p_simulator
        //    = CellBasedSimulationArchiver<3, OffLatticeSimulation<3> >::Load("GonadArmAdultRunDeath095", 70.0);
        //p_simulator->SetEndTime(100.0);
        //p_simulator->Solve();

        //CellBasedSimulationArchiver<3, OffLatticeSimulation<3> >::Save(p_simulator);
        //delete p_simulator;
    }
};

#endif /* TESTELEGANSADULT_HPP_ */
