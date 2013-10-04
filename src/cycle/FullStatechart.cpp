/* Chaste interface header. Whenever the statechart needs to:
* 1)Provide output to chaste
* 2)Get data about its cell for use in a guard
*  we insert a function of the form "GetMyData(myCellPtr)". That function
*  should then be declared in the interface header, and the user will need to fill in the
*  (usually very brief) details.
*/
#include "FullStatechartChasteInterface.hpp"
#include "FullStatechart.hpp"


/*Implement all the functions declared in simple statechart. These are usually:
* 1) Boilerplate stuff that CellStatechart should be able to do, like copy.
* 2) Responses to events. Often a transition of the form: return transit<NEW_STATE>();
* 3) Constructors or Destructors - a nice place to hook in and get info on the state, since when a state is entered
*	the corresponding object is constructed and when it is exited the destructor is called.
*/


//CellStatechart
CellStatechart::CellStatechart(CellPtr myCell):
		pCell(myCell){
		assert(pCell!=NULL);
//		Activity=0.0;
		initiate();	//calls the constructors of initial states
};

void CellStatechart::SetCell(CellPtr newCell){
	 assert(newCell!=NULL);
	 pCell=newCell;
}

//Copy function, returns a new chart in the same state as the old one, but with a pointer to a new cell
boost::shared_ptr<CellStatechart> CellStatechart::Copy(CellPtr myCell){
	assert(myCell!=NULL);

	MAKE_PTR_ARGS(CellStatechart,myNewStatechart,(myCell));
	if(state_cast<const GLP1Unbound*>()!=0){
			myNewStatechart->process_event(EvGoToGLP1Unbound());
		}
		if(state_cast<const GLP1Bound*>()!=0){
			myNewStatechart->process_event(EvGoToGLP1Bound());
		}
		if(state_cast<const GLP1Inactive*>()!=0){
			myNewStatechart->process_event(EvGoToGLP1Inactive());
		}
		if(state_cast<const GLP1Active*>()!=0){
			myNewStatechart->process_event(EvGoToGLP1Active());
			//myNewStatechart->Activity=(Activity);
		}
		if(state_cast<const GLP1Absent*>()!=0){
			myNewStatechart->process_event(EvGoToGLP1Absent());
		}
		if(state_cast<const LAG1Active*>()!=0){
			myNewStatechart->process_event(EvGoToLAG1Active());
		}
		if(state_cast<const LAG1Inactive*>()!=0){
			myNewStatechart->process_event(EvGoToLAG1Inactive());
		}
		if(state_cast<const GLD1Active*>()!=0){
			myNewStatechart->process_event(EvGoToGLD1Active());
		}
		if(state_cast<const GLD1Inactive*>()!=0){
			myNewStatechart->process_event(EvGoToGLD1Inactive());
		}
		if(state_cast<const M*>()!=0){
			myNewStatechart->process_event(EvGoToM());
		}
		if(state_cast<const S*>()!=0){
			myNewStatechart->process_event(EvGoToS());
		}
		if(state_cast<const G1*>()!=0){
			myNewStatechart->process_event(EvGoToG1());
		}
		if(state_cast<const G2*>()!=0){
			myNewStatechart->process_event(EvGoToG2());
		}
		if(state_cast<const Meiosis*>()!=0){
			myNewStatechart->process_event(EvGoToMeiosis());
		}

	return (myNewStatechart);
};

//--------------------------------------------------------------------
//Propagate the message to update the state to each orthogonal region
//--------------------------------------------------------------------

