#ifndef StatechartINTERFACE_HPP_
#define StatechartINTERFACE_HPP_

//Include any Chaste headers you need.

#include <Cell.hpp>
#include <AbstractCellPopulation.hpp>
#include <CellCyclePhases.hpp>
#include <StatechartCellCycleModelSerializable.hpp>

//Fill out all the functions that will get and set cell properties. Some common functions provided.

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
    dynamic_cast<StatechartCellCycleModelSerializable*>(model)->SetCellCyclePhase(phase);
}

void SetReadyToDivide(CellPtr pCell, bool Ready){
    AbstractCellCycleModel* model = pCell->GetCellCycleModel();
    dynamic_cast<StatechartCellCycleModelSerializable*>(model)->SetReadyToDivide(Ready);
};

//Misc
bool IsDead(CellPtr pCell){
     return pCell->IsDead();
};
double GetTimestep(){
     return SimulationTime::Instance()->GetTimeStep();
};
double GetTime(){
     return SimulationTime::Instance()->GetTime();
};

//Elegans Specific
void SetProliferationFlag(CellPtr pCell, double Flag){
    pCell->GetCellData()->SetItem("Proliferating",0.0);
};
void SetRadius(CellPtr pCell, double radius){
      pCell->GetCellData()->SetItem("Radius",radius);
};
double GetDistanceFromDTC(CellPtr pCell){
    return pCell->GetCellData()->GetItem("DistanceAwayFromDTC");
};
double GetMaxRadius(CellPtr pCell){
    return pCell->GetCellData()->GetItem("MaxRadius");
};

void UpdateRadius(CellPtr pCell){
  double MaxRad = GetMaxRadius(pCell);
  double Rad = pCell->GetCellData()->GetItem("Radius");
  if(Rad<MaxRad-0.1){
    SetRadius(pCell,Rad+=GetTimestep());
  }
};
#endif
