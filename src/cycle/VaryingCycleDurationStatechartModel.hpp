#ifndef VaryingCycleDurationStatechartModel_HPP_
#define VaryingCycleDurationStatechartModel_HPP_

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

struct Running;
struct CellStateChart_Life;
struct CellStateChart_Life_Living;
struct CellStateChart_Life_Dead;
struct CellStateChart_GLP1;
struct CellStateChart_GLP1_Unbound;
struct CellStateChart_GLP1_Inactive;
struct CellStateChart_GLP1_Active;
struct CellStateChart_GLP1_Bound;
struct CellStateChart_GLP1_Absent;
struct CellStateChart_LAG1;
struct CellStateChart_LAG1_Inactive;
struct CellStateChart_LAG1_Active;
struct CellStateChart_GLD1;
struct CellStateChart_GLD1_Active;
struct CellStateChart_GLD1_Inactive;
struct CellStateChart_CellCycle;
struct CellStateChart_CellCycle_Mitosis;
struct CellStateChart_CellCycle_Mitosis_G1;
struct CellStateChart_CellCycle_Mitosis_G2;
struct CellStateChart_CellCycle_Mitosis_S;
struct CellStateChart_CellCycle_Mitosis_M;
struct CellStateChart_CellCycle_Meiosis;

struct EvCheckCellData :  sc::event< EvCheckCellData > {};
struct EvCellStateChart_CellCycleUpdate : sc::event< EvCellStateChart_CellCycleUpdate > {};
struct EvCellStateChart_GLD1Update : sc::event< EvCellStateChart_GLD1Update > {};
struct EvCellStateChart_LAG1Update : sc::event< EvCellStateChart_LAG1Update > {};
struct EvCellStateChart_GLP1Update : sc::event< EvCellStateChart_GLP1Update > {};
struct EvCellStateChart_LifeUpdate : sc::event< EvCellStateChart_LifeUpdate > {};

struct EvGoToCellStateChart_Life_Living : sc::event< EvGoToCellStateChart_Life_Living > {};
struct EvGoToCellStateChart_Life_Dead : sc::event< EvGoToCellStateChart_Life_Dead > {};
struct EvGoToCellStateChart_GLP1_Unbound : sc::event< EvGoToCellStateChart_GLP1_Unbound > {};
struct EvGoToCellStateChart_GLP1_Inactive : sc::event< EvGoToCellStateChart_GLP1_Inactive > {};
struct EvGoToCellStateChart_GLP1_Active : sc::event< EvGoToCellStateChart_GLP1_Active > {};
struct EvGoToCellStateChart_GLP1_Bound : sc::event< EvGoToCellStateChart_GLP1_Bound > {};
struct EvGoToCellStateChart_GLP1_Absent : sc::event< EvGoToCellStateChart_GLP1_Absent > {};
struct EvGoToCellStateChart_LAG1_Inactive : sc::event< EvGoToCellStateChart_LAG1_Inactive > {};
struct EvGoToCellStateChart_LAG1_Active : sc::event< EvGoToCellStateChart_LAG1_Active > {};
struct EvGoToCellStateChart_GLD1_Active : sc::event< EvGoToCellStateChart_GLD1_Active > {};
struct EvGoToCellStateChart_GLD1_Inactive : sc::event< EvGoToCellStateChart_GLD1_Inactive > {};
struct EvGoToCellStateChart_CellCycle_Mitosis_G1 : sc::event< EvGoToCellStateChart_CellCycle_Mitosis_G1 > {};
struct EvGoToCellStateChart_CellCycle_Mitosis_G2 : sc::event< EvGoToCellStateChart_CellCycle_Mitosis_G2 > {};
struct EvGoToCellStateChart_CellCycle_Mitosis_S : sc::event< EvGoToCellStateChart_CellCycle_Mitosis_S > {};
struct EvGoToCellStateChart_CellCycle_Mitosis_M : sc::event< EvGoToCellStateChart_CellCycle_Mitosis_M > {};
struct EvGoToCellStateChart_CellCycle_Meiosis : sc::event< EvGoToCellStateChart_CellCycle_Meiosis > {};

