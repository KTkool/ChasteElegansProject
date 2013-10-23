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

/*Tests germ cell growth and division constrained to an adult C elegans gonad arm*/

//Standard headers
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
#include "NodeBasedCellPopulation.hpp"
#include <string.h>

/* The next header file defines 2 static boundary conditions - an outer one confining cells to lie inside the 
 * gonad arm and an inner one confining cells to lie outside the syncytium of the distal gonad.
 * */
#include "CombinedStaticGonadBoundaryCondition.hpp"

//Defines a cell cycle model that is controlled by a statechart and can archive the state.
//NOTE: MY LOCAL COPY OF THE TRUNK HAS ONE ALTERED FILE TO MAKE THESE WORK:
// Comment out the box ownership test, in the function AddNodeWithFixedIndex of NodesOnlyMesh
// Otherwise a segfault occurs on loading from archive, because the boxcollection is null.
#include "StatechartCellCycleModelSerializable.hpp"

//Specialised cell killers. These output by default a text file containing the time of death 
//and id of each cell killed.
//1) Randomly kills a certain number of cells in the loop region of the gonad each timestep
#include "RandomCellKillerInCuboid.hpp"
//2) Removes eggs as they are laid
#include "TimedPlaneBasedCellKiller.hpp"

//Buske force law headers. NOTE: MY LOCAL COPY OF THE TRUNK HAS TWO ALTERED FILES TO MAKE THESE WORK:
//1) IN LinearSystem.cpp: Hard code a row preallocation of 30.
//2) IN BuskeCompressionForce.cpp: Set target volume = current cell volume.
// It would also help if you:
//3) Set buske force law constants to the values given in my project report.
#include "BuskeCompressionForce.hpp"
#include "BuskeAdhesiveForce.hpp"
#include "BuskeElasticForce.hpp"
#include "NodeBasedCellPopulationWithBuskeUpdate.hpp"

//Repulsion force law header. This version has drag scale linearly with cell radius
#include "RepulsionForceMassCorrected.hpp"

//A modifier outputting the positions of all cells on an hourly basis 
#include "GonadArmPositionTrackerModifier.hpp"


