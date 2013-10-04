#include <StatechartChasteInterface.hpp>
#include <Statechart.hpp>

CellStatechart::CellStatechart(CellPtr myCell):
        pCell(myCell){
        assert(pCell!=NULL);
        initiate(); //calls the constructors of initial states
};

void CellStatechart::SetCell(CellPtr newCell){
     assert(newCell!=NULL);
     pCell=newCell;
};

boost::shared_ptr<CellStatechart> CellStatechart::Copy(CellPtr myCell){
    assert(myCell!=NULL);
    MAKE_PTR_ARGS(CellStatechart,myNewStatechart,(myCell));
    if(state_cast<const CellStateChart_Membrane_GLP1_Unbound*>()!=0){
        myNewStatechart->process_event(EvGoToCellStateChart_Membrane_GLP1_Unbound());
    }
    if(state_cast<const CellStateChart_Membrane_GLP1_Bound*>()!=0){
        myNewStatechart->process_event(EvGoToCellStateChart_Membrane_GLP1_Bound());
    }
    if(state_cast<const CellStateChart_Membrane_GLP1_Inactive*>()!=0){
        myNewStatechart->process_event(EvGoToCellStateChart_Membrane_GLP1_Inactive());
    }
    if(state_cast<const CellStateChart_Membrane_GLP1_Active*>()!=0){
        myNewStatechart->process_event(EvGoToCellStateChart_Membrane_GLP1_Active());
    }
    if(state_cast<const CellStateChart_Membrane_GLP1_Absent*>()!=0){
        myNewStatechart->process_event(EvGoToCellStateChart_Membrane_GLP1_Absent());
    }
    if(state_cast<const CellStateChart_CytoplasmLAG1_Active*>()!=0){
        myNewStatechart->process_event(EvGoToCellStateChart_CytoplasmLAG1_Active());
    }
    if(state_cast<const CellStateChart_CytoplasmLAG1_Inactive*>()!=0){
        myNewStatechart->process_event(EvGoToCellStateChart_CytoplasmLAG1_Inactive());
    }
    if(state_cast<const CellStateChart_CytoplasmGLD1_Active*>()!=0){
        myNewStatechart->process_event(EvGoToCellStateChart_CytoplasmGLD1_Active());
    }
    if(state_cast<const CellStateChart_CytoplasmGLD1_Inactive*>()!=0){
        myNewStatechart->process_event(EvGoToCellStateChart_CytoplasmGLD1_Inactive());
    }
    if(state_cast<const CellStateChart_Abstract_Mitosis_G_ONE*>()!=0){
        myNewStatechart->process_event(EvGoToCellStateChart_Abstract_Mitosis_G_ONE());
    }
    if(state_cast<const CellStateChart_Abstract_Mitosis_S*>()!=0){
        myNewStatechart->process_event(EvGoToCellStateChart_Abstract_Mitosis_S());
    }
    if(state_cast<const CellStateChart_Abstract_Mitosis_G_TWO*>()!=0){
        myNewStatechart->process_event(EvGoToCellStateChart_Abstract_Mitosis_G_TWO());
    }
    if(state_cast<const CellStateChart_Abstract_Mitosis_M*>()!=0){
        myNewStatechart->process_event(EvGoToCellStateChart_Abstract_Mitosis_M());
    }
    if(state_cast<const CellStateChart_Abstract_Meiosis*>()!=0){
        myNewStatechart->process_event(EvGoToCellStateChart_Abstract_Meiosis());
    }
    if(state_cast<const CellStateChart_Life_Living*>()!=0){
        myNewStatechart->process_event(EvGoToCellStateChart_Life_Living());
    }
    if(state_cast<const CellStateChart_Life_Dead*>()!=0){
        myNewStatechart->process_event(EvGoToCellStateChart_Life_Dead());
    }
    return (myNewStatechart);
};