//PARENT STATECHART

struct CellStatechart:  sc::state_machine<CellStatechart,Running>{
  CellStatechart();

  CellPtr pCell;

  boost::shared_ptr<CellStatechart> Copy(boost::shared_ptr<CellStatechart> myNewStatechart);

  int GetState();
  std::vector<double> GetVariables();
  void SetState(int state);
  void SetVariables(std::vector<double> variableValues);
  void SetCell(CellPtr newCell);

  double TimeInPhase;
};

//FIRSTRESPONDER STATE

struct Running:  sc::simple_state<Running,CellStatechart,mpl::list< CellStateChart_CellCycle,CellStateChart_GLD1,CellStateChart_LAG1,CellStateChart_GLP1,CellStateChart_Life > >{
  typedef sc::custom_reaction< EvCheckCellData > reactions;
  sc::result react( const EvCheckCellData & );
};

//STATES
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
struct CellStateChart_Life: sc::state<CellStateChart_Life,Running::orthogonal<4>,CellStateChart_Life_Living>{
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
struct CellStateChart_GLP1: sc::state<CellStateChart_GLP1,Running::orthogonal<3>,CellStateChart_GLP1_Unbound>{
  CellStateChart_GLP1(my_context ctx);

  typedef mpl::list< sc::custom_reaction< EvCellStateChart_GLP1Update >,
   sc::custom_reaction< EvGoToCellStateChart_GLP1_Unbound >,
   sc::custom_reaction< EvGoToCellStateChart_GLP1_Inactive >,
   sc::custom_reaction< EvGoToCellStateChart_GLP1_Active >,
   sc::custom_reaction< EvGoToCellStateChart_GLP1_Bound >,
   sc::custom_reaction< EvGoToCellStateChart_GLP1_Absent >   > reactions;

  sc::result react( const EvCellStateChart_GLP1Update & );
  sc::result react( const EvGoToCellStateChart_GLP1_Unbound & ){return transit<CellStateChart_GLP1_Unbound>();};
  sc::result react( const EvGoToCellStateChart_GLP1_Inactive & ){return transit<CellStateChart_GLP1_Inactive>();};
  sc::result react( const EvGoToCellStateChart_GLP1_Active & ){return transit<CellStateChart_GLP1_Active>();};
  sc::result react( const EvGoToCellStateChart_GLP1_Bound & ){return transit<CellStateChart_GLP1_Bound>();};
  sc::result react( const EvGoToCellStateChart_GLP1_Absent & ){return transit<CellStateChart_GLP1_Absent>();};
};

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
struct CellStateChart_LAG1: sc::state<CellStateChart_LAG1,Running::orthogonal<2>,CellStateChart_LAG1_Active>{
  CellStateChart_LAG1(my_context ctx);

  typedef mpl::list< sc::custom_reaction< EvCellStateChart_LAG1Update >,
   sc::custom_reaction< EvGoToCellStateChart_LAG1_Inactive >,
   sc::custom_reaction< EvGoToCellStateChart_LAG1_Active >   > reactions;

  sc::result react( const EvCellStateChart_LAG1Update & );
  sc::result react( const EvGoToCellStateChart_LAG1_Inactive & ){return transit<CellStateChart_LAG1_Inactive>();};
  sc::result react( const EvGoToCellStateChart_LAG1_Active & ){return transit<CellStateChart_LAG1_Active>();};
};

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
struct CellStateChart_GLD1: sc::state<CellStateChart_GLD1,Running::orthogonal<1>,CellStateChart_GLD1_Active>{
  CellStateChart_GLD1(my_context ctx);

  typedef mpl::list< sc::custom_reaction< EvCellStateChart_GLD1Update >,
   sc::custom_reaction< EvGoToCellStateChart_GLD1_Active >,
   sc::custom_reaction< EvGoToCellStateChart_GLD1_Inactive >   > reactions;

