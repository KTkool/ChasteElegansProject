#include "StatechartChasteInterfaceSerializable.hpp"
#include "ExampleStatechartSerializable.hpp"


/*Implements all the functions declared in the statechart header. These are typically:
* 1) Boilerplate stuff that ensures the statechart can be copied, archived, hold variables etc.
* 2) Response functions for events. Often a change of state: return transit<NEW_STATE>();
* 3) Constructors or Destructors - a nice place to hook in and give information on the state to Chaste, 
* since when a state is entered the corresponding constructor is called and when it is exited the destructor is called.
*/

//------------------------------------------------------------
//-------------------------Statechart proper------------------
//------------------------------------------------------------

CellStatechart::CellStatechart(){
	//Initially the cell pointer is null. It will be set by the SetCell function of cell cycle model 
	pCell=boost::shared_ptr<Cell>();
	//List any variables the model requires to hold non state information e.g. to implement a delay
	//TimeInPhase is required in this category. Give a default value.
	TimeInPhase=0;
	//Constructs rest of chart with default initial state
};

//Setter method for cell pointer
void CellStatechart::SetCell(CellPtr newCell){
	assert(newCell!=NULL);
	pCell=newCell;
	initiate();	
}

//For archiving, write a method that packs up all variables in an array and returns the array. 
std::vector<double> CellStatechart::GetVariables(){
	std::vector<double> variables;
	variables.push_back(TimeInPhase);
	return variables;
}

//For archiving, write a method that takes an array and unpacks values for all variables.
void CellStatechart::SetVariables(std::vector<double> variables){
	TimeInPhase=variables.at(1);
}

//2 functions allowing the encoding and decoding of the cell state (for archiving purposes only).
//These can safely be slow, because they are rarely called. Currently encode state as a product of primes
//then on load, check divisibility and force transition into the appropriate state.
uint CellStatechart::GetState(){
	uint state=1;
	if(state_cast<const GLP1Unbound*>()!=0){
		state=state*2;
	}
	if(state_cast<const GLP1Bound*>()!=0){
		state=state*3;
	}
	if(state_cast<const GLP1Inactive*>()!=0){
		state=state*5;
	}
	if(state_cast<const GLP1Active*>()!=0){
		state=state*7;
	}
	if(state_cast<const GLP1Absent*>()!=0){
		state=state*11;
	}
	if(state_cast<const LAG1Active*>()!=0){
		state=state*13;
	}
	if(state_cast<const LAG1Inactive*>()!=0){
		state=state*17;
	}
	if(state_cast<const GLD1Active*>()!=0){
		state=state*19;
	}
	if(state_cast<const GLD1Inactive*>()!=0){
		state=state*23;
	}
	if(state_cast<const M*>()!=0){
		state=state*29;
	}
	if(state_cast<const S*>()!=0){
		state=state*31;
	}
	if(state_cast<const G1*>()!=0){
		state=state*37;
	}
	if(state_cast<const G2*>()!=0){
		state=state*41;
	}
	if(state_cast<const Meiosis*>()!=0){
		state=state*43;
	}
	return state;
}
void CellStatechart::SetState(uint state){
	if(state%2==0){
		process_event(EvGoToGLP1Unbound());
	}
	if(state%3==0){
		process_event(EvGoToGLP1Bound());
	}
	if(state%5==0){
		process_event(EvGoToGLP1Inactive());
	}
	if(state%7==0){
		process_event(EvGoToGLP1Active());
	}
	if(state%11==0){
		process_event(EvGoToGLP1Absent());
	}
	if(state%13==0){
		process_event(EvGoToLAG1Active());
	}
	if(state%17==0){
		process_event(EvGoToLAG1Inactive());
	}
	if(state%19==0){
		process_event(EvGoToGLD1Active());
	}
	if(state%23==0){
		process_event(EvGoToGLD1Inactive());
	}
	if(state%29==0){
		process_event(EvGoToM());
	}
	if(state%31==0){
		process_event(EvGoToS());
	}
	if(state%37==0){
		process_event(EvGoToG1());
	}
	if(state%41==0){
		process_event(EvGoToG2());
	}
	if(state%43==0){
		process_event(EvGoToMeiosis());
	}
}

