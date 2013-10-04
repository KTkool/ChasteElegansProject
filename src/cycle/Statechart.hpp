#ifndef STATECHART_HPP_
#define STATECHART_HPP_

#include <boost/statechart/event.hpp>
#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/simple_state.hpp>
#include <boost/statechart/state.hpp>
#include <boost/statechart/custom_reaction.hpp>
#include <boost/statechart/transition.hpp>
#include <boost/mpl/list.hpp>
#include <boost/config.hpp>

namespace sc = boost::statechart;
namespace mpl = boost::mpl;

struct Running;
struct CellStateChart_Membrane;
struct CellStateChart_Membrane_GLP1;
struct CellStateChart_Membrane_GLP1_Unbound;
struct CellStateChart_Membrane_GLP1_Bound;
struct CellStateChart_Membrane_GLP1_Inactive;
struct CellStateChart_Membrane_GLP1_Active;
struct CellStateChart_Membrane_GLP1_Absent;
struct CellStateChart_CytoplasmLAG1;
struct CellStateChart_CytoplasmLAG1_Active;
struct CellStateChart_CytoplasmLAG1_Inactive;
struct CellStateChart_CytoplasmGLD1;
struct CellStateChart_CytoplasmGLD1_Active;
struct CellStateChart_CytoplasmGLD1_Inactive;
struct CellStateChart_Abstract;
struct CellStateChart_Abstract_Mitosis;
struct CellStateChart_Abstract_Mitosis_G_ONE;
struct CellStateChart_Abstract_Mitosis_S;
struct CellStateChart_Abstract_Mitosis_G_TWO;
struct CellStateChart_Abstract_Mitosis_M;
struct CellStateChart_Abstract_Meiosis;
struct CellStateChart_Life;
struct CellStateChart_Life_Living;
struct CellStateChart_Life_Dead;

struct EvCheckCellData :  sc::event< EvCheckCellData > {};
struct EvCellStateChart_LifeUpdate : sc::event< EvCellStateChart_LifeUpdate > {};
struct EvCellStateChart_AbstractUpdate : sc::event< EvCellStateChart_AbstractUpdate > {};
struct EvCellStateChart_CytoplasmGLD1Update : sc::event< EvCellStateChart_CytoplasmGLD1Update > {};
struct EvCellStateChart_CytoplasmLAG1Update : sc::event< EvCellStateChart_CytoplasmLAG1Update > {};
struct EvCellStateChart_MembraneUpdate : sc::event< EvCellStateChart_MembraneUpdate > {};

struct EvGoToCellStateChart_Membrane_GLP1_Unbound : sc::event< EvGoToCellStateChart_Membrane_GLP1_Unbound > {};
struct EvGoToCellStateChart_Membrane_GLP1_Bound : sc::event< EvGoToCellStateChart_Membrane_GLP1_Bound > {};
struct EvGoToCellStateChart_Membrane_GLP1_Inactive : sc::event< EvGoToCellStateChart_Membrane_GLP1_Inactive > {};
struct EvGoToCellStateChart_Membrane_GLP1_Active : sc::event< EvGoToCellStateChart_Membrane_GLP1_Active > {};
struct EvGoToCellStateChart_Membrane_GLP1_Absent : sc::event< EvGoToCellStateChart_Membrane_GLP1_Absent > {};
struct EvGoToCellStateChart_CytoplasmLAG1_Active : sc::event< EvGoToCellStateChart_CytoplasmLAG1_Active > {};
struct EvGoToCellStateChart_CytoplasmLAG1_Inactive : sc::event< EvGoToCellStateChart_CytoplasmLAG1_Inactive > {};
struct EvGoToCellStateChart_CytoplasmGLD1_Active : sc::event< EvGoToCellStateChart_CytoplasmGLD1_Active > {};
struct EvGoToCellStateChart_CytoplasmGLD1_Inactive : sc::event< EvGoToCellStateChart_CytoplasmGLD1_Inactive > {};
struct EvGoToCellStateChart_Abstract_Mitosis_G_ONE : sc::event< EvGoToCellStateChart_Abstract_Mitosis_G_ONE > {};
struct EvGoToCellStateChart_Abstract_Mitosis_S : sc::event< EvGoToCellStateChart_Abstract_Mitosis_S > {};
struct EvGoToCellStateChart_Abstract_Mitosis_G_TWO : sc::event< EvGoToCellStateChart_Abstract_Mitosis_G_TWO > {};
struct EvGoToCellStateChart_Abstract_Mitosis_M : sc::event< EvGoToCellStateChart_Abstract_Mitosis_M > {};
struct EvGoToCellStateChart_Abstract_Meiosis : sc::event< EvGoToCellStateChart_Abstract_Meiosis > {};
struct EvGoToCellStateChart_Life_Living : sc::event< EvGoToCellStateChart_Life_Living > {};
struct EvGoToCellStateChart_Life_Dead : sc::event< EvGoToCellStateChart_Life_Dead > {};

