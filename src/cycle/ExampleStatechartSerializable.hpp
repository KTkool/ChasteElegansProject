#ifndef EXAMPLESTATECHARTSERIALIZABLE_HPP_
#define EXAMPLESTATECHARTSERIALIZABLE_HPP_

//INCLUDE ALLLLLLLLLLLL THE HEADERS
#include <boost/statechart/event.hpp>
#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/simple_state.hpp>
#include <boost/statechart/state.hpp>
#include <boost/statechart/custom_reaction.hpp>
#include <boost/statechart/transition.hpp>
#include <boost/mpl/list.hpp>
#include <boost/config.hpp>
#include "ChasteSerialization.hpp"
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/vector.hpp>
namespace sc = boost::statechart;
namespace mpl = boost::mpl;


/* This boost statechart header defines the structure of the statechart model:
 * - Which states exist
 * - How they are nested
 * - What are the initial states
 * - What transitions can occur
 * The boost library makes use of curiously recurring templates. So it's best to forward declare everything.
 * First, all the states that exist:
*/
struct Living;
struct Membrane;
struct Cytoplasm;
struct Abstract;
struct GLP1Unbound;
struct GLP1Bound;
struct GLP1Inactive;
struct GLP1Active;
struct GLP1Absent;
struct GLD1Active;
struct GLD1Inactive;
struct LAG1Active;
struct LAG1Inactive;
struct M;
struct G1;
struct G2;
struct S;
struct Mitosis;
struct Meiosis;

//Declare all the events we want to be able to handle:
//1) Events to do with updating the chart at each timestep - one master update event, plus one
//for each orthogonal region
struct EvCheckCellData :  sc::event< EvCheckCellData > {};
struct EvGLP1Update :  sc::event< EvGLP1Update > {};
struct EvLAG1Update :  sc::event< EvLAG1Update > {};
struct EvGLD1Update :  sc::event< EvGLD1Update > {};
struct EvMitosisUpdate :  sc::event< EvMitosisUpdate > {};
//2) Events allowing a forced transitions into each innermost or leaf state - used for inheritance
struct EvGoToGLP1Unbound: sc::event< EvGoToGLP1Unbound > {};
struct EvGoToGLP1Bound: sc::event< EvGoToGLP1Bound > {};
struct EvGoToGLP1Inactive: sc::event< EvGoToGLP1Inactive > {};
struct EvGoToGLP1Active: sc::event< EvGoToGLP1Active > {};
struct EvGoToGLP1Absent: sc::event< EvGoToGLP1Absent > {};
struct EvGoToLAG1Active: sc::event< EvGoToLAG1Active > {};
struct EvGoToLAG1Inactive: sc::event< EvGoToLAG1Inactive > {};
struct EvGoToGLD1Inactive: sc::event< EvGoToGLD1Inactive > {};
struct EvGoToGLD1Active: sc::event< EvGoToGLD1Active > {};
struct EvGoToMeiosis: sc::event< EvGoToMeiosis > {};
struct EvGoToM: sc::event< EvGoToM > {};
struct EvGoToG1: sc::event< EvGoToG1 > {};
struct EvGoToG2: sc::event< EvGoToG2 > {};
struct EvGoToS: sc::event< EvGoToS > {};



/*Define the statemachine itself. Chaste expects it to be named CellStatechart.
* In the boost library, templates are used to declare the layout of the chart:
* 1st template argument = the state you are defining (recursive)
* 2nd argument = the substate that will be active initially (in this case Living). If you specify
* 				  multiple states in an mpl::list, then you are splitting into orthogonal regions and
* 				  giving the initial state of each one.
* 3rd argument = the parent state (in this case, we're at the top level so leave it blank)
* 
* In addition, Chaste statemachines need some extra stuff:
* 1) A CellPtr (and a corresponding setter method) so we can access the Chaste cell being controlled.
* 2) A method Copy (copy constructor), which returns a boost::shared_ptr to a new copy of this statemachine
* 3) Setter and getter methods for the state. Saves/Loads state info as a uint for archiving.
*
* You may also include variables in the chart, to track continuous aspects of the state (like
* delays or activity levels).
*/

