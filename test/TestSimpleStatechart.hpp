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

#ifndef TESTSIMPLESTATECHART_HPP_
#define TESTSIMPLESTATECHART_HPP_

#include <cxxtest/TestSuite.h>
#include "CheckpointArchiveTypes.hpp"
#include "AbstractCellBasedTestSuite.hpp"
#include "AbstractCellPopulation.hpp"
#include "PetscSetupAndFinalize.hpp"
#include "CellsGenerator.hpp"
#include "TransitCellProliferativeType.hpp"
#include "HoneycombMeshGenerator.hpp"
#include "OffLatticeSimulation.hpp"
#include "SmartPointers.hpp"
#include "NodesOnlyMesh.hpp"
#include "RepulsionForce.hpp"
#include "NodeBasedCellPopulation.hpp"
#include "DistalArmBoundaryCondition.hpp"
#include "StatechartCellCycleModel.hpp"
#include "PlaneBasedCellKiller.hpp"

class TestSimpleStatechart : public AbstractCellBasedTestSuite
{
public:

void TestStatechart() throw(Exception)
    {
        /** The next line is needed because we cannot currently run node based simulations in parallel. */
        EXIT_IF_PARALLEL;

        //Specify distal arm dimensions
        double cylinderRadius=10;
        double cylinderLength=200;

        /*
         * CREATE CELL POPULATION
         * Seed a few cells at the end of a cylinder representing the distal gonad arm.
         */
        std::vector<Node<3>*> nodes;
        for(double i=0; i<15; i+=5){
        	for(double j=0; j<12; j++){
                	double xcoord= cylinderLength+cylinderRadius-i;
                    Node<3>* my_node;
        			my_node=new Node<3>(12*(i/5)+j,  false,  xcoord, (cylinderRadius-5)*sin(2*3.14159*(j/12)), (cylinderRadius-5)*cos(2*3.14159*(j/12)));
                	nodes.push_back(my_node);
        	}
        }

        //MAKE CELL POPULATION
        NodesOnlyMesh<3>* p_mesh = new NodesOnlyMesh<3>;
        /* Define a cut off length for the connectivity of the nodes by setting a radius of interaction. */
        p_mesh->ConstructNodesWithoutMesh(nodes, 30.0);
        std::vector<CellPtr> cells;

        MAKE_PTR(TransitCellProliferativeType, p_transit_type);
        CellsGenerator<StatechartCellCycleModel, 3> cells_generator;
        cells_generator.GenerateBasicRandom(cells, p_mesh->GetNumNodes(), p_transit_type);
        NodeBasedCellPopulation<3> cell_population(*p_mesh, cells);


    	//loop through cells and initialise cell data properties that will be used in interfacing
        //with the statechart
        cell_population.SetUseVariableRadii(true);
    	for (NodeBasedCellPopulation<3>::Iterator cell_iter = cell_population.Begin();
    	         cell_iter != cell_population.End();
    	         ++cell_iter){

    		 Node<3>* cell_centre_node = cell_population.GetNode(cell_population.GetLocationIndexUsingCell(*cell_iter));
    		 cell_centre_node->SetRadius(2.5);
    		 (*cell_iter)->GetCellData()->SetItem("Radius",2.5);
    		 (*cell_iter)->GetCellData()->SetItem("Proliferating",1.0);
    		 (*cell_iter)->GetCellData()->SetItem("DistanceAwayFromDTC",0.0);

    	}
    	 cell_population.Update();

		//SET SIMULATION PROPERTIES, ADD REPULSION FORCE
        OffLatticeSimulation<3> simulator(cell_population);
        simulator.SetOutputDirectory("StatechartsGeneratedTest");
        simulator.SetSamplingTimestepMultiple(12);
        simulator.SetDt(1.0/300.0);
        simulator.SetEndTime(15.0);
        MAKE_PTR(RepulsionForce<3>, p_force1);
        simulator.AddForce(p_force1);


        //ADD CYLINDRICAL BOUNDARY CONDITION
        c_vector<double,3> left_end;
        left_end[0]=0; left_end[1]=0; left_end[2]=0;
        MAKE_PTR_ARGS(DistalArmBoundaryCondition<3>, p_boundary_condition, (&cell_population,left_end,cylinderLength,cylinderRadius,1e-5));
        simulator.AddCellPopulationBoundaryCondition(p_boundary_condition);

        //ADD A CELL KILLER ON FAR END
        //Plane based, corresponds to egg laying
        c_vector<double,3> pointOnKillerPlane=zero_vector<double>(3);
        c_vector<double,3> normalToKillerPlane=zero_vector<double>(3);
        pointOnKillerPlane[0]=0;
        pointOnKillerPlane[1]=0;
        normalToKillerPlane[0]=-1;
        MAKE_PTR_ARGS(PlaneBasedCellKiller<3>, cell_killer, (&cell_population, pointOnKillerPlane, normalToKillerPlane));
        simulator.AddCellKiller(cell_killer);

        /* RUN */
        simulator.Solve();

        /* GARBAGE COLLECTION*/
        for (unsigned i=0; i<nodes.size(); i++)
        {
            delete nodes[i];
        }
        delete p_mesh;

    };
};

#endif
