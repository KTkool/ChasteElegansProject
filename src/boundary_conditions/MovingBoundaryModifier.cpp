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

#include "MovingBoundaryModifier.hpp"
#include "MeshBasedCellPopulation.hpp"

template<unsigned DIM>
MovingBoundaryModifier<DIM>::MovingBoundaryModifier()
    : AbstractCellBasedSimulationModifier<DIM>()
{
}

template<unsigned DIM>
MovingBoundaryModifier<DIM>::~MovingBoundaryModifier()
{
}

//At each timestep, loop through all moving boundary conditions the modifier knows about. For each one,
//call the UpdateBoundaryCondition() method.
template<unsigned DIM>
void MovingBoundaryModifier<DIM>::UpdateAtEndOfTimeStep(AbstractCellPopulation<DIM,DIM>& rCellPopulation)
{
    for (typename std::vector<boost::shared_ptr<AbstractMovingBoundaryCondition<DIM,DIM> > >::iterator bcon_iter = mBoundaryConditions.begin();
    		bcon_iter != mBoundaryConditions.end();
         ++bcon_iter)
    {
        (*bcon_iter)->UpdateBoundaryCondition();
    }
    UpdateCellData(rCellPopulation);

}

template<unsigned DIM>
void MovingBoundaryModifier<DIM>::SetupSolve(AbstractCellPopulation<DIM,DIM>& rCellPopulation, std::string outputDirectory)
{
    /*
     * We must update CellData in SetupSolve(), otherwise it will not have been
     * fully initialised by the time we enter the main time loop.
     */
    UpdateCellData(rCellPopulation);
}


template<unsigned DIM>
void MovingBoundaryModifier<DIM>::UpdateCellData(AbstractCellPopulation<DIM,DIM>& rCellPopulation)
{
    // Make sure the cell population is updated
    rCellPopulation.Update();
}


//Inform the modifier about a new moving boundary condition using this method and a pointer to the boundary condition
template<unsigned DIM>
void MovingBoundaryModifier<DIM>::SpecifyMovingBoundary(boost::shared_ptr<AbstractMovingBoundaryCondition<DIM,DIM> >  pBoundaryCondition){
	mBoundaryConditions.push_back(pBoundaryCondition);
};


/////////////////////////////////////////////////////////////////////////////
// Explicit instantiation
/////////////////////////////////////////////////////////////////////////////

template class MovingBoundaryModifier<1>;
template class MovingBoundaryModifier<2>;
template class MovingBoundaryModifier<3>;

// Serialization for Boost >= 1.36
#include "SerializationExportWrapperForCpp.hpp"
EXPORT_TEMPLATE_CLASS_SAME_DIMS(MovingBoundaryModifier)

