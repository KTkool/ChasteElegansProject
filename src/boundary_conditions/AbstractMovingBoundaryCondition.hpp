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

#ifndef ABSTRACTMOVINGBOUNDARYCONDITION_HPP_
#define ABSTRACTMOVINGBOUNDARYCONDITION_HPP_

#include "AbstractCellPopulationBoundaryCondition.hpp"

/* An abstract boundary condition class inheriting from AbstractCellPopulationBoundaryCondition.
 * It is identical to AbstractCellPopulationBoundaryCondition except that it requires derived classes
 * to implement a function UpdateBoundaryCondition, to be called at each timestep, which specifies how
 * the boundary condition changes. See TestRunningMovingBoundarySimulations.hpp for examples.
 * */
template<unsigned ELEMENT_DIM, unsigned SPACE_DIM=ELEMENT_DIM>
class AbstractMovingBoundaryCondition : public AbstractCellPopulationBoundaryCondition<ELEMENT_DIM,SPACE_DIM>
{
	friend class TestCellPopulationBoundaryConditions;

	private:
	    /** Needed for serialization. */
	    friend class boost::serialization::access;
	    /**
	     * Serialize the object and its member variables.
	     *
	     * @param archive the archive
	     * @param version the current version of this class
	     */
	    template<class Archive>
	    void serialize(Archive & archive, const unsigned int version)
	    {
	        // Archiving of mpCellPopulation is implemented in load_construct_data of subclasses
	    }
public:

    /**
     * Constructor.
     *
     * @param pCellPopulation pointer to the cell population.
     */
    AbstractMovingBoundaryCondition(AbstractCellPopulation<ELEMENT_DIM,SPACE_DIM>* pCellPopulation);

    /*
     * New function that all boundary condition files of this type need to define and implement.
     * Called at each timestep by the moving boundary modifier, this will probably alter one of
     * the parameters defining the boundary.
     *
     * */
    virtual void UpdateBoundaryCondition()=0;

};

TEMPLATED_CLASS_IS_ABSTRACT_2_UNSIGNED(AbstractMovingBoundaryCondition)

#endif /*ABSTRACTMOVINGBOUNDARYCONDITION_HPP_*/