struct EvGoToTime: sc::event< EvGoToTime > {
double time;
EvGoToTime(double timeInState):time(timeInState){};
};

//PARENT STATECHART

struct CellStatechart:  sc::state_machine<CellStatechart,Running >{
CellStatechart(CellPtr myCell);
CellPtr pCell;
boost::shared_ptr<CellStatechart> Copy(CellPtr myCell);
void SetCell(CellPtr newCell);
};

//FIRSTRESPONDER STATE

struct Running:  sc::simple_state<Running,CellStatechart,mpl::list< CellStateChart_Life,CellStateChart_Abstract,CellStateChart_CytoplasmGLD1,CellStateChart_CytoplasmLAG1,CellStateChart_Membrane > >{
typedef sc::custom_reaction< EvCheckCellData > reactions;
sc::result react( const EvCheckCellData & );
};
//STATES

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
struct CellStateChart_Membrane: sc::state<CellStateChart_Membrane,Running::orthogonal<4>,CellStateChart_Membrane_GLP1>{
   CellStateChart_Membrane(my_context ctx);

   typedef mpl::list< sc::custom_reaction< EvCellStateChart_MembraneUpdate >,
    sc::custom_reaction< EvGoToCellStateChart_Membrane_GLP1_Unbound >,
    sc::custom_reaction< EvGoToCellStateChart_Membrane_GLP1_Bound >,
    sc::custom_reaction< EvGoToCellStateChart_Membrane_GLP1_Inactive >,
    sc::custom_reaction< EvGoToCellStateChart_Membrane_GLP1_Active >,
    sc::custom_reaction< EvGoToCellStateChart_Membrane_GLP1_Absent >   > reactions;

   sc::result react( const EvCellStateChart_MembraneUpdate & );
   sc::result react( const EvGoToCellStateChart_Membrane_GLP1_Unbound & ){return transit<CellStateChart_Membrane_GLP1_Unbound>();};
   sc::result react( const EvGoToCellStateChart_Membrane_GLP1_Bound & ){return transit<CellStateChart_Membrane_GLP1_Bound>();};
   sc::result react( const EvGoToCellStateChart_Membrane_GLP1_Inactive & ){return transit<CellStateChart_Membrane_GLP1_Inactive>();};
   sc::result react( const EvGoToCellStateChart_Membrane_GLP1_Active & ){return transit<CellStateChart_Membrane_GLP1_Active>();};
   sc::result react( const EvGoToCellStateChart_Membrane_GLP1_Absent & ){return transit<CellStateChart_Membrane_GLP1_Absent>();};
};

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
struct CellStateChart_Membrane_GLP1: sc::state<CellStateChart_Membrane_GLP1,CellStateChart_Membrane,CellStateChart_Membrane_GLP1_Unbound>{
   CellStateChart_Membrane_GLP1(my_context ctx);

   typedef mpl::list< sc::custom_reaction< EvCellStateChart_MembraneUpdate >   > reactions;

   sc::result react( const EvCellStateChart_MembraneUpdate & );
};

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
struct CellStateChart_CytoplasmLAG1: sc::state<CellStateChart_CytoplasmLAG1,Running::orthogonal<3>,CellStateChart_CytoplasmLAG1_Active>{
   CellStateChart_CytoplasmLAG1(my_context ctx);

