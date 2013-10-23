#include <StatechartInterface.hpp>
#include <BasicStatechart.hpp>

//--------------------STATECHART FUNCTIONS------------------------------

CellStatechart::CellStatechart(){
        pCell=boost::shared_ptr<Cell>();
        TimeInPhase=0;
};

void CellStatechart::SetCell(CellPtr newCell){
     assert(newCell!=NULL);
     pCell=newCell;
};

std::vector<double> CellStatechart::GetVariables(){
    std::vector<double> variables;
    variables.push_back(TimeInPhase);
    return variables;
}
void CellStatechart::SetVariables(std::vector<double> variables){
    TimeInPhase=variables.at(0);
}

int CellStatechart::GetState(){
  int state=1;
 if(state_cast<const CellStateChart_Life_Living*>()!=0){ 
     state=(state<<1)+1;
 }else{state=(state<<1);}
 if(state_cast<const CellStateChart_Life_Dead*>()!=0){ 
     state=(state<<1)+1;
 }else{state=(state<<1);}
 if(state_cast<const CellStateChart_GLP1_Unbound*>()!=0){ 
     state=(state<<1)+1;
 }else{state=(state<<1);}
 if(state_cast<const CellStateChart_GLP1_Inactive*>()!=0){ 
     state=(state<<1)+1;
 }else{state=(state<<1);}
 if(state_cast<const CellStateChart_GLP1_Active*>()!=0){ 
     state=(state<<1)+1;
 }else{state=(state<<1);}
 if(state_cast<const CellStateChart_GLP1_Bound*>()!=0){ 
     state=(state<<1)+1;
 }else{state=(state<<1);}
 if(state_cast<const CellStateChart_GLP1_Absent*>()!=0){ 
     state=(state<<1)+1;
 }else{state=(state<<1);}
 if(state_cast<const CellStateChart_LAG1_Inactive*>()!=0){ 
     state=(state<<1)+1;
 }else{state=(state<<1);}
 if(state_cast<const CellStateChart_LAG1_Active*>()!=0){ 
     state=(state<<1)+1;
 }else{state=(state<<1);}
 if(state_cast<const CellStateChart_GLD1_Active*>()!=0){ 
     state=(state<<1)+1;
 }else{state=(state<<1);}
 if(state_cast<const CellStateChart_GLD1_Inactive*>()!=0){ 
     state=(state<<1)+1;
 }else{state=(state<<1);}
 if(state_cast<const CellStateChart_CellCycle_Mitosis_G1*>()!=0){ 
     state=(state<<1)+1;
 }else{state=(state<<1);}
 if(state_cast<const CellStateChart_CellCycle_Mitosis_G2*>()!=0){ 
     state=(state<<1)+1;
 }else{state=(state<<1);}
 if(state_cast<const CellStateChart_CellCycle_Mitosis_S*>()!=0){ 
     state=(state<<1)+1;
 }else{state=(state<<1);}
 if(state_cast<const CellStateChart_CellCycle_Mitosis_M*>()!=0){ 
     state=(state<<1)+1;
 }else{state=(state<<1);}
 if(state_cast<const CellStateChart_CellCycle_Meiosis*>()!=0){ 
     state=(state<<1)+1;
 }else{state=(state<<1);}
return state;

}