  sc::result react( const EvCellStateChart_GLD1Update & );
  sc::result react( const EvGoToCellStateChart_GLD1_Active & ){return transit<CellStateChart_GLD1_Active>();};
  sc::result react( const EvGoToCellStateChart_GLD1_Inactive & ){return transit<CellStateChart_GLD1_Inactive>();};
};

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
struct CellStateChart_CellCycle: sc::state<CellStateChart_CellCycle,Running::orthogonal<0>,CellStateChart_CellCycle_Mitosis>{
  CellStateChart_CellCycle(my_context ctx);

  typedef mpl::list< sc::custom_reaction< EvCellStateChart_CellCycleUpdate >,
   sc::custom_reaction< EvGoToCellStateChart_CellCycle_Mitosis_G1 >,
   sc::custom_reaction< EvGoToCellStateChart_CellCycle_Mitosis_G2 >,
   sc::custom_reaction< EvGoToCellStateChart_CellCycle_Mitosis_S >,
   sc::custom_reaction< EvGoToCellStateChart_CellCycle_Mitosis_M >,
   sc::custom_reaction< EvGoToCellStateChart_CellCycle_Meiosis >   > reactions;

  sc::result react( const EvCellStateChart_CellCycleUpdate & );
  sc::result react( const EvGoToCellStateChart_CellCycle_Mitosis_G1 & ){return transit<CellStateChart_CellCycle_Mitosis_G1>();};
  sc::result react( const EvGoToCellStateChart_CellCycle_Mitosis_G2 & ){return transit<CellStateChart_CellCycle_Mitosis_G2>();};
  sc::result react( const EvGoToCellStateChart_CellCycle_Mitosis_S & ){return transit<CellStateChart_CellCycle_Mitosis_S>();};
  sc::result react( const EvGoToCellStateChart_CellCycle_Mitosis_M & ){return transit<CellStateChart_CellCycle_Mitosis_M>();};
  sc::result react( const EvGoToCellStateChart_CellCycle_Meiosis & ){return transit<CellStateChart_CellCycle_Meiosis>();};
};

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
struct CellStateChart_CellCycle_Mitosis: sc::state<CellStateChart_CellCycle_Mitosis,CellStateChart_CellCycle,CellStateChart_CellCycle_Mitosis_G1>{
  CellStateChart_CellCycle_Mitosis(my_context ctx);

  typedef mpl::list< sc::custom_reaction< EvCellStateChart_CellCycleUpdate >   > reactions;

  sc::result react( const EvCellStateChart_CellCycleUpdate & );
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

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
struct CellStateChart_GLP1_Unbound: sc::state<CellStateChart_GLP1_Unbound,CellStateChart_GLP1 >{
  CellStateChart_GLP1_Unbound(my_context ctx);

  typedef mpl::list< sc::custom_reaction< EvCellStateChart_GLP1Update > > reactions;
  sc::result react( const EvCellStateChart_GLP1Update & );
};

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
struct CellStateChart_GLP1_Inactive: sc::state<CellStateChart_GLP1_Inactive,CellStateChart_GLP1 >{
  CellStateChart_GLP1_Inactive(my_context ctx);

  typedef mpl::list< sc::custom_reaction< EvCellStateChart_GLP1Update > > reactions;
  sc::result react( const EvCellStateChart_GLP1Update & );
};

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
struct CellStateChart_GLP1_Active: sc::state<CellStateChart_GLP1_Active,CellStateChart_GLP1 >{
  CellStateChart_GLP1_Active(my_context ctx);

  typedef mpl::list< sc::custom_reaction< EvCellStateChart_GLP1Update > > reactions;
  sc::result react( const EvCellStateChart_GLP1Update & );
};

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
struct CellStateChart_GLP1_Bound: sc::state<CellStateChart_GLP1_Bound,CellStateChart_GLP1 >{
  CellStateChart_GLP1_Bound(my_context ctx);

  typedef mpl::list< sc::custom_reaction< EvCellStateChart_GLP1Update > > reactions;
  sc::result react( const EvCellStateChart_GLP1Update & );
};

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
struct CellStateChart_GLP1_Absent: sc::state<CellStateChart_GLP1_Absent,CellStateChart_GLP1 >{
  CellStateChart_GLP1_Absent(my_context ctx);