   typedef mpl::list< sc::custom_reaction< EvCellStateChart_CytoplasmLAG1Update >,
    sc::custom_reaction< EvGoToCellStateChart_CytoplasmLAG1_Active >,
    sc::custom_reaction< EvGoToCellStateChart_CytoplasmLAG1_Inactive >   > reactions;

   sc::result react( const EvCellStateChart_CytoplasmLAG1Update & );
   sc::result react( const EvGoToCellStateChart_CytoplasmLAG1_Active & ){return transit<CellStateChart_CytoplasmLAG1_Active>();};
   sc::result react( const EvGoToCellStateChart_CytoplasmLAG1_Inactive & ){return transit<CellStateChart_CytoplasmLAG1_Inactive>();};
};

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
struct CellStateChart_CytoplasmGLD1: sc::state<CellStateChart_CytoplasmGLD1,Running::orthogonal<2>,CellStateChart_CytoplasmGLD1_Active>{
   CellStateChart_CytoplasmGLD1(my_context ctx);

   typedef mpl::list< sc::custom_reaction< EvCellStateChart_CytoplasmGLD1Update >,
    sc::custom_reaction< EvGoToCellStateChart_CytoplasmGLD1_Active >,
    sc::custom_reaction< EvGoToCellStateChart_CytoplasmGLD1_Inactive >   > reactions;

   sc::result react( const EvCellStateChart_CytoplasmGLD1Update & );
   sc::result react( const EvGoToCellStateChart_CytoplasmGLD1_Active & ){return transit<CellStateChart_CytoplasmGLD1_Active>();};
   sc::result react( const EvGoToCellStateChart_CytoplasmGLD1_Inactive & ){return transit<CellStateChart_CytoplasmGLD1_Inactive>();};
};

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
struct CellStateChart_Abstract: sc::state<CellStateChart_Abstract,Running::orthogonal<1>,CellStateChart_Abstract_Mitosis>{
   CellStateChart_Abstract(my_context ctx);

   typedef mpl::list< sc::custom_reaction< EvCellStateChart_AbstractUpdate >,
    sc::custom_reaction< EvGoToCellStateChart_Abstract_Mitosis_G_ONE >,
    sc::custom_reaction< EvGoToCellStateChart_Abstract_Mitosis_S >,
    sc::custom_reaction< EvGoToCellStateChart_Abstract_Mitosis_G_TWO >,
    sc::custom_reaction< EvGoToCellStateChart_Abstract_Mitosis_M >,
    sc::custom_reaction< EvGoToCellStateChart_Abstract_Meiosis >   > reactions;

   sc::result react( const EvCellStateChart_AbstractUpdate & );
   sc::result react( const EvGoToCellStateChart_Abstract_Mitosis_G_ONE & ){return transit<CellStateChart_Abstract_Mitosis_G_ONE>();};
   sc::result react( const EvGoToCellStateChart_Abstract_Mitosis_S & ){return transit<CellStateChart_Abstract_Mitosis_S>();};
   sc::result react( const EvGoToCellStateChart_Abstract_Mitosis_G_TWO & ){return transit<CellStateChart_Abstract_Mitosis_G_TWO>();};
   sc::result react( const EvGoToCellStateChart_Abstract_Mitosis_M & ){return transit<CellStateChart_Abstract_Mitosis_M>();};
   sc::result react( const EvGoToCellStateChart_Abstract_Meiosis & ){return transit<CellStateChart_Abstract_Meiosis>();};
};

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
struct CellStateChart_Abstract_Mitosis: sc::state<CellStateChart_Abstract_Mitosis,CellStateChart_Abstract,CellStateChart_Abstract_Mitosis_G_ONE>{
   CellStateChart_Abstract_Mitosis(my_context ctx);

   typedef mpl::list< sc::custom_reaction< EvCellStateChart_AbstractUpdate >   > reactions;

   sc::result react( const EvCellStateChart_AbstractUpdate & );
};

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
struct CellStateChart_Life: sc::state<CellStateChart_Life,Running::orthogonal<0>,CellStateChart_Life_Living>{
   CellStateChart_Life(my_context ctx);