void CellStatechart::SetState(int state){
 if((state&1)/1){ 
     process_event(EvGoToCellStateChart_CellCycle_Meiosis());;
 }
 if((state&2)/2){ 
     process_event(EvGoToCellStateChart_CellCycle_Mitosis_M());;
 }
 if((state&4)/4){ 
     process_event(EvGoToCellStateChart_CellCycle_Mitosis_S());;
 }
 if((state&8)/8){ 
     process_event(EvGoToCellStateChart_CellCycle_Mitosis_G2());;
 }
 if((state&16)/16){ 
     process_event(EvGoToCellStateChart_CellCycle_Mitosis_G1());;
 }
 if((state&32)/32){ 
     process_event(EvGoToCellStateChart_GLD1_Inactive());;
 }
 if((state&64)/64){ 
     process_event(EvGoToCellStateChart_GLD1_Active());;
 }
 if((state&128)/128){ 
     process_event(EvGoToCellStateChart_LAG1_Active());;
 }
 if((state&256)/256){ 
     process_event(EvGoToCellStateChart_LAG1_Inactive());;
 }
 if((state&512)/512){ 
     process_event(EvGoToCellStateChart_GLP1_Absent());;
 }
 if((state&1024)/1024){ 
     process_event(EvGoToCellStateChart_GLP1_Bound());;
 }
 if((state&2048)/2048){ 
     process_event(EvGoToCellStateChart_GLP1_Active());;
 }
 if((state&4096)/4096){ 
     process_event(EvGoToCellStateChart_GLP1_Inactive());;
 }
 if((state&8192)/8192){ 
     process_event(EvGoToCellStateChart_GLP1_Unbound());;
 }
 if((state&16384)/16384){ 
     process_event(EvGoToCellStateChart_Life_Dead());;
 }
 if((state&32768)/32768){ 
     process_event(EvGoToCellStateChart_Life_Living());;
 }
}

boost::shared_ptr<CellStatechart> CellStatechart::Copy(boost::shared_ptr<CellStatechart> myNewStatechart){
    myNewStatechart->initiate();
    if(state_cast<const CellStateChart_Life_Living*>()!=0){
        myNewStatechart->process_event(EvGoToCellStateChart_Life_Living());
    }
    if(state_cast<const CellStateChart_Life_Dead*>()!=0){
        myNewStatechart->process_event(EvGoToCellStateChart_Life_Dead());
    }
    if(state_cast<const CellStateChart_GLP1_Unbound*>()!=0){
        myNewStatechart->process_event(EvGoToCellStateChart_GLP1_Unbound());
    }
    if(state_cast<const CellStateChart_GLP1_Inactive*>()!=0){
        myNewStatechart->process_event(EvGoToCellStateChart_GLP1_Inactive());
    }
    if(state_cast<const CellStateChart_GLP1_Active*>()!=0){
        myNewStatechart->process_event(EvGoToCellStateChart_GLP1_Active());
    }
    if(state_cast<const CellStateChart_GLP1_Bound*>()!=0){
        myNewStatechart->process_event(EvGoToCellStateChart_GLP1_Bound());
    }
    if(state_cast<const CellStateChart_GLP1_Absent*>()!=0){
        myNewStatechart->process_event(EvGoToCellStateChart_GLP1_Absent());
    }
    if(state_cast<const CellStateChart_LAG1_Inactive*>()!=0){
        myNewStatechart->process_event(EvGoToCellStateChart_LAG1_Inactive());
    }
    if(state_cast<const CellStateChart_LAG1_Active*>()!=0){
        myNewStatechart->process_event(EvGoToCellStateChart_LAG1_Active());
    }
    if(state_cast<const CellStateChart_GLD1_Active*>()!=0){
        myNewStatechart->process_event(EvGoToCellStateChart_GLD1_Active());
    }
    if(state_cast<const CellStateChart_GLD1_Inactive*>()!=0){
        myNewStatechart->process_event(EvGoToCellStateChart_GLD1_Inactive());
    }
    if(state_cast<const CellStateChart_CellCycle_Mitosis_G1*>()!=0){
        myNewStatechart->process_event(EvGoToCellStateChart_CellCycle_Mitosis_G1());
    }
    if(state_cast<const CellStateChart_CellCycle_Mitosis_G2*>()!=0){
        myNewStatechart->process_event(EvGoToCellStateChart_CellCycle_Mitosis_G2());
    }
    if(state_cast<const CellStateChart_CellCycle_Mitosis_S*>()!=0){
        myNewStatechart->process_event(EvGoToCellStateChart_CellCycle_Mitosis_S());
    }
    if(state_cast<const CellStateChart_CellCycle_Mitosis_M*>()!=0){
        myNewStatechart->process_event(EvGoToCellStateChart_CellCycle_Mitosis_M());
    }
    if(state_cast<const CellStateChart_CellCycle_Meiosis*>()!=0){
        myNewStatechart->process_event(EvGoToCellStateChart_CellCycle_Meiosis());
    }
    return (myNewStatechart);
};