sc::result Running::react( const EvCheckCellData & ){
    post_event(EvCellStateChart_LifeUpdate());
    post_event(EvCellStateChart_AbstractUpdate());
    post_event(EvCellStateChart_CytoplasmGLD1Update());
    post_event(EvCellStateChart_CytoplasmLAG1Update());
    post_event(EvCellStateChart_MembraneUpdate());
    return discard_event();
};
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
CellStateChart_Membrane::CellStateChart_Membrane( my_context ctx ):my_base( ctx ){};

sc::result CellStateChart_Membrane::react( const EvCellStateChart_MembraneUpdate & ){
    CellPtr myCell=context<CellStatechart>().pCell;
    return discard_event();
};
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
CellStateChart_Membrane_GLP1::CellStateChart_Membrane_GLP1( my_context ctx ):my_base( ctx ){};

sc::result CellStateChart_Membrane_GLP1::react( const EvCellStateChart_MembraneUpdate & ){
    CellPtr myCell=context<CellStatechart>().pCell;
    return forward_event();
};
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
CellStateChart_CytoplasmLAG1::CellStateChart_CytoplasmLAG1( my_context ctx ):my_base( ctx ){};

sc::result CellStateChart_CytoplasmLAG1::react( const EvCellStateChart_CytoplasmLAG1Update & ){
    CellPtr myCell=context<CellStatechart>().pCell;
    return discard_event();
};
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
CellStateChart_CytoplasmGLD1::CellStateChart_CytoplasmGLD1( my_context ctx ):my_base( ctx ){};

sc::result CellStateChart_CytoplasmGLD1::react( const EvCellStateChart_CytoplasmGLD1Update & ){
    CellPtr myCell=context<CellStatechart>().pCell;
    return discard_event();
};
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
CellStateChart_Abstract::CellStateChart_Abstract( my_context ctx ):my_base( ctx ){};

sc::result CellStateChart_Abstract::react( const EvCellStateChart_AbstractUpdate & ){
    CellPtr myCell=context<CellStatechart>().pCell;
    return discard_event();
};
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
CellStateChart_Abstract_Mitosis::CellStateChart_Abstract_Mitosis( my_context ctx ):my_base( ctx ){};

sc::result CellStateChart_Abstract_Mitosis::react( const EvCellStateChart_AbstractUpdate & ){
    CellPtr myCell=context<CellStatechart>().pCell;
    if(/*CytoplasmGLD1.Active & ChasteTime > 10*/){
        return transit<CellStateChart_Abstract_Meiosis>();
    }
    return forward_event();
};
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
CellStateChart_Life::CellStateChart_Life( my_context ctx ):my_base( ctx ){};

sc::result CellStateChart_Life::react( const EvCellStateChart_LifeUpdate & ){
    CellPtr myCell=context<CellStatechart>().pCell;
    return discard_event();
};
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
CellStateChart_Membrane_GLP1_Unbound::CellStateChart_Membrane_GLP1_Unbound( my_context ctx ):
my_base( ctx ){};

sc::result CellStateChart_Membrane_GLP1_Unbound::react( const EvCellStateChart_MembraneUpdate & ){
    CellPtr myCell=context<CellStatechart>().pCell;

    if(/*distanceCondition*/){
        return transit<CellStateChart_Membrane_GLP1_Bound>();
    }
    return forward_event();
};
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
CellStateChart_Membrane_GLP1_Bound::CellStateChart_Membrane_GLP1_Bound( my_context ctx ):
my_base( ctx ){};

sc::result CellStateChart_Membrane_GLP1_Bound::react( const EvCellStateChart_MembraneUpdate & ){
    CellPtr myCell=context<CellStatechart>().pCell;

        return transit<CellStateChart_Membrane_GLP1_Inactive>();
    return forward_event();
};
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
CellStateChart_Membrane_GLP1_Inactive::CellStateChart_Membrane_GLP1_Inactive( my_context ctx ):
my_base( ctx ){};