sc::result Living::react( const EvCheckCellData & ){
	if(IsDead(context<CellStatechart>().pCell)){		//Eclipse complains about this line, but it compiles + runs
		return transit<Dead>();
	};
	post_event(EvGLP1Update());
	post_event(EvGLD1Update());
	post_event(EvLAG1Update());
	post_event(EvMitosisUpdate());
	return discard_event();
};
//------------------------------------------------------------
// Define transitions
//------------------------------------------------------------
sc::result GLP1Unbound::react( const EvGLP1Update & ){
	CellPtr myCell=context<CellStatechart>().pCell;
	if(GetDistanceFromDTC(myCell)<15){
		return transit<GLP1Bound>();
	}
	return discard_event();
};
//------------------------------------------------------------
//------------------------------------------------------------
sc::result GLP1Bound::react( const EvGLP1Update & ){
	return transit<GLP1Inactive>();
};
//------------------------------------------------------------
//------------------------------------------------------------
sc::result GLP1Inactive::react( const EvGLP1Update & ){
	return transit<GLP1Active>();
};
//------------------------------------------------------------
//------------------------------------------------------------
GLP1Active::GLP1Active( my_context ctx ):
	my_base( ctx ){
};
sc::result GLP1Active::react( const EvGLP1Update & ){
	//if(GetDistanceFromDTC(context<CellStatechart>().pCell)<15 && outermost_context().Activity<16){
	//	outermost_context().Activity+=0.1*SimulationTime::Instance()->GetTimeStep();
	//	context<CellStatechart>().pCell->GetCellData()->SetItem("GLP1Activity",outermost_context().Activity);
	//}else if(GetDistanceFromDTC(context<CellStatechart>().pCell)>15){
	//	outermost_context().Activity-=0.5*SimulationTime::Instance()->GetTimeStep();
	//	context<CellStatechart>().pCell->GetCellData()->SetItem("GLP1Activity",outermost_context().Activity);
	//}
	if(GetDistanceFromDTC(context<CellStatechart>().pCell)>110 /*&& context<CellStatechart>().pCell->GetCellData()->GetItem("Mutant")==0*/){
		return transit<GLP1Absent>();
	}
	//if(outermost_context().Activity<0.0){
	//	return transit<GLP1Absent>();
	//}
	return discard_event();
};
//------------------------------------------------------------
//------------------------------------------------------------
sc::result GLP1Absent::react( const EvGLP1Update & ){
	return discard_event();
};
//------------------------------------------------------------
//------------------------------------------------------------
sc::result LAG1Active::react( const EvLAG1Update & ){
	if(state_downcast< const GLP1Active * >()==0 ){
		return transit<LAG1Inactive>();
	}
	return discard_event();
};
//------------------------------------------------------------
//------------------------------------------------------------
sc::result LAG1Inactive::react( const EvLAG1Update & ){
	if(state_downcast< const GLP1Active * >()!=0 ){
		return transit<LAG1Active>();
	}
	return discard_event();
};
//------------------------------------------------------------
//------------------------------------------------------------
sc::result GLD1Inactive::react( const EvGLD1Update & ){

	if(state_downcast< const LAG1Inactive * >()!=0 ){
		return transit<GLD1Active>();
	}
	return discard_event();
};
//------------------------------------------------------------
//------------------------------------------------------------
sc::result GLD1Active::react( const EvGLD1Update & ){
	if(state_downcast< const LAG1Active * >()!=0 ){
		return transit<GLD1Inactive>();
	}
	return discard_event();
};
//------------------------------------------------------------
//------------------------------------------------------------
sc::result Mitosis::react( const EvMitosisUpdate & ){
	return forward_event();
};
//------------------------------------------------------------
// For Meiosis state, action on entry is: initialise radius,
//										  tell chaste the cell is no longer proliferating
//										  update its cell cycle accordingly.
// Thereafter, increment the radius at each timestep.
//------------------------------------------------------------
Meiosis::Meiosis( my_context ctx ):
	my_base( ctx ),
	radius(2.95){
	SetProliferationFlag(context<CellStatechart>().pCell,0.0);
};
sc::result Meiosis::react( const EvMitosisUpdate & ){
	if(radius<(GetMaxRadius(context<CellStatechart>().pCell)-4*SimulationTime::Instance()->GetTimeStep() -0.05 )){
		radius+=4*SimulationTime::Instance()->GetTimeStep();
		SetRadius(context<CellStatechart>().pCell, radius);
	}
	return discard_event();
};
//------------------------------------------------------------
//------------------------------------------------------------

M::M( my_context ctx ):
	my_base( ctx ),
	TimeInThisState(0.0){
	SetCellCyclePhase(context<CellStatechart>().pCell,M_PHASE);
};

sc::result M::react( const EvMitosisUpdate & ){

	/*VariableCellCycle
	double d=GetDistanceFromDTC(context<CellStatechart>().pCell);
	double CellCycleDuration;
	if(d>=0 && d<7){
		 CellCycleDuration = 32-24*d/7.0;
	}else if(d>7 ){
		 CellCycleDuration = 8+24*(d-7.0)/93.0;
	}
	double duration=CellCycleDuration*0.0835;
	*/

	TimeInThisState+=SimulationTime::Instance()->GetTimeStep();
	if(state_downcast<const GLD1Active*>()!=0 && SimulationTime::Instance()->GetTime()>1){
		return transit<Meiosis>();
	}

	if(TimeInThisState>= GetMDuration(context<CellStatechart>().pCell)){
		return transit<G1>();
	}
	return discard_event();
};
sc::result M::react( const EvGoToTime & e){
	TimeInThisState=e.time;
	return discard_event();
};