//Copy function; returns a new chart in the same state as the old one, and with the cell pointer unset
//Cell pointer will be set by SetCell. 
boost::shared_ptr<CellStatechart> CellStatechart::Copy(){
	MAKE_PTR(CellStatechart,myNewStatechart);
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
//----------------------Stuff to do with transitions------------------
//--------------------------------------------------------------------

//A first responder state. This receives the instruction to update from the cell cycle model,
//then propagates an update instruction to each orthogonal region of the chart.  
sc::result Living::react( const EvCheckCellData & ){
	if(IsDead(context<CellStatechart>().pCell)){		
		return transit<Dead>();
	};
	post_event(EvGLP1Update());
	post_event(EvGLD1Update());
	post_event(EvLAG1Update());
	post_event(EvMitosisUpdate());
	return discard_event();
};


// Describe all the possible transitions

//------------------------------------------------------------
//-------------------Container/Outer states-------------------
//------------------------------------------------------------
sc::result Mitosis::react( const EvMitosisUpdate & ){

	if(state_downcast<const GLD1Active*>()!=0 && SimulationTime::Instance()->GetTime()>1){
		return transit<Meiosis>();
	}
	else{
		return discard_event();
	}
}

//------------------------------------------------------------
//-------------------Leaf/Innermost states--------------------
//------------------------------------------------------------
//Example:
//The state GLP1Unbound should react to the update event for its orthogonal region
sc::result GLP1Unbound::react( const EvGLP1Update & ){
	//It reacts by looking at its cell
	CellPtr myCell=context<CellStatechart>().pCell;
	//If that cell is less than 
	if(GetDistanceFromDTC(myCell)<15){
		return transit<GLP1Bound>();
	}
	//Otherwise, forward the update event so that other states in the same region get a chance to respond.
	//Forwarding should continue until you reach the outermost state in the region, at which point the event
	//should be discarded
	return forward_event();
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
sc::result GLP1Active::react( const EvGLP1Update & ){
	CellPtr myCell=context<CellStatechart>().pCell;
	if(GetDistanceFromDTC(myCell)>100){
		return transit<GLP1Absent>();
	}
	return forward_event();
};
//------------------------------------------------------------
//------------------------------------------------------------
sc::result GLP1Absent::react( const EvGLP1Update & ){
	return forward_event();
};
//------------------------------------------------------------
//------------------------------------------------------------
sc::result LAG1Active::react( const EvLAG1Update & ){
	//This is an example of a condition on a state of the chart rather than cell data
	//It reads, if NOT in the GLP1Active state, then:
	if(state_downcast< const GLP1Active * >()==0 ){
		return transit<LAG1Inactive>();
	}
	return forward_event();
};
//------------------------------------------------------------
//------------------------------------------------------------
sc::result LAG1Inactive::react( const EvLAG1Update & ){
	if(state_downcast< const GLP1Active * >()!=0 ){
		return transit<LAG1Active>();
	}
	return forward_event();
};
//------------------------------------------------------------
//------------------------------------------------------------
sc::result GLD1Inactive::react( const EvGLD1Update & ){
	if(state_downcast< const LAG1Inactive * >()!=0 ){
		return transit<GLD1Active>();
	}
	return forward_event();
};
//------------------------------------------------------------
//------------------------------------------------------------
sc::result GLD1Active::react( const EvGLD1Update & ){
	if(state_downcast< const LAG1Active * >()!=0 ){
		return transit<GLD1Inactive>();
	}
	return forward_event();
};

//------------------------------------------------------------
//------------------------------------------------------------

//Meiosis performs an action on entry, setting an output flag so we can visualise the proliferation state
//in paraview. States with a non standard constructor must inherit from state rather than simple_state, and
//and must include the my_contex ctx and my_base(ctx) sections (always the same) 
Meiosis::Meiosis( my_context ctx ):
	my_base( ctx ){
	CellPtr myCell=context<CellStatechart>().pCell;
	radius=GetRadius(myCell);
	SetProliferationFlag(myCell,0.0);
};

sc::result Meiosis::react( const EvMitosisUpdate & ){
	CellPtr myCell = context<CellStatechart>().pCell;
	if(radius<(GetMaxRadius(myCell)-SimulationTime::Instance()->GetTimeStep())){
		radius+=SimulationTime::Instance()->GetTimeStep();
		SetRadius(myCell, radius);
	}
	return forward_event();
};
//------------------------------------------------------------
//------------------------------------------------------------
M::M( my_context ctx ):
	my_base( ctx ){
	//On entry, set the TimeInPhase variable of CellStatechart to 0.0
	context<CellStatechart>().TimeInPhase = 0.0;
	//Generate a normally distributed duration for this phase about some mean
	CellPtr myCell=context<CellStatechart>().pCell;
	RandomNumberGenerator* p_gen = RandomNumberGenerator::Instance();
	Duration=p_gen->NormalRandomDeviate(GetMDuration(myCell),0.1*GetMDuration(myCell));
	//Communicate the change of phase to the cell cycle model
	SetCellCyclePhase(myCell,M_PHASE);
};
sc::result M::react( const EvMitosisUpdate & ){
	context<CellStatechart>().TimeInPhase+=SimulationTime::Instance()->GetTimeStep();
	//If cell has been in this phase for longer than the duration, transition to the next phase
	if(context<CellStatechart>().TimeInPhase >= Duration){
		return transit<G1>();
	}
	return forward_event();
};
//------------------------------------------------------------
//------------------------------------------------------------
G1::G1( my_context ctx ):
	my_base( ctx ){
	context<CellStatechart>().TimeInPhase = 0.0;
	CellPtr myCell=context<CellStatechart>().pCell;
	RandomNumberGenerator* p_gen = RandomNumberGenerator::Instance();
	Duration=p_gen->NormalRandomDeviate(GetG1Duration(myCell),0.1*GetG1Duration(myCell));
	SetCellCyclePhase(myCell,G_ONE_PHASE);
};
sc::result G1::react( const EvMitosisUpdate & ){
	context<CellStatechart>().TimeInPhase+=SimulationTime::Instance()->GetTimeStep();
	if(context<CellStatechart>().TimeInPhase >= Duration){
		return transit<S>();
	}
	return forward_event();
};
//------------------------------------------------------------
//------------------------------------------------------------
S::S( my_context ctx ):
	my_base( ctx ){
	context<CellStatechart>().TimeInPhase = 0.0;
	CellPtr myCell=context<CellStatechart>().pCell;
	RandomNumberGenerator* p_gen = RandomNumberGenerator::Instance();
	Duration=p_gen->NormalRandomDeviate(GetSDuration(myCell),0.1*GetSDuration(myCell));
	SetCellCyclePhase(myCell,S_PHASE);
};
sc::result S::react( const EvMitosisUpdate & ){
	context<CellStatechart>().TimeInPhase+=SimulationTime::Instance()->GetTimeStep();
	if(context<CellStatechart>().TimeInPhase >= Duration){
		return transit<G2>();
	}
	return forward_event();
};
//------------------------------------------------------------
//------------------------------------------------------------
G2::G2( my_context ctx ):
	my_base( ctx ){
	context<CellStatechart>().TimeInPhase = 0.0;
	CellPtr myCell=context<CellStatechart>().pCell;
	RandomNumberGenerator* p_gen = RandomNumberGenerator::Instance();
	Duration=p_gen->NormalRandomDeviate(GetG2Duration(myCell),0.1*GetG2Duration(myCell));
	SetCellCyclePhase(myCell,G_TWO_PHASE);
};
sc::result G2::react( const EvMitosisUpdate & ){
	context<CellStatechart>().TimeInPhase+=SimulationTime::Instance()->GetTimeStep();
	if(context<CellStatechart>().TimeInPhase >= Duration){
		return transit<M>();
	}
	return forward_event();
};
//------------------------------------------------------------
//------------------------------------------------------------