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
#include "CheckpointArchiveTypes.hpp"
#include "AbstractCellBasedTestSuite.hpp"
#include "PetscSetupAndFinalize.hpp"
#include "CellsGenerator.hpp"
#include "TransitCellProliferativeType.hpp"
#include "HoneycombMeshGenerator.hpp"
#include "OffLatticeSimulation.hpp"
#include "SmartPointers.hpp"
#include "NodesOnlyMesh.hpp"
#include "NodeBasedCellPopulation.hpp"
/* The next header files define moving boundary conditions - an outer one confining cells to lie inside the gonad arm
 * and a cylindrical boundary condition confining cells to lie outside the syncytium of the distal gonad.
 * The moving boundary modifier keeps a list of moving boundary conditions and can be used to update them
 * at each timestep - in this case it does not because the boundary conditions are at their full size from the start.
 * */
#include "GonadArmForcedBoundaryCondition.hpp"
//Cell cycle model, with G1 duration dependent on distance from the DTC
#include "ElegansStochasticCellCycleModel.hpp"

#include "RepulsionForceMassCorrected.hpp"

#include "TimedPlaneBasedCellKiller.hpp"


class TestGrowingGonad : public AbstractCellBasedTestSuite
{
public:

void TestGonadArmGrowing() throw(Exception)
    {
        /** The next line is needed because we cannot currently run node based simulations in parallel. */
        EXIT_IF_PARALLEL;


        /* DEFINE PARAMETERS GOVERNING THE ADULT GONAD ARM.*/
        double InitialLength=40;
        double LengthOfStraightLower=176;
        double LengthOfStraightUpper=161;
        double RadiusOfTurn=20;
        double InitialRadiusOfTube=15;
        double FinalRadiusOfTube=15;
        double GrowthRateLinear=10;
        double GrowthRateRadial=0;
        double LengthOfProliferativeZone=100;


        /*
         * CREATE CELL POPULATION
         * Seed cells in a cylinder all along the proximal end of the gonad arm, 8 cells to each circle.
         */
        std::vector<Node<3>*> nodes;
        for(double i=1; i<40; i+=5){
        	for(double j=0; j<8; j++){
                	double xcoord= LengthOfStraightLower+InitialRadiusOfTube-i;
                    Node<3>* my_node;
        			my_node=new Node<3>(8*(i/5)+j,  false,  xcoord,-RadiusOfTurn+(InitialRadiusOfTube-2.5)*sin(2*3.14159*(j/8)), (InitialRadiusOfTube-2.5)*cos(2*3.14159*(j/8)));
                	nodes.push_back(my_node);
        	}
        }


        //MAKE CELL POPULATION
        NodesOnlyMesh<3>* p_mesh = new NodesOnlyMesh<3>;
        /* Define a cut off length for the connectivity of the nodes by setting a radius of interaction. */
        p_mesh->ConstructNodesWithoutMesh(nodes, 10);
        std::vector<CellPtr> cells;
        MAKE_PTR(TransitCellProliferativeType, p_transit_type);
        CellsGenerator<ElegansStochasticCellCycleModel, 3> cells_generator;
        cells_generator.GenerateBasicRandom(cells, p_mesh->GetNumNodes(), p_transit_type);
        NodeBasedCellPopulation<3> cell_population(*p_mesh, cells);


        /*INITIALISE CELL PROPERTIES NEEDED IN MODEL: RADIUS*/
		for (AbstractCellPopulation<3>::Iterator cell_iter = cell_population.Begin();
				 cell_iter != cell_population.End();
				 ++cell_iter)
			{
			 c_vector<double,3> cell_location = cell_population.GetLocationOfCellCentre(*cell_iter);
			 cell_iter->GetCellData()->SetItem("Radius",2.5);
			 Node<3>* cell_centre_node = cell_population.GetNode(cell_population.GetLocationIndexUsingCell(*cell_iter));
		     cell_centre_node->SetRadius(2.5); // needs to be set after the mesh is created or Nodes Only Mesh will reset it to 0.5
			}
		cell_population.Update();


		//SET SIMULATION PROPERTIES, ADD REPULSION FORCE
        OffLatticeSimulation<3> simulator(cell_population);
        simulator.SetOutputDirectory("GonadArmGrowing");
        simulator.SetSamplingTimestepMultiple(12);
        simulator.SetDt(1.0/200.0);
        simulator.SetEndTime(20);
        MAKE_PTR(RepulsionForceMassCorrected<3>, p_force);
        simulator.AddForce(p_force);


        /*MOVING BOUNDARY CONDITIONS*/
        double endcapDampingConstant=15;
        MAKE_PTR_ARGS(GonadArmForcedBoundaryCondition<3>, p_boundary_condition, (&cell_population,InitialLength,LengthOfStraightLower,LengthOfStraightUpper,RadiusOfTurn,InitialRadiusOfTube,FinalRadiusOfTube,GrowthRateLinear,GrowthRateRadial,endcapDampingConstant));
        simulator.AddCellPopulationBoundaryCondition(p_boundary_condition);


        /*CELL KILLERS*/
        //Plane based, corresponds to egg laying
        c_vector<double,3> pointOnKillerPlane=zero_vector<double>(3);
        c_vector<double,3> normalToKillerPlane=zero_vector<double>(3);
        pointOnKillerPlane[0]=LengthOfStraightLower;
        pointOnKillerPlane[1]=-RadiusOfTurn;
        double StartTime=24;
        normalToKillerPlane[0]=1;
        MAKE_PTR_ARGS(TimedPlaneBasedCellKiller<3>, cell_killer2, (&cell_population, pointOnKillerPlane, normalToKillerPlane,StartTime));
        simulator.AddCellKiller(cell_killer2);



        /* RUN */
        simulator.Solve();


        /* GARBAGE COLLECTION*/
        for (unsigned i=0; i<nodes.size(); i++)
        {
            delete nodes[i];
        }
        delete p_mesh;
    }
};

#endif /* TESTELEGANSADULT_HPP_ */