//--------------------FIRST RESPONDER------------------------------
sc::result Running::react( const EvCheckCellData & ){
    post_event(EvCellStateChart_CellCycleUpdate());
    post_event(EvCellStateChart_GLD1Update());
    post_event(EvCellStateChart_LAG1Update());
    post_event(EvCellStateChart_GLP1Update());
    post_event(EvCellStateChart_LifeUpdate());
    return discard_event();
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
CellStateChart_GLP1::CellStateChart_GLP1( my_context ctx ):my_base( ctx ){};

sc::result CellStateChart_GLP1::react( const EvCellStateChart_GLP1Update & ){
    CellPtr myCell=context<CellStatechart>().pCell;
    return discard_event();
};
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
CellStateChart_LAG1::CellStateChart_LAG1( my_context ctx ):my_base( ctx ){};

sc::result CellStateChart_LAG1::react( const EvCellStateChart_LAG1Update & ){
    CellPtr myCell=context<CellStatechart>().pCell;
    return discard_event();
};
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
CellStateChart_GLD1::CellStateChart_GLD1( my_context ctx ):my_base( ctx ){};

sc::result CellStateChart_GLD1::react( const EvCellStateChart_GLD1Update & ){
    CellPtr myCell=context<CellStatechart>().pCell;
    return discard_event();
};
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
CellStateChart_CellCycle::CellStateChart_CellCycle( my_context ctx ):my_base( ctx ){};

sc::result CellStateChart_CellCycle::react( const EvCellStateChart_CellCycleUpdate & ){
    CellPtr myCell=context<CellStatechart>().pCell;
    return discard_event();
};
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
CellStateChart_CellCycle_Mitosis::CellStateChart_CellCycle_Mitosis( my_context ctx ):my_base( ctx ){};

sc::result CellStateChart_CellCycle_Mitosis::react( const EvCellStateChart_CellCycleUpdate & ){
    CellPtr myCell=context<CellStatechart>().pCell;
    if(state_cast<const CellStateChart_GLD1_Active*>()!=0 && GetTime()>1){
        return transit<CellStateChart_CellCycle_Meiosis>();
    }
    return forward_event();
};
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
CellStateChart_Life_Living::CellStateChart_Life_Living( my_context ctx ):
my_base( ctx ){};

sc::result CellStateChart_Life_Living::react( const EvCellStateChart_LifeUpdate & ){
    CellPtr myCell=context<CellStatechart>().pCell;

    if(IsDead(myCell)==true){
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
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
CellStateChart_GLP1_Unbound::CellStateChart_GLP1_Unbound( my_context ctx ):
my_base( ctx ){};

sc::result CellStateChart_GLP1_Unbound::react( const EvCellStateChart_GLP1Update & ){
    CellPtr myCell=context<CellStatechart>().pCell;

    if(GetDistanceFromDTC(myCell)<15){
        return transit<CellStateChart_GLP1_Bound>();
    }
    return forward_event();
};
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
CellStateChart_GLP1_Inactive::CellStateChart_GLP1_Inactive( my_context ctx ):
my_base( ctx ){};

sc::result CellStateChart_GLP1_Inactive::react( const EvCellStateChart_GLP1Update & ){
    CellPtr myCell=context<CellStatechart>().pCell;

    if(state_cast<const CellStateChart_GLP1_Inactive*>()!=0){
        return transit<CellStateChart_GLP1_Active>();
    }
    return forward_event();
};
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
CellStateChart_GLP1_Active::CellStateChart_GLP1_Active( my_context ctx ):
my_base( ctx ){};

sc::result CellStateChart_GLP1_Active::react( const EvCellStateChart_GLP1Update & ){
    CellPtr myCell=context<CellStatechart>().pCell;

    if(GetDistanceFromDTC(myCell)>100){
        return transit<CellStateChart_GLP1_Absent>();
    }
    return forward_event();
};
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
CellStateChart_GLP1_Bound::CellStateChart_GLP1_Bound( my_context ctx ):
my_base( ctx ){};

sc::result CellStateChart_GLP1_Bound::react( const EvCellStateChart_GLP1Update & ){
    CellPtr myCell=context<CellStatechart>().pCell;

    if(state_cast<const CellStateChart_GLP1_Bound*>()!=0){
        return transit<CellStateChart_GLP1_Inactive>();
    }
    return forward_event();
};
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
CellStateChart_GLP1_Absent::CellStateChart_GLP1_Absent( my_context ctx ):
my_base( ctx ){};

sc::result CellStateChart_GLP1_Absent::react( const EvCellStateChart_GLP1Update & ){
    CellPtr myCell=context<CellStatechart>().pCell;

    return forward_event();
};
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
CellStateChart_LAG1_Inactive::CellStateChart_LAG1_Inactive( my_context ctx ):
my_base( ctx ){};

sc::result CellStateChart_LAG1_Inactive::react( const EvCellStateChart_LAG1Update & ){
    CellPtr myCell=context<CellStatechart>().pCell;

    if(state_cast<const CellStateChart_GLP1_Active*>()!=0){
        return transit<CellStateChart_LAG1_Active>();
    }
    return forward_event();
};
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
CellStateChart_LAG1_Active::CellStateChart_LAG1_Active( my_context ctx ):
my_base( ctx ){};

sc::result CellStateChart_LAG1_Active::react( const EvCellStateChart_LAG1Update & ){
    CellPtr myCell=context<CellStatechart>().pCell;

    if(state_cast<const CellStateChart_GLP1_Active*>()==0){
        return transit<CellStateChart_LAG1_Inactive>();
    }
    return forward_event();
};
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
CellStateChart_GLD1_Active::CellStateChart_GLD1_Active( my_context ctx ):
my_base( ctx ){};

sc::result CellStateChart_GLD1_Active::react( const EvCellStateChart_GLD1Update & ){
    CellPtr myCell=context<CellStatechart>().pCell;

    if(state_cast<const CellStateChart_LAG1_Active*>()!=0){
        return transit<CellStateChart_GLD1_Inactive>();
    }
    return forward_event();
};
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
CellStateChart_GLD1_Inactive::CellStateChart_GLD1_Inactive( my_context ctx ):
my_base( ctx ){};

sc::result CellStateChart_GLD1_Inactive::react( const EvCellStateChart_GLD1Update & ){
    CellPtr myCell=context<CellStatechart>().pCell;

    if(state_cast<const CellStateChart_LAG1_Inactive*>()!=0){
        return transit<CellStateChart_GLD1_Active>();
    }
    return forward_event();
};
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
CellStateChart_CellCycle_Mitosis_G1::CellStateChart_CellCycle_Mitosis_G1( my_context ctx ):
my_base( ctx ){ context<CellStatechart>().TimeInPhase = 0.0;
 CellPtr myCell=context<CellStatechart>().pCell;
 RandomNumberGenerator* p_gen = RandomNumberGenerator::Instance();
 Duration=p_gen->NormalRandomDeviate(GetG2Duration(myCell),0.1*GetG2Duration(myCell));
 SetCellCyclePhase(myCell,G_ONE_PHASE);
}

sc::result CellStateChart_CellCycle_Mitosis_G1::react( const EvCellStateChart_CellCycleUpdate & ){
    CellPtr myCell=context<CellStatechart>().pCell;

  context<CellStatechart>().TimeInPhase+=GetTimestep();
    if(context<CellStatechart>().TimeInPhase>=Duration){
        return transit<CellStateChart_CellCycle_Mitosis_S>();
    }
    return forward_event();
};
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
CellStateChart_CellCycle_Mitosis_G2::CellStateChart_CellCycle_Mitosis_G2( my_context ctx ):
my_base( ctx ){ context<CellStatechart>().TimeInPhase = 0.0;
 CellPtr myCell=context<CellStatechart>().pCell;
 RandomNumberGenerator* p_gen = RandomNumberGenerator::Instance();
 Duration=p_gen->NormalRandomDeviate(GetG2Duration(myCell),0.1*GetG2Duration(myCell));
 SetCellCyclePhase(myCell,G_TWO_PHASE);
}

sc::result CellStateChart_CellCycle_Mitosis_G2::react( const EvCellStateChart_CellCycleUpdate & ){
    CellPtr myCell=context<CellStatechart>().pCell;

  context<CellStatechart>().TimeInPhase+=GetTimestep();
    if(context<CellStatechart>().TimeInPhase>=Duration){
        SetReadyToDivide(myCell,true);
        return transit<CellStateChart_CellCycle_Mitosis_M>();
    }
    return forward_event();
};
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
CellStateChart_CellCycle_Mitosis_S::CellStateChart_CellCycle_Mitosis_S( my_context ctx ):
my_base( ctx ){ context<CellStatechart>().TimeInPhase = 0.0;
 CellPtr myCell=context<CellStatechart>().pCell;
 RandomNumberGenerator* p_gen = RandomNumberGenerator::Instance();
 Duration=p_gen->NormalRandomDeviate(GetG2Duration(myCell),0.1*GetG2Duration(myCell));
 SetCellCyclePhase(myCell,S_PHASE);
}

sc::result CellStateChart_CellCycle_Mitosis_S::react( const EvCellStateChart_CellCycleUpdate & ){
    CellPtr myCell=context<CellStatechart>().pCell;

  context<CellStatechart>().TimeInPhase+=GetTimestep();
    if(context<CellStatechart>().TimeInPhase>=Duration){
        return transit<CellStateChart_CellCycle_Mitosis_G2>();
    }
    return forward_event();
};
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
CellStateChart_CellCycle_Mitosis_M::CellStateChart_CellCycle_Mitosis_M( my_context ctx ):
my_base( ctx ){ context<CellStatechart>().TimeInPhase = 0.0;
 CellPtr myCell=context<CellStatechart>().pCell;
 RandomNumberGenerator* p_gen = RandomNumberGenerator::Instance();
 Duration=p_gen->NormalRandomDeviate(GetG2Duration(myCell),0.1*GetG2Duration(myCell));
 SetCellCyclePhase(myCell,M_PHASE);
}

sc::result CellStateChart_CellCycle_Mitosis_M::react( const EvCellStateChart_CellCycleUpdate & ){
    CellPtr myCell=context<CellStatechart>().pCell;

  context<CellStatechart>().TimeInPhase+=GetTimestep();
    if(context<CellStatechart>().TimeInPhase>=Duration){
        return transit<CellStateChart_CellCycle_Mitosis_G1>();
    }
    return forward_event();
};
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
CellStateChart_CellCycle_Meiosis::CellStateChart_CellCycle_Meiosis( my_context ctx ):
my_base( ctx ){
    CellPtr myCell=context<CellStatechart>().pCell;
    SetProliferationFlag(myCell,0.0);
}
sc::result CellStateChart_CellCycle_Meiosis::react( const EvCellStateChart_CellCycleUpdate & ){
    CellPtr myCell=context<CellStatechart>().pCell;

    UpdateRadius(myCell);
    return forward_event();
};