struct CellStatechart:  sc::state_machine<CellStatechart,Living>{

	CellPtr pCell;

	//A statechart variable tracking the time elapsed in the current cell cycle phase to allow for
	//delays
	double TimeInPhase;

	//Constructor. All this stuff has default values, so need not call with arguments
	CellStatechart();
	
	//Utility methods, mostly to do with copying and archiving and called by the wrapper class
	//StatechartCellCycleModelSerializable. 
	boost::shared_ptr<CellStatechart> Copy();
	uint GetState();
	std::vector<double> GetVariables();
	void SetState(uint state);
	void SetVariables(std::vector<double> variableValues);
	void SetCell(CellPtr newCell);
};



/*Now we flesh out the inner states. In terms of template arguments, they work as described in the previous comment
 * In addition a state needs to declare
 * 1) the list of events it can respond to, if any (look at the typedef reactions line)
 * 2) for each event that the state responds to, a react function which will be called when that event occurs
 */
struct Living:  sc::simple_state<Living,CellStatechart,mpl::list<Membrane,Cytoplasm,Abstract > >{
	typedef sc::custom_reaction< EvCheckCellData > reactions;
	sc::result react( const EvCheckCellData & );
};
struct Dead:  sc::simple_state<Dead,CellStatechart>{
};
struct Membrane:  sc::simple_state<Membrane,Living::orthogonal< 0 >,GLP1Unbound>{
	typedef sc::custom_reaction< EvGLP1Update > reactions;
	sc::result react( const EvGLP1Update & ){return discard_event();};
};
struct Cytoplasm:  sc::simple_state<Cytoplasm,Living::orthogonal< 1 >,mpl::list<LAG1Inactive,GLD1Active> >{
	typedef mpl::list<sc::custom_reaction< EvLAG1Update >,
	sc::custom_reaction< EvGLD1Update> >reactions;
	sc::result react( const EvGLD1Update & ){return discard_event();};
	sc::result react( const EvLAG1Update & ){return discard_event();};
};
struct Abstract:  sc::simple_state<Abstract,Living::orthogonal< 2 >,Mitosis>{
	typedef sc::custom_reaction< EvMitosisUpdate > reactions;
	sc::result react( const EvMitosisUpdate & ){return discard_event();};
};



/* Now we're defining the innermost states. Each orthogonal region will be in exactly one of these states.
 * Here is where we will insert some code that allows us to copy a statechart. To copy, we first make a new
 * statechart. All the new statechart's orthogonal regions will be in the initial state. Then we want to be
 * able to force it to transition into the same state as the "parent". So we would determine that, say
 * the parent is in state B. We know the child is in the initial state A. So we let A handle an event that says "go to B",
 * for any B in its region.
 */
struct GLP1Unbound:  sc::simple_state<GLP1Unbound,Membrane>{
	typedef mpl::list<sc::custom_reaction< EvGLP1Update >,
			sc::custom_reaction< EvGoToGLP1Unbound >,
			sc::custom_reaction< EvGoToGLP1Active >,
			sc::custom_reaction< EvGoToGLP1Bound >,
			sc::custom_reaction< EvGoToGLP1Inactive >,
			sc::custom_reaction< EvGoToGLP1Absent > > reactions;
	sc::result react( const EvGoToGLP1Unbound & ){return transit<GLP1Unbound>();};
	sc::result react( const EvGoToGLP1Active & ){return transit<GLP1Active>();};
	sc::result react( const EvGoToGLP1Bound& ){return transit<GLP1Bound>();};
	sc::result react( const EvGoToGLP1Inactive & ){return transit<GLP1Inactive>();};
	sc::result react( const EvGoToGLP1Absent & ){return transit<GLP1Absent>();};
	sc::result react( const EvGLP1Update & );
};
struct GLP1Bound:  sc::simple_state<GLP1Bound,Membrane>{
	typedef sc::custom_reaction< EvGLP1Update > reactions;
	sc::result react( const EvGLP1Update & );
};
struct GLP1Inactive:  sc::simple_state<GLP1Inactive,Membrane>{
	typedef sc::custom_reaction< EvGLP1Update > reactions;
	sc::result react( const EvGLP1Update & );
};
struct GLP1Active:  sc::simple_state<GLP1Active,Membrane>{
	typedef sc::custom_reaction< EvGLP1Update > reactions;
	sc::result react( const EvGLP1Update & );
};
struct GLP1Absent:  sc::simple_state<GLP1Absent,Membrane>{
	typedef sc::custom_reaction< EvGLP1Update > reactions;
	sc::result react( const EvGLP1Update & );
};