   typedef mpl::list< sc::custom_reaction< EvCellStateChart_LifeUpdate >,
    sc::custom_reaction< EvGoToCellStateChart_Life_Living >,
    sc::custom_reaction< EvGoToCellStateChart_Life_Dead >   > reactions;

   sc::result react( const EvCellStateChart_LifeUpdate & );
   sc::result react( const EvGoToCellStateChart_Life_Living & ){return transit<CellStateChart_Life_Living>();};
   sc::result react( const EvGoToCellStateChart_Life_Dead & ){return transit<CellStateChart_Life_Dead>();};
};

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
struct CellStateChart_Membrane_GLP1_Unbound: sc::state<CellStateChart_Membrane_GLP1_Unbound,CellStateChart_Membrane_GLP1 >{
   CellStateChart_Membrane_GLP1_Unbound(my_context ctx);

  typedef mpl::list< sc::custom_reaction< EvCellStateChart_MembraneUpdate > > reactions;
   sc::result react( const EvCellStateChart_MembraneUpdate & );
};

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
struct CellStateChart_Membrane_GLP1_Bound: sc::state<CellStateChart_Membrane_GLP1_Bound,CellStateChart_Membrane_GLP1 >{
   CellStateChart_Membrane_GLP1_Bound(my_context ctx);

  typedef mpl::list< sc::custom_reaction< EvCellStateChart_MembraneUpdate > > reactions;
   sc::result react( const EvCellStateChart_MembraneUpdate & );
};

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
struct CellStateChart_Membrane_GLP1_Inactive: sc::state<CellStateChart_Membrane_GLP1_Inactive,CellStateChart_Membrane_GLP1 >{
   CellStateChart_Membrane_GLP1_Inactive(my_context ctx);

  typedef mpl::list< sc::custom_reaction< EvCellStateChart_MembraneUpdate > > reactions;
   sc::result react( const EvCellStateChart_MembraneUpdate & );
};

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
struct CellStateChart_Membrane_GLP1_Active: sc::state<CellStateChart_Membrane_GLP1_Active,CellStateChart_Membrane_GLP1 >{
   CellStateChart_Membrane_GLP1_Active(my_context ctx);

  typedef mpl::list< sc::custom_reaction< EvCellStateChart_MembraneUpdate > > reactions;
   sc::result react( const EvCellStateChart_MembraneUpdate & );
};

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
struct CellStateChart_Membrane_GLP1_Absent: sc::state<CellStateChart_Membrane_GLP1_Absent,CellStateChart_Membrane_GLP1 >{
   CellStateChart_Membrane_GLP1_Absent(my_context ctx);

  typedef mpl::list< sc::custom_reaction< EvCellStateChart_MembraneUpdate > > reactions;
   sc::result react( const EvCellStateChart_MembraneUpdate & );
};

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
struct CellStateChart_CytoplasmLAG1_Active: sc::state<CellStateChart_CytoplasmLAG1_Active,CellStateChart_CytoplasmLAG1 >{
   CellStateChart_CytoplasmLAG1_Active(my_context ctx);

  typedef mpl::list< sc::custom_reaction< EvCellStateChart_CytoplasmLAG1Update > > reactions;
   sc::result react( const EvCellStateChart_CytoplasmLAG1Update & );
};

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
struct CellStateChart_CytoplasmLAG1_Inactive: sc::state<CellStateChart_CytoplasmLAG1_Inactive,CellStateChart_CytoplasmLAG1 >{
   CellStateChart_CytoplasmLAG1_Inactive(my_context ctx);

  typedef mpl::list< sc::custom_reaction< EvCellStateChart_CytoplasmLAG1Update > > reactions;
   sc::result react( const EvCellStateChart_CytoplasmLAG1Update & );
};

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
struct CellStateChart_CytoplasmGLD1_Active: sc::state<CellStateChart_CytoplasmGLD1_Active,CellStateChart_CytoplasmGLD1 >{
   CellStateChart_CytoplasmGLD1_Active(my_context ctx);

