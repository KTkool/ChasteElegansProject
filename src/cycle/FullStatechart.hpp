#ifndef FULLSTATECHART_HPP_
#define FULLSTATECHART_HPP_

#include <boost/statechart/event.hpp>
#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/simple_state.hpp>
#include <boost/statechart/state.hpp>
#include <boost/statechart/custom_reaction.hpp>
#include <boost/statechart/transition.hpp>
#include <boost/mpl/list.hpp>
#include <boost/config.hpp>

#include "RandomNumberGenerator.hpp"

namespace sc = boost::statechart;
namespace mpl = boost::mpl;


/*This header defines the structure of a statechart:
 * - Which states exist
 * - How they are nested
 * - What are the initial states
 * - What transitions exist
 *
 * Note that much of the cell cycle part will be the same for all statechart models and can be reproduced
 * unchanged.
 *
 *
 * The library makes use of curiously recurring templates. So the easiest thing is to forward declare all
 * states:
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
//Events to do with updating at each timestep
struct EvCheckCellData :  sc::event< EvCheckCellData > {};
struct EvGLP1Update :  sc::event< EvGLP1Update > {};
struct EvLAG1Update :  sc::event< EvLAG1Update > {};
struct EvGLD1Update :  sc::event< EvGLD1Update > {};
struct EvMitosisUpdate :  sc::event< EvMitosisUpdate > {};

//Events to do with forced transitions into each state
struct EvGoToGLP1Unbound: sc::event< EvGoToGLP1Unbound > {};
struct EvGoToGLP1Bound: sc::event< EvGoToGLP1Bound > {};
struct EvGoToGLP1Inactive: sc::event< EvGoToGLP1Inactive > {};
struct EvGoToGLP1Active: sc::event< EvGoToGLP1Active > {};
struct EvGoToGLP1Absent: sc::event< EvGoToGLP1Absent > {};
struct EvGoToLAG1Active: sc::event< EvGoToLAG1Active > {};
struct EvGoToLAG1Inactive: sc::event< EvGoToLAG1Inactive > {};
struct EvGoToGLD1Inactive: sc::event< EvGoToGLD1Inactive > {};
struct EvGoToGLD1Active: sc::event< EvGoToGLD1Active > {};
struct EvGoToMitosis: sc::event< EvGoToMitosis > {};
struct EvGoToMeiosis: sc::event< EvGoToMeiosis > {};
struct EvGoToM: sc::event< EvGoToM > {};
struct EvGoToG1: sc::event< EvGoToG1 > {};
struct EvGoToG2: sc::event< EvGoToG2 > {};
struct EvGoToS: sc::event< EvGoToS > {};

//One additional event to set the time in this phase of the cell cycle. Used when initialising a
//starting population of cells
struct EvGoToTime: sc::event< EvGoToTime > {
	double time;
	EvGoToTime(double timeInState):time(timeInState){};
};



/* Define the statemachine itself. It must always be named CellStatechart.
 * As with all states created using this library, templates are used to define the layout of the chart:
 * 1st template argument = the state you are defining (recursive)
 * 2nd argument = the substate that will be the initial state (in this case Living). If you specify
 * 				  multiple states in an mpl::list, then you are splitting into orthogonal regions and
 * 				  giving the initial state of each one.
 * 3rd argument = the parent state (in this case, we're at the top level so leave it blank)
 * In addition, this statemachine should also have:
 * 1) a constructor taking a CellPtr, and a member of type CellPtr so we can access the Chaste cell being controlled.
 * 2) a method Copy (copy constructor), which takes in a CellPtr and returns a boost::shared_ptr to the new statemachine
 * 3) a method SetCell, allowing the statechart's cell pointer to be reassigned. Used in making daughter cells.
*/
struct CellStatechart:  sc::state_machine<CellStatechart,Living>{
	CellStatechart(CellPtr myCell);
	CellPtr pCell;
	//double Activity;
	//Copier
	boost::shared_ptr<CellStatechart> Copy(CellPtr myCell);
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
};
struct Cytoplasm:  sc::simple_state<Cytoplasm,Living::orthogonal< 1 >,mpl::list<LAG1Inactive,GLD1Active> >{
};
struct Abstract:  sc::simple_state<Abstract,Living::orthogonal< 2 >,Mitosis>{
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
struct GLP1Active:  sc::state<GLP1Active,Membrane>{
	GLP1Active(my_context ctx);
	typedef mpl::list< sc::custom_reaction< EvGLP1Update> > reactions;
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

	typedef mpl::list<sc::custom_reaction< EvMitosisUpdate >,
			sc::custom_reaction< EvGoToMitosis >,
			sc::custom_reaction< EvGoToMeiosis >,
			sc::custom_reaction< EvGoToM >,
			sc::custom_reaction< EvGoToG1 >,
			sc::custom_reaction< EvGoToG2 >,
			sc::custom_reaction< EvGoToS > >reactions;
	sc::result react( const EvGoToMitosis & ){return transit<Mitosis>();};
	sc::result react( const EvGoToMeiosis & ){return transit<Meiosis>();};
	sc::result react( const EvGoToM & ){return transit<M>();};
	sc::result react( const EvGoToG1 & ){return transit<G1>();};
	sc::result react( const EvGoToG2 & ){return transit<G2>();};
	sc::result react( const EvGoToS & ){return transit<S>();};
	sc::result react( const EvMitosisUpdate & );
};

struct M:  sc::state<M,Mitosis>{
	double TimeInThisState;
	M(my_context ctx);
	typedef mpl::list<sc::custom_reaction< EvMitosisUpdate >,
			sc::custom_reaction< EvGoToTime > >reactions;
	sc::result react( const EvMitosisUpdate & );
	sc::result react( const EvGoToTime & );
};

struct G1:  sc::state<G1,Mitosis>{
	double TimeInThisState;
	double myRand;
	G1(my_context ctx);
	typedef mpl::list<sc::custom_reaction< EvMitosisUpdate>,
			sc::custom_reaction< EvGoToTime > >reactions;
	sc::result react( const EvMitosisUpdate & );
	sc::result react( const EvGoToTime & );
};

struct G2:  sc::state<G2,Mitosis>{
	double TimeInThisState;
	double myRand;
	G2(my_context ctx);
	typedef mpl::list<sc::custom_reaction< EvMitosisUpdate >,
			sc::custom_reaction< EvGoToTime > >reactions;
	sc::result react( const EvMitosisUpdate & );
	sc::result react( const EvGoToTime & );
};

struct S:  sc::state<S,Mitosis>{
	double TimeInThisState;
	S(my_context ctx);
	typedef mpl::list<sc::custom_reaction< EvMitosisUpdate >,
			sc::custom_reaction< EvGoToTime > >reactions;
	sc::result react( const EvMitosisUpdate & );
	sc::result react( const EvGoToTime & );
};

/* This last state is unusual in that it performs an action on entry (it initialises its member variable radius).
 * If a state does something on entry, and so has a non-default constructor, that functionality is not supported
 * by sc::simple_state, and instead the state needs to inherit sc::state. sc::states need
 * to have a constructor that takes in my_context ctx, and in the .cpp file you'll see they have a short initialiser
 * list as well. This bit of extra code is the same for all sc::states, and if you wanted, you could just make all
 * states sc::states to save yourself having to determine which are special cases.
 */
struct Meiosis:  sc::state<Meiosis,Abstract>{
	double radius;
	typedef sc::custom_reaction< EvMitosisUpdate > reactions;
	Meiosis(my_context ctx);
	sc::result react( const EvMitosisUpdate & );
};

#endif