sc::result CellStateChart_Membrane_GLP1_Inactive::react( const EvCellStateChart_MembraneUpdate & ){
    CellPtr myCell=context<CellStatechart>().pCell;

        return transit<CellStateChart_Membrane_GLP1_Active>();
    return forward_event();
};
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
CellStateChart_Membrane_GLP1_Active::CellStateChart_Membrane_GLP1_Active( my_context ctx ):
my_base( ctx ){};

sc::result CellStateChart_Membrane_GLP1_Active::react( const EvCellStateChart_MembraneUpdate & ){
    CellPtr myCell=context<CellStatechart>().pCell;

    if(/*distanceCondition*/){
        return transit<CellStateChart_Membrane_GLP1_Absent>();
    }
    return forward_event();
};
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
CellStateChart_Membrane_GLP1_Absent::CellStateChart_Membrane_GLP1_Absent( my_context ctx ):
my_base( ctx ){};

sc::result CellStateChart_Membrane_GLP1_Absent::react( const EvCellStateChart_MembraneUpdate & ){
    CellPtr myCell=context<CellStatechart>().pCell;

    return forward_event();
};
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
CellStateChart_CytoplasmLAG1_Active::CellStateChart_CytoplasmLAG1_Active( my_context ctx ):
my_base( ctx ){};

sc::result CellStateChart_CytoplasmLAG1_Active::react( const EvCellStateChart_CytoplasmLAG1Update & ){
    CellPtr myCell=context<CellStatechart>().pCell;

    if(/*!Membrane.GLP1.Active*/){
        return transit<CellStateChart_CytoplasmLAG1_Inactive>();
    }
    return forward_event();
};
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
CellStateChart_CytoplasmLAG1_Inactive::CellStateChart_CytoplasmLAG1_Inactive( my_context ctx ):
my_base( ctx ){};

sc::result CellStateChart_CytoplasmLAG1_Inactive::react( const EvCellStateChart_CytoplasmLAG1Update & ){
    CellPtr myCell=context<CellStatechart>().pCell;

    if(/*Membrane.GLP1.Active*/){
        return transit<CellStateChart_CytoplasmLAG1_Active>();
    }
    return forward_event();
};
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
CellStateChart_CytoplasmGLD1_Active::CellStateChart_CytoplasmGLD1_Active( my_context ctx ):
my_base( ctx ){};

sc::result CellStateChart_CytoplasmGLD1_Active::react( const EvCellStateChart_CytoplasmGLD1Update & ){
    CellPtr myCell=context<CellStatechart>().pCell;

    if(/*CytoplasmLAG1.Active*/){
        return transit<CellStateChart_CytoplasmGLD1_Inactive>();
    }
    return forward_event();
};
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
CellStateChart_CytoplasmGLD1_Inactive::CellStateChart_CytoplasmGLD1_Inactive( my_context ctx ):
my_base( ctx ){};

sc::result CellStateChart_CytoplasmGLD1_Inactive::react( const EvCellStateChart_CytoplasmGLD1Update & ){
    CellPtr myCell=context<CellStatechart>().pCell;

    if(/*CytoplasmLAG1.Inactive*/){
        return transit<CellStateChart_CytoplasmGLD1_Active>();
    }
    return forward_event();
};
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
CellStateChart_Abstract_Mitosis_G_ONE::CellStateChart_Abstract_Mitosis_G_ONE( my_context ctx ):
my_base( ctx ),TimeInThisState(0.0){
SetCellCyclePhase(context<CellStatechart>().pCell,G_ONE_PHASE);
}

sc::result CellStateChart_Abstract_Mitosis_G_ONE::react( const EvCellStateChart_AbstractUpdate & ){
    CellPtr myCell=context<CellStatechart>().pCell;

    TimeInThisState+=GetTimestep(myCell);
    if(TimeInThisState>= GetG1Duration(myCell)){
        return transit<CellStateChart_Abstract_Mitosis_S>();
    }
    return forward_event();
};
sc::result CellStateChart_Abstract_Mitosis_G_ONE::react( const EvGoToTime & e){
    TimeInThisState=e.time;
    return discard_event();
};
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
CellStateChart_Abstract_Mitosis_S::CellStateChart_Abstract_Mitosis_S( my_context ctx ):
my_base( ctx ),TimeInThisState(0.0){
SetCellCyclePhase(context<CellStatechart>().pCell,S_PHASE);
}