  typedef mpl::list< sc::custom_reaction< EvCellStateChart_GLP1Update > > reactions;
  sc::result react( const EvCellStateChart_GLP1Update & );
};

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
struct CellStateChart_LAG1_Inactive: sc::state<CellStateChart_LAG1_Inactive,CellStateChart_LAG1 >{
  CellStateChart_LAG1_Inactive(my_context ctx);

  typedef mpl::list< sc::custom_reaction< EvCellStateChart_LAG1Update > > reactions;
  sc::result react( const EvCellStateChart_LAG1Update & );
};

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
struct CellStateChart_LAG1_Active: sc::state<CellStateChart_LAG1_Active,CellStateChart_LAG1 >{
  CellStateChart_LAG1_Active(my_context ctx);

  typedef mpl::list< sc::custom_reaction< EvCellStateChart_LAG1Update > > reactions;
  sc::result react( const EvCellStateChart_LAG1Update & );
};

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
struct CellStateChart_GLD1_Active: sc::state<CellStateChart_GLD1_Active,CellStateChart_GLD1 >{
  CellStateChart_GLD1_Active(my_context ctx);

  typedef mpl::list< sc::custom_reaction< EvCellStateChart_GLD1Update > > reactions;
  sc::result react( const EvCellStateChart_GLD1Update & );
};

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
struct CellStateChart_GLD1_Inactive: sc::state<CellStateChart_GLD1_Inactive,CellStateChart_GLD1 >{
  CellStateChart_GLD1_Inactive(my_context ctx);

  typedef mpl::list< sc::custom_reaction< EvCellStateChart_GLD1Update > > reactions;
  sc::result react( const EvCellStateChart_GLD1Update & );
};

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
struct CellStateChart_CellCycle_Mitosis_G1: sc::state<CellStateChart_CellCycle_Mitosis_G1,CellStateChart_CellCycle_Mitosis >{
  double Duration;
  CellStateChart_CellCycle_Mitosis_G1(my_context ctx);

  typedef sc::custom_reaction< EvCellStateChart_CellCycleUpdate> reactions;
  sc::result react( const EvCellStateChart_CellCycleUpdate & );
};

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
struct CellStateChart_CellCycle_Mitosis_G2: sc::state<CellStateChart_CellCycle_Mitosis_G2,CellStateChart_CellCycle_Mitosis >{
  double Duration;
  CellStateChart_CellCycle_Mitosis_G2(my_context ctx);

  typedef sc::custom_reaction< EvCellStateChart_CellCycleUpdate> reactions;
  sc::result react( const EvCellStateChart_CellCycleUpdate & );
};

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
struct CellStateChart_CellCycle_Mitosis_S: sc::state<CellStateChart_CellCycle_Mitosis_S,CellStateChart_CellCycle_Mitosis >{
  double Duration;
  CellStateChart_CellCycle_Mitosis_S(my_context ctx);

  typedef sc::custom_reaction< EvCellStateChart_CellCycleUpdate> reactions;
  sc::result react( const EvCellStateChart_CellCycleUpdate & );
};

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
struct CellStateChart_CellCycle_Mitosis_M: sc::state<CellStateChart_CellCycle_Mitosis_M,CellStateChart_CellCycle_Mitosis >{
  double Duration;
  CellStateChart_CellCycle_Mitosis_M(my_context ctx);

  typedef sc::custom_reaction< EvCellStateChart_CellCycleUpdate> reactions;
  sc::result react( const EvCellStateChart_CellCycleUpdate & );
};

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
struct CellStateChart_CellCycle_Meiosis: sc::state<CellStateChart_CellCycle_Meiosis,CellStateChart_CellCycle >{
  CellStateChart_CellCycle_Meiosis(my_context ctx);

  typedef mpl::list< sc::custom_reaction< EvCellStateChart_CellCycleUpdate > > reactions;
  sc::result react( const EvCellStateChart_CellCycleUpdate & );
};

#endif
