#ifndef FULLSTATECHARTCHASTEINTERFACE_HPP_
#define FULLSTATECHARTCHASTEINTERFACE_HPP_

//Include any Chaste headers you need.
#include <Cell.hpp>
#include <AbstractCellPopulation.hpp>
#include <CellCyclePhases.hpp>
#include <StatechartCellCycleModel.hpp>

//Fill out all the functions that will get and set cell properties.

//Getters
bool IsDead(CellPtr pCell){
	 return pCell->IsDead();
};

//Getters for cell cycle model
double GetMDuration(CellPtr pCell){
	return pCell->GetCellCycleModel()->GetMDuration();
};
double GetSDuration(CellPtr pCell){
	return pCell->GetCellCycleModel()->GetSDuration();
};
double GetG1Duration(CellPtr pCell){
	return pCell->GetCellCycleModel()->GetG1Duration();
};
double GetG2Duration(CellPtr pCell){
	return pCell->GetCellCycleModel()->GetG2Duration();
};

//Setters for cell cycle model
void SetCellCyclePhase(CellPtr pCell, CellCyclePhase_ phase){
	AbstractCellCycleModel* model = pCell->GetCellCycleModel();
	dynamic_cast<StatechartCellCycleModel*>(model)->SetCellCyclePhase(phase);
}
void SetReadyToDivide(CellPtr pCell, bool Ready){
	AbstractCellCycleModel* model = pCell->GetCellCycleModel();
	dynamic_cast<StatechartCellCycleModel*>(model)->SetReadyToDivide(Ready);
}

//Non Standard
void SetProliferationFlag(CellPtr pCell, double Flag){
	pCell->GetCellData()->SetItem("Proliferating",0.0);
};
void   SetRadius(CellPtr pCell, double radius){
	   pCell->GetCellData()->SetItem("Radius",radius);
};
double GetDistanceFromDTC(CellPtr pCell){
	return pCell->GetCellData()->GetItem("DistanceAwayFromDTC");
};
double GetMaxRadius(CellPtr pCell){
	return pCell->GetCellData()->GetItem("MaxRadius");
};

#endif