  typedef mpl::list< sc::custom_reaction< EvCellStateChart_CytoplasmGLD1Update > > reactions;
   sc::result react( const EvCellStateChart_CytoplasmGLD1Update & );
};

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
struct CellStateChart_CytoplasmGLD1_Inactive: sc::state<CellStateChart_CytoplasmGLD1_Inactive,CellStateChart_CytoplasmGLD1 >{
   CellStateChart_CytoplasmGLD1_Inactive(my_context ctx);

  typedef mpl::list< sc::custom_reaction< EvCellStateChart_CytoplasmGLD1Update > > reactions;
   sc::result react( const EvCellStateChart_CytoplasmGLD1Update & );
};

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
struct CellStateChart_Abstract_Mitosis_G_ONE: sc::state<CellStateChart_Abstract_Mitosis_G_ONE,CellStateChart_Abstract_Mitosis >{
   double TimeInThisState;
CellStateChart_Abstract_Mitosis_G_ONE(my_context ctx);

  typedef mpl::list< sc::custom_reaction< EvCellStateChart_AbstractUpdate >   ,sc::custom_reaction< EvGoToTime >
 > reactions;
   sc::result react( const EvCellStateChart_AbstractUpdate & );
   sc::result react( const EvGoToTime & );
};

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
struct CellStateChart_Abstract_Mitosis_S: sc::state<CellStateChart_Abstract_Mitosis_S,CellStateChart_Abstract_Mitosis >{
   double TimeInThisState;
CellStateChart_Abstract_Mitosis_S(my_context ctx);

  typedef mpl::list< sc::custom_reaction< EvCellStateChart_AbstractUpdate >   ,sc::custom_reaction< EvGoToTime >
 > reactions;
   sc::result react( const EvCellStateChart_AbstractUpdate & );
   sc::result react( const EvGoToTime & );
};

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
struct CellStateChart_Abstract_Mitosis_G_TWO: sc::state<CellStateChart_Abstract_Mitosis_G_TWO,CellStateChart_Abstract_Mitosis >{
   double TimeInThisState;
CellStateChart_Abstract_Mitosis_G_TWO(my_context ctx);

  typedef mpl::list< sc::custom_reaction< EvCellStateChart_AbstractUpdate >   ,sc::custom_reaction< EvGoToTime >
 > reactions;
   sc::result react( const EvCellStateChart_AbstractUpdate & );
   sc::result react( const EvGoToTime & );
};

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
struct CellStateChart_Abstract_Mitosis_M: sc::state<CellStateChart_Abstract_Mitosis_M,CellStateChart_Abstract_Mitosis >{
   double TimeInThisState;
CellStateChart_Abstract_Mitosis_M(my_context ctx);

  typedef mpl::list< sc::custom_reaction< EvCellStateChart_AbstractUpdate >   ,sc::custom_reaction< EvGoToTime >
 > reactions;
   sc::result react( const EvCellStateChart_AbstractUpdate & );
   sc::result react( const EvGoToTime & );
};

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
struct CellStateChart_Abstract_Meiosis: sc::state<CellStateChart_Abstract_Meiosis,CellStateChart_Abstract >{
   double radius;
   CellStateChart_Abstract_Meiosis(my_context ctx);

  typedef mpl::list< sc::custom_reaction< EvCellStateChart_AbstractUpdate > > reactions;
   sc::result react( const EvCellStateChart_AbstractUpdate & );
};

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
struct CellStateChart_Life_Living: sc::state<CellStateChart_Life_Living,CellStateChart_Life >{
   CellStateChart_Life_Living(my_context ctx);

  typedef mpl::list< sc::custom_reaction< EvCellStateChart_LifeUpdate > > reactions;
   sc::result react( const EvCellStateChart_LifeUpdate & );
};

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
struct CellStateChart_Life_Dead: sc::state<CellStateChart_Life_Dead,CellStateChart_Life >{
   CellStateChart_Life_Dead(my_context ctx);

  typedef mpl::list< sc::custom_reaction< EvCellStateChart_LifeUpdate > > reactions;
   sc::result react( const EvCellStateChart_LifeUpdate & );
};

#endif