struct LAG1Active:  sc::simple_state<LAG1Active,Cytoplasm::orthogonal<0> >{

	typedef sc::custom_reaction< EvLAG1Update > reactions;
	sc::result react( const EvLAG1Update & );
};
struct LAG1Inactive:  sc::simple_state<LAG1Inactive,Cytoplasm::orthogonal<0> >{
	typedef mpl::list<sc::custom_reaction< EvLAG1Update >,
			sc::custom_reaction< EvGoToLAG1Active >,
			sc::custom_reaction< EvGoToLAG1Inactive > > reactions;
	sc::result react( const EvGoToLAG1Active & ){return transit<LAG1Active>();};
	sc::result react( const EvGoToLAG1Inactive & ){return transit<LAG1Inactive>();};
	sc::result react( const EvLAG1Update & );
};



struct GLD1Inactive:  sc::simple_state<GLD1Inactive,Cytoplasm::orthogonal<1> >{

	typedef sc::custom_reaction< EvGLD1Update > reactions;
	sc::result react( const EvGLD1Update & );
};
struct GLD1Active:  sc::simple_state<GLD1Active,Cytoplasm::orthogonal<1> >{
	typedef mpl::list<sc::custom_reaction< EvGLD1Update >,
			sc::custom_reaction< EvGoToGLD1Active >,
			sc::custom_reaction< EvGoToGLD1Inactive > > reactions;
	sc::result react( const EvGoToGLD1Active & ){return transit<GLD1Active>();};
	sc::result react( const EvGoToGLD1Inactive & ){return transit<GLD1Inactive>();};
	sc::result react( const EvGLD1Update & );
};



struct Mitosis:  sc::simple_state<Mitosis,Abstract,G1>{
	typedef mpl::list< sc::custom_reaction< EvMitosisUpdate >,
			sc::custom_reaction< EvGoToMeiosis >,
			sc::custom_reaction< EvGoToM >,
			sc::custom_reaction< EvGoToG1 >,
			sc::custom_reaction< EvGoToG2 >,
			sc::custom_reaction< EvGoToS > >reactions;
	sc::result react( const EvGoToMeiosis & ){return transit<Meiosis>();};
	sc::result react( const EvGoToM & ){return transit<M>();};
	sc::result react( const EvGoToG1 & ){return transit<G1>();};
	sc::result react( const EvGoToG2 & ){return transit<G2>();};
	sc::result react( const EvGoToS & ){return transit<S>();};
	sc::result react( const EvMitosisUpdate & );
};

struct M:  sc::state<M,Mitosis>{
	double Duration;
	M( my_context ctx );
	typedef sc::custom_reaction< EvMitosisUpdate > reactions;
	sc::result react( const EvMitosisUpdate & );
};

struct G1:  sc::state<G1,Mitosis>{
	double Duration;
	G1( my_context ctx );
	typedef sc::custom_reaction< EvMitosisUpdate > reactions;
	sc::result react( const EvMitosisUpdate & );
};

struct G2:  sc::state<G2,Mitosis>{
	double Duration;
	G2( my_context ctx );
	typedef sc::custom_reaction< EvMitosisUpdate > reactions;
	sc::result react( const EvMitosisUpdate & );
};

struct S:  sc::state<S,Mitosis>{
	double Duration;
	S( my_context ctx );
	typedef sc::custom_reaction< EvMitosisUpdate > reactions;
	sc::result react( const EvMitosisUpdate & );
};

struct Meiosis:  sc::state<Meiosis,Abstract>{
	Meiosis( my_context ctx );
	double radius;
	typedef sc::custom_reaction< EvMitosisUpdate > reactions;
	sc::result react( const EvMitosisUpdate & );
};


#endif