sc::result CellStateChart_Abstract_Mitosis_S::react( const EvCellStateChart_AbstractUpdate & ){
    CellPtr myCell=context<CellStatechart>().pCell;

    TimeInThisState+=GetTimestep(myCell);
    if(TimeInThisState>= GetSDuration(myCell)){
        return transit<CellStateChart_Abstract_Mitosis_G_TWO>();
    }
    return forward_event();
};
sc::result CellStateChart_Abstract_Mitosis_S::react( const EvGoToTime & e){
    TimeInThisState=e.time;
    return discard_event();
};
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
CellStateChart_Abstract_Mitosis_G_TWO::CellStateChart_Abstract_Mitosis_G_TWO( my_context ctx ):
my_base( ctx ),TimeInThisState(0.0){
SetCellCyclePhase(context<CellStatechart>().pCell,G_TWO_PHASE);
}

sc::result CellStateChart_Abstract_Mitosis_G_TWO::react( const EvCellStateChart_AbstractUpdate & ){
    CellPtr myCell=context<CellStatechart>().pCell;

    TimeInThisState+=GetTimestep(myCell);
    if(TimeInThisState>= GetG2Duration(myCell)){
        SetReadyToDivide(myCell,true);
        return transit<CellStateChart_Abstract_Mitosis_M>();
    }
    return forward_event();
};
sc::result CellStateChart_Abstract_Mitosis_G_TWO::react( const EvGoToTime & e){
    TimeInThisState=e.time;
    return discard_event();
};
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
CellStateChart_Abstract_Mitosis_M::CellStateChart_Abstract_Mitosis_M( my_context ctx ):
my_base( ctx ),TimeInThisState(0.0){
SetCellCyclePhase(context<CellStatechart>().pCell,M_PHASE);
}

sc::result CellStateChart_Abstract_Mitosis_M::react( const EvCellStateChart_AbstractUpdate & ){
    CellPtr myCell=context<CellStatechart>().pCell;

    TimeInThisState+=GetTimestep(myCell);
    if(TimeInThisState>= GetMDuration(myCell)){
        return transit<CellStateChart_Abstract_Mitosis_G_ONE>();
    }
    return forward_event();
};
sc::result CellStateChart_Abstract_Mitosis_M::react( const EvGoToTime & e){
    TimeInThisState=e.time;
    return discard_event();
};
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
CellStateChart_Abstract_Meiosis::CellStateChart_Abstract_Meiosis( my_context ctx ):
my_base( ctx ),radius(2.5){
    CellPtr myCell=context<CellStatechart>().pCell;
    SetProliferationFlag(myCell,0.0);
}
sc::result CellStateChart_Abstract_Meiosis::react( const EvCellStateChart_AbstractUpdate & ){
    CellPtr myCell=context<CellStatechart>().pCell;

    radius+=0.4*GetTimestep(myCell);
    return forward_event();
};
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
CellStateChart_Life_Living::CellStateChart_Life_Living( my_context ctx ):
my_base( ctx ){};

sc::result CellStateChart_Life_Living::react( const EvCellStateChart_LifeUpdate & ){
    CellPtr myCell=context<CellStatechart>().pCell;

    if(/*ChasteIsDead*/){
        return transit<CellStateChart_Life_Dead>();
    }
    return forward_event();
};
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
CellStateChart_Life_Dead::CellStateChart_Life_Dead( my_context ctx ):
my_base( ctx ){};

sc::result CellStateChart_Life_Dead::react( const EvCellStateChart_LifeUpdate & ){
    CellPtr myCell=context<CellStatechart>().pCell;

    return forward_event();
};