class TestElegansAdult : public AbstractCellBasedTestSuite
{
public:

void TestGonadArmAdult() throw(Exception)
    {
        /** The next line is needed because we cannot currently run node based simulations in parallel. */
        EXIT_IF_PARALLEL;

        /* 0) SET TEST OPTIONS*/
        bool isBuske = false;                   //if false, defaults to repulsion force law.
        bool trackAncestors = true;
        bool useGLP1ActivityVariable = true;   //if false, defaults to base statechart model from report.
        bool seedSingleMutant = false;
        std::string outputDirectory = "TEST_REFACTORED_CODE";
        double endTime = 5.0;

        /* 1) DEFINE PARAMETERS GOVERNING THE ADULT GONAD ARM.*/
        double Length=400;
        double LengthOfStraightLower=176;
        double LengthOfStraightUpper=161;
        double RadiusOfTurn=20;
        double RadiusOfTube=15;


        /*
         * 2) CREATE A SET OF NODES
         * Seed cells in a cylinder all along the distal gonad arm, with 12 cells to each circle.
         */
        std::vector<Node<3>*> nodes;
        for(double i=0; i<50; i+=5){
            double noInCircle=12;
        	for(double j=0; j<noInCircle; j++){
                	double xcoord= LengthOfStraightUpper+RadiusOfTube-i;
                    Node<3>* my_node;
        			my_node=new Node<3>(noInCircle*(i/5)+j,  false,  xcoord, RadiusOfTurn+(RadiusOfTube-2.95)*sin(2*M_PI*(j/noInCircle)), (RadiusOfTube-2.95)*cos(2*M_PI*(j/noInCircle)));
                	nodes.push_back(my_node);
        	}
        }

        
        //3) MAKE A CELL POPULATION
        NodesOnlyMesh<3>* p_mesh = new NodesOnlyMesh<3>;
        /* Define a cut off length for the connectivity of nodes by setting a radius of interaction.
         * Must be greater than 2 times the max cell radius*/
        p_mesh->ConstructNodesWithoutMesh(nodes, 35.0);
        std::vector<CellPtr> cells;
        MAKE_PTR(TransitCellProliferativeType, p_transit_type);

        //DEAL WITH STATECHART MODEL CHOICE
        CellsGenerator<StatechartCellCycleModelSerializable, 3> cells_generator;
        
        //finish making a node based cell population
        cells_generator.GenerateBasicRandom(cells, p_mesh->GetNumNodes(), p_transit_type);
        
        NodeBasedCellPopulation<3> *cell_population=NULL;
        if(isBuske==true){
            cell_population=new NodeBasedCellPopulationWithBuskeUpdate<3>(*p_mesh, cells);
            cell_population->SetDampingConstantNormal(1.39);
        }else{
            cell_population=new NodeBasedCellPopulation<3>(*p_mesh, cells);
            cell_population->SetDampingConstantNormal(0.1);
            cell_population->SetAbsoluteMovementThreshold(10);
        }
        

        /*4) INITIALISE CELL DATA ITEMS REQUIRED BY THE STATECHART MODEL: 
             RADIUS, MAXRADIUS, PROLIFERATION FLAGS, DISTANCE FROM DTC*/ 
        //Going to use cells of differing radii
        cell_population->SetUseVariableRadii(true);
        for (AbstractCellPopulation<3>::Iterator cell_iter = cell_population->Begin();
			cell_iter != cell_population->End(); ++cell_iter)
			{
			 	c_vector<double,3> cell_location = cell_population->GetLocationOfCellCentre(*cell_iter);
				cell_iter->GetCellData()->SetItem("DistanceAwayFromDTC",LengthOfStraightUpper+RadiusOfTube-cell_location[0]);
				cell_iter->GetCellData()->SetItem("Proliferating",1.0);
				cell_iter->GetCellData()->SetItem("Radius",2.95);
				cell_iter->GetCellData()->SetItem("MaxRadius",2.95);
                
                if(useGLP1ActivityVariable==true){
                    cell_iter->GetCellData()->SetItem("GLP1Activity",0.0);
                }
                
                if(seedSingleMutant==true){
                    if(cell_iter->GetCellId()!=0){
                        cell_iter->GetCellData()->SetItem("Mutant",0.0);
                    }else{
                        cell_iter->GetCellData()->SetItem("Mutant",1.0);
                    }
                }
			}
		cell_population->Update();
        //Setup lineage tracking
        if(trackAncestors==true){
            cell_population->SetCellAncestorsToLocationIndices();
            cell_population->SetOutputCellAncestors(true);
        }



		//5) SET SIMULATION PROPERTIES AND ADD FORCES
        OffLatticeSimulation<3> simulator(*cell_population);
        simulator.SetOutputDirectory(outputDirectory);
        simulator.SetSamplingTimestepMultiple(12);
        simulator.SetDt(1.0/250.0);
        simulator.SetEndTime(endTime);

        if(isBuske==false){
            MAKE_PTR(RepulsionForceMassCorrected<3>, p_force1);
            p_force1->SetMeinekeSpringStiffness(1.5);
            simulator.AddForce(p_force1);
        }else{
            MAKE_PTR(BuskeCompressionForce<3>, p_force1);
            MAKE_PTR(BuskeAdhesiveForce<3>, p_force2);
            MAKE_PTR(BuskeElasticForce<3>, p_force3);
            simulator.AddForce(p_force1);
            simulator.AddForce(p_force2);
            simulator.AddForce(p_force3);
        }


        /*6) ADD BOUNDARY CONDITION*/
        MAKE_PTR_ARGS(CombinedStaticGonadBoundaryCondition<3>, p_boundary_condition, (cell_population,LengthOfStraightLower,LengthOfStraightUpper,RadiusOfTurn,RadiusOfTube,1e-5));
        simulator.AddCellPopulationBoundaryCondition(p_boundary_condition);


        //7) ADD TWO CELL KILLERS
        //Stochastic in loop, narrows the field by apoptosis as the eggs grow
        c_vector<double,3> top_right_killing_region=zero_vector<double>(3);
        c_vector<double,3> bottom_left_killing_region=zero_vector<double>(3);
        top_right_killing_region[0]=0;
        top_right_killing_region[1]=RadiusOfTurn;
        top_right_killing_region[2]=RadiusOfTube;
        bottom_left_killing_region[0]=-RadiusOfTurn;
        bottom_left_killing_region[1]=-RadiusOfTurn;
        bottom_left_killing_region[2]=-RadiusOfTube;
        double probability_of_death_hour=0.5;
        MAKE_PTR_ARGS(RandomCellKillerInCuboid<3>, cell_killer1, (cell_population, probability_of_death_hour, top_right_killing_region, bottom_left_killing_region));
        simulator.AddCellKiller(cell_killer1);
        //Plane based, corresponds to egg laying
        c_vector<double,3> pointOnKillerPlane=zero_vector<double>(3);
        c_vector<double,3> normalToKillerPlane=zero_vector<double>(3);
        pointOnKillerPlane[0]=LengthOfStraightLower;
        pointOnKillerPlane[1]=-RadiusOfTurn;
        normalToKillerPlane[0]=1;
        MAKE_PTR_ARGS(TimedPlaneBasedCellKiller<3>, cell_killer2, (cell_population, pointOnKillerPlane, normalToKillerPlane,0.0));
        simulator.AddCellKiller(cell_killer2);


        //add output modifier...
        MAKE_PTR(GonadArmPositionTrackerModifier<3>, mod1);
        simulator.AddSimulationModifier(mod1);


        /*8) RUN AND SAVE*/
        simulator.Solve();
        CellBasedSimulationArchiver<3, OffLatticeSimulation<3> >::Save(&simulator);

        
        /*9) GARBAGE COLLECTION*/
        for (unsigned i=0; i<nodes.size(); i++)
        {
            delete nodes[i];
        }
        delete p_mesh;
        delete cell_population;
    }
};

#endif /* TESTELEGANSADULT_HPP_ */