//------------------------------------------------------------
//------------------------------------------------------------

G1::G1( my_context ctx ):
	my_base( ctx ),
	TimeInThisState(0.0){
	SetCellCyclePhase(context<CellStatechart>().pCell,G_ONE_PHASE);
	RandomNumberGenerator* p_gen = RandomNumberGenerator::Instance();
	myRand=p_gen->NormalRandomDeviate(0.0,0.1*GetG1Duration(context<CellStatechart>().pCell));
};


sc::result G1::react( const EvMitosisUpdate & ){

	/*VariablCellCycle
	double d=GetDistanceFromDTC(context<CellStatechart>().pCell);
	double CellCycleDuration;
	if(d>=0 && d<7){
		 CellCycleDuration = 32-24*d/7.0;
	}else if(d>7 ){
		 CellCycleDuration = 8+24*(d-7.0)/93.0;
	}
	double duration=CellCycleDuration*0.1665;
	*/

	TimeInThisState+=SimulationTime::Instance()->GetTimeStep();
	if(state_downcast<const GLD1Active*>()!=0 && SimulationTime::Instance()->GetTime()>1){
		return transit<Meiosis>();
	}
	if(TimeInThisState>= GetG1Duration(context<CellStatechart>().pCell)+myRand){
		return transit<S>();
	}
	return discard_event();
};
sc::result G1::react( const EvGoToTime & e){
	TimeInThisState=e.time;
	return discard_event();
};


//------------------------------------------------------------
//------------------------------------------------------------

S::S( my_context ctx ):
	my_base( ctx ),
	TimeInThisState(0.0){
	SetCellCyclePhase(context<CellStatechart>().pCell,S_PHASE);
};
sc::result S::react( const EvMitosisUpdate & ){

	/*VariableCellCycle
	double d=GetDistanceFromDTC(context<CellStatechart>().pCell);
	double CellCycleDuration;
	if(d>=0 && d<7){
		 CellCycleDuration = 32-24*d/7.0;
	}else if(d>7){
		 CellCycleDuration = 8+24*(d-7.0)/93.0;
	}
	double duration=CellCycleDuration*0.4165;
	*/

	TimeInThisState+=SimulationTime::Instance()->GetTimeStep();
	if(state_downcast<const GLD1Active*>()!=0 && SimulationTime::Instance()->GetTime()>1){
		return transit<Meiosis>();
	}
	if(TimeInThisState>= GetSDuration(context<CellStatechart>().pCell)){
		return transit<G2>();
	}
	return discard_event();
};
sc::result S::react( const EvGoToTime & e){
	TimeInThisState=e.time;
	return discard_event();
};

//------------------------------------------------------------
//------------------------------------------------------------

G2::G2( my_context ctx ):
	my_base( ctx ),
	TimeInThisState(0.0){
	SetCellCyclePhase(context<CellStatechart>().pCell,G_TWO_PHASE);
	RandomNumberGenerator* p_gen = RandomNumberGenerator::Instance();
	myRand=p_gen->NormalRandomDeviate(0.0,0.1*GetG2Duration(context<CellStatechart>().pCell));
};

sc::result G2::react( const EvMitosisUpdate & ){

	/*VariableCellCycle
	double d=GetDistanceFromDTC(context<CellStatechart>().pCell);
	double CellCycleDuration;
	if(d>=0 && d<7){
		 CellCycleDuration = 32-24*d/7.0;
	}else if(d>7){
		 CellCycleDuration = 8+24*(d-7.0)/93.0;
	}
	double duration=CellCycleDuration*0.3335;
	*/

	TimeInThisState+=SimulationTime::Instance()->GetTimeStep();
	if(state_downcast<const GLD1Active*>()!=0 && SimulationTime::Instance()->GetTime()>1){
		return transit<Meiosis>();
	}
	if(TimeInThisState>= GetG2Duration(context<CellStatechart>().pCell)+myRand){
		SetReadyToDivide(context<CellStatechart>().pCell, true);
		return transit<M>();
	}
	return discard_event();
};

sc::result G2::react( const EvGoToTime & e){
	TimeInThisState=e.time;
	return discard_event();
};

//------------------------------------------------------------
//------------------------------------------------------------

