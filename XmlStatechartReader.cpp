#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/validators/common/Grammar.hpp>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <math.h>

using namespace std;
using namespace xercesc;

//------------------------------------------------------------------------------------------------
// Define classes State and Event, which will be used to store data read from the xml. 
//------------------------------------------------------------------------------------------------

class State{
    public:
    //Standard stuff. Stores the name of the state, the immediate containing state, whether the state is simple
    //(contains no substates) or not, and (if the state is not simple) which substate is active initially.    
    string name;
    string parent;
    bool isSimple;    
    string initial;
    //The following members assist in communication between orthogonal regions. Update messages need to be passed to the top 
    // state or head of each orthogonal region. Heads of orthogonal regions have special roles including: broadcasting messages 
    // to the rest of their region / discarding actioned events / forcing transitions. "String region" stores the name of the
    // head of this state's region. IsOrthogonal=true implies that this state IS a head state. OrthogonalRegionNumber
    // is a numeric key identifying which region this state belongs to. If the chart splits in 3, the regions created will be called
    // 0,1,2 and these keynumbers are required by boost when specifying the structure of the chart. So we'd better have a place
    // to store them. 
    string region;
    bool isOrthogonal;
    int  orthogonalRegionNumber;

    State(string Name, string Parent, bool IsSimple,
     string Initial="", string Region="", bool IsOrthogonal=false, int OrthogonalRegionNumber=0):
    name(Name),
    parent(Parent),
    isSimple(IsSimple),
    initial(Initial),
    region(Region),
    isOrthogonal(IsOrthogonal),
    orthogonalRegionNumber(OrthogonalRegionNumber){}; 
};


class Event{
    public:
    //Standard stuff. Stores the name of the transition event, the states it goes from and to, and a guard condition.
    string name;
    string from;
    string to;
    string guard;
    
    Event(string Name, string From, string To, string Guard):
    name(Name),
    from(From),
    to(To),
    guard(Guard){};
};


//A utility function. Since Boost Statecharts makes heavy use of templates, we need to ensure state definitions are 
//completed in the right order, so when each state is defined, everything it depends on is already done. This function
//helps sort states into dependency order.
bool MyDataSorter(const State& s1, const State& s2)
{   
    //Looks to see if the state s2 contains s1 in its name somewhere as a substring.
    //That would imply that s2 depends on s1.
    if(s2.name.find(s1.name)!=string::npos){
        return true;
    }else{
        return false;
    }
}


//-------------------------------------------------------------------------------
//                        Xerces XML parser preamble
//-------------------------------------------------------------------------------

int main (int argc, char* args[]) {
    
    //Initialises XMLPlatformUtils, gets a DOM parser and an errorhandler and sets their options
    try {
        XMLPlatformUtils::Initialize();
    }
    catch (const XMLException& toCatch) {
        char* message = XMLString::transcode(toCatch.getMessage());
        cout << "Error during initialization! :\n"
             << message << "\n";
        XMLString::release(&message);
        return 1;
    }

    XercesDOMParser* parser = new XercesDOMParser();
    parser->setValidationScheme(XercesDOMParser::Val_Never); //Will need to change once I'm validating against a .xsd
    ErrorHandler* errHandler = (ErrorHandler*) new HandlerBase();
    parser->setErrorHandler(errHandler);


//-------------------------------------------------------------------------------
//                    Setup specific to statecharts
//-------------------------------------------------------------------------------


    //Get name of file to parse from command line args
    string xmlFile = args[1];

    //Make some storage space for the data extracted from the xml
    vector<State> StateList;               //Lists all states
    vector<Event> EventList;               //Lists all events
    vector<string> OrthogonalRegionNames;  //Lists names of all states where a split to orthogonal regions occurs
    vector<int> OrthogonalRegionNumbers;   //Lists the number identifying each region in these splits 
                    
    vector<State> Chart;          // Will hold the uppermost state: the BoostStateMachine CellStatechart
    vector<State> FirstResponder; //Artificial wrapper state responsible for processing the main update command 
                                  //EvCheckCellData issued by Chaste

    //Declare all the tags and attributes we expect to see in the xml file
    XMLCh* TAG_statechart= XMLString::transcode("statechart");
    XMLCh* TAG_event= XMLString::transcode("event");
    XMLCh* TAG_simple_state = XMLString::transcode("simple_state");
    XMLCh* TAG_compound_state= XMLString::transcode("compound_state");

    XMLCh* ATTR_name= XMLString::transcode("name");
    XMLCh* ATTR_parent= XMLString::transcode("parent");
    XMLCh* ATTR_start= XMLString::transcode("start");
    XMLCh* ATTR_to= XMLString::transcode("to");
    XMLCh* ATTR_from= XMLString::transcode("from");
    XMLCh* ATTR_guard= XMLString::transcode("guard");


//-------------------------------------------------------------------------------
//                     Main parsing block
//-------------------------------------------------------------------------------
    
    try {
        parser->parse(xmlFile.c_str());

        DOMDocument* xmlDoc = parser->getDocument();                // Gets the top-level element "statechart"
        DOMElement* elementRoot = xmlDoc->getDocumentElement();     // Parses file looking for tags:
        DOMNodeList*      children = elementRoot->getChildNodes();
        const  XMLSize_t nodeCount = children->getLength();         // Identify children of "statechart" in the XML tree.
        for( XMLSize_t xx = 0; xx < nodeCount; ++xx ){              // Grab them, check nothing odd is going on and cast
            DOMNode* currentNode = children->item(xx);              // to DOMElement.
            if( currentNode->getNodeType() &&  
                currentNode->getNodeType() == DOMNode::ELEMENT_NODE ) 
            {   
            DOMElement* currentElement
            = dynamic_cast< xercesc::DOMElement* >( currentNode );

            
            //If we've found a "simple_state" tag, make a new state + store it in the states vector
            if( XMLString::equals(currentElement->getTagName(), TAG_simple_state))
            {
                const XMLCh* xmlch_name = currentElement->getAttribute(ATTR_name);
                char * temp_name = XMLString::transcode(xmlch_name);
                string name(temp_name);
                const XMLCh* xmlch_parent = currentElement->getAttribute(ATTR_parent);
                char * temp_parent = XMLString::transcode(xmlch_parent);
                string parent(temp_parent);

                StateList.push_back(State(name, parent, true)); //Pushing back a simple state
            }

            //If we've found a "compound_state" tag, make a new state + store it in the states vector
            if( XMLString::equals(currentElement->getTagName(), TAG_compound_state))
                {
                const XMLCh* xmlch_name = currentElement->getAttribute(ATTR_name);
                char * temp_name = XMLString::transcode(xmlch_name);
                string name(temp_name);
                const XMLCh* xmlch_parent = currentElement->getAttribute(ATTR_parent);
                char * temp_parent = XMLString::transcode(xmlch_parent);
                string parent(temp_parent);
                const XMLCh* xmlch_start = currentElement->getAttribute(ATTR_start);
                char * temp_start = XMLString::transcode(xmlch_start);
                string start(temp_start);

                State newState = State(name, parent, false, start); //Create a state object

                //Check whether this is the top level state: the chart itself.
                // Does it have no parent? If so, its the statechart, and we do the following:
                if(parent.compare("")==0){      
                    Chart.push_back(newState);  // Store the state in the Chart vector.

                    //Now, if the state has multiple children separated by commas, it means we have a split
                    //into orthogonal regions. Extract all the child states from the list. Assign each one a
                    //region number, starting at 0. Store their names in a list of Orthogonal Region heads.
                    int regionNumberCounter=0;  
                    istringstream iss(start);
                    string token;
                    while(getline(iss, token, ','))
                    {
                        OrthogonalRegionNames.push_back(token);
                        OrthogonalRegionNumbers.push_back(regionNumberCounter);
                        regionNumberCounter++;
                    }

                    //Add a "fake" state called "Running" as an immediate child of CellStatechart that will
                    //contain everything else. This is our FirstResponder state. We need this state to respond to
                    //Chaste's initial command to update the chart, and pass the update message on to the head of
                    //each region. We can't use just any state for this job: it has to be one that's always active.
                    //So we make a dummy state, Running, and use that.
                    State Running = State("Running","CellStatechart",false,Chart.at(0).initial);
                    FirstResponder.push_back(Running); //Store our dummy state in the vector FirstResponder.
                    Chart.at(0).initial="Running"; //Set the initial state of the CellStatechart to Running.


                }else{

                    //For other compound states, NOT the CellStatechart:
                    //Check is the immediate parent is CellStateChart. If it is, change it to "Running"
                    //(Slotting our dummy state between the chart and its immediate children)
                    if(newState.parent.compare("CellStateChart")==0){
                        newState.parent="Running";
                        StateList.push_back(newState);  //Then, push back the state.
                    }else{
                        StateList.push_back(newState);  //Then, push back the state.
                    }

                    //Record any more orthogonal regions by splitting a child list that contains commas.
                    //Works exactly like the previous section.
                    if(start.find(",")!=string::npos){
                        int regionNumberCounter=0;
                        istringstream iss(start);
                        string token;
                        while(getline(iss, token, ','))
                        {
                            OrthogonalRegionNames.push_back(token);
                            OrthogonalRegionNumbers.push_back(regionNumberCounter);
                            regionNumberCounter++;
                        }
                    }
                }

            }

            //If we've found an event, make a new event object and store it in the events vector
            if( XMLString::equals(currentElement->getTagName(), TAG_event))
            {
                const XMLCh* xmlch_name = currentElement->getAttribute(ATTR_name);
                char * temp_name = XMLString::transcode(xmlch_name);
                string name(temp_name);
                const XMLCh* xmlch_from = currentElement->getAttribute(ATTR_from);
                char * temp_from = XMLString::transcode(xmlch_from);
                string from(temp_from);
                const XMLCh* xmlch_to = currentElement->getAttribute(ATTR_to);
                char * temp_to = XMLString::transcode(xmlch_to);
                string to(temp_to);
                const XMLCh* xmlch_guard = currentElement->getAttribute(ATTR_guard);
                char * temp_guard = XMLString::transcode(xmlch_guard);
                string guard(temp_guard);

                EventList.push_back(Event(name, from, to, guard));
            }

            }
        }

//-------------------------------------------------------------------------------
//                     Catch any parsing exceptions
//-------------------------------------------------------------------------------

   }catch (const XMLException& toCatch) {
        char* message = XMLString::transcode(toCatch.getMessage());
        cout << "Exception message is: \n"
             << message << "\n";
        XMLString::release(&message);
        return -1;
    }
    catch (const DOMException& toCatch) {
        char* message = XMLString::transcode(toCatch.msg);
        cout << "Exception message is: \n"
             << message << "\n";
        XMLString::release(&message);
        return -1;
    }
    catch (...) {
        cout << "Unexpected Exception \n" ;
        return -1;
    }

//-------------------------------------------------------------------------------
//                     Free parser memory
//-------------------------------------------------------------------------------

    try{
        XMLString::release( &TAG_statechart );
        XMLString::release( &TAG_event );
        XMLString::release( &TAG_simple_state );
        XMLString::release( &TAG_compound_state );
        XMLString::release( &ATTR_name );
        XMLString::release( &ATTR_parent );
        XMLString::release( &ATTR_start );
    }catch( ... ){
       cerr << "Unknown exception encountered in TagNamesdtor" << endl;
    }
    delete parser;
    delete errHandler;

    XMLPlatformUtils::Terminate();

//-------------------------------------------------------------------------------
//                     Pre-process our newfound data
//-------------------------------------------------------------------------------

    //Sort the compound states into dependency order using our helper function
    sort(StateList.begin(), StateList.end(), MyDataSorter);

    //Set the "isOrthogonal" flag for each state, by looking through our list of orthogonal regions
    //and checking whether its name appears. If it is orthogonal, also store the appropriate region number.
    for (int i=0; i<StateList.size(); i++){
        for (int j=0; j<OrthogonalRegionNames.size(); j++){
            if((StateList.at(i).name).compare(OrthogonalRegionNames.at(j))==0){
                StateList.at(i).isOrthogonal=true;
                StateList.at(i).orthogonalRegionNumber=OrthogonalRegionNumbers.at(j);
            }
        }
    }

    //Set the region flag. Go through the list of states and record the name of each state's 
    //head-of-region. This is the state that it will need to look to for information/updates in the broadcast system.
    for (int i=0; i<StateList.size(); i++){
        bool foundRegion=false;
        //If state isOrthogonal, then just record its own name. It is the region head.
        if(StateList.at(i).isOrthogonal==true){
            StateList.at(i).region=StateList.at(i).name;
        }else{
            //Otherwise, keep looking at the parent of the current state recursively, until you find one that isOrthogonal
            string currentParentName = StateList.at(i).parent;
            while(foundRegion==false){
                for(int j=0; j<StateList.size(); j++){
                    if((StateList.at(j).name).compare(currentParentName)==0){
                        State next = StateList.at(j);
                        if(next.isOrthogonal==true){
                            StateList.at(i).region=next.name;
                            foundRegion=true;
                        }else{
                            currentParentName=next.parent;
                        }
                        break;
                    }
                }
            }
        }
    }

//-------------------------------------------------------------------------------
//                     Start writing C++ header file
//-------------------------------------------------------------------------------

    ofstream HEADER;
    //Call the file whatever was in the second command line argument.
    char filename[80];
    strcpy(filename,args[2]);
    HEADER.open(strcat(filename,".hpp"));

    //Header preamble
    HEADER << "#ifndef "<< args[2] <<"_HPP_"<<endl;
    HEADER << "#define "<< args[2] <<"_HPP_"<<endl<<endl;    
    HEADER << "#include <boost/statechart/event.hpp>"<<endl;
    HEADER << "#include <boost/statechart/state_machine.hpp>"<<endl;
    HEADER << "#include <boost/statechart/simple_state.hpp>"<<endl;
    HEADER << "#include <boost/statechart/state.hpp>"<<endl;
    HEADER << "#include <boost/statechart/custom_reaction.hpp>"<<endl;
    HEADER << "#include <boost/statechart/transition.hpp>"<<endl;
    HEADER << "#include <boost/mpl/list.hpp>"<<endl;
    HEADER << "#include <boost/config.hpp>"<<endl<<endl;
    HEADER << "#include \"ChasteSerialization.hpp\"" << endl;
    HEADER << "#include <boost/serialization/base_object.hpp>" <<endl;
    HEADER << "#include <boost/serialization/vector.hpp>" <<endl<<endl;  
    HEADER << "namespace sc = boost::statechart;"<<endl;
    HEADER << "namespace mpl = boost::mpl;"<<endl<<endl;


    //Forward-declare all the states as structs
    HEADER<<"struct Running;"<<endl;

    for(int i=0; i<StateList.size(); i++){
        HEADER << "struct " << StateList.at(i).name << ";" << endl;
    }
    HEADER<<endl;


    //Declare some update events. One for each orthogonal region, named Ev*HeadName*Update,
    //and one master update event to be called EvCheckCellData.
    HEADER<< "struct EvCheckCellData :  sc::event< EvCheckCellData > {};"<<endl;
    for(int i=0; i<OrthogonalRegionNames.size(); i++){
        HEADER << "struct Ev" << OrthogonalRegionNames.at(i) << "Update : sc::event< Ev"
        << OrthogonalRegionNames.at(i)<< "Update > {};" << endl;
    }
    HEADER<<endl;

    //Declare events allowing forced transitions into each simple state. We use these to
    //set the state when copying and loading
    for(int i=0; i<StateList.size(); i++){
        if(StateList.at(i).isSimple==true){ 
            //If the state is simple, make a goto event.
            HEADER << "struct EvGoTo" << StateList.at(i).name << " : sc::event< EvGoTo"
            << StateList.at(i).name<< " > {};" << endl;
        }
    }
    HEADER<<endl;

    //Declare the chart structure starting with the CellStatechart itself
    HEADER<<"//PARENT STATECHART"<<endl<<endl;
    
    HEADER<<"struct CellStatechart:  sc::state_machine<CellStatechart,Running>{"<< endl;
    //Declare some functions and variables a statechart is expected to have:
    //1) A constructor.
    HEADER<<"  CellStatechart();"<<endl<<endl;
    //2) A pointer to its cell.
    HEADER<<"  CellPtr pCell;"<<endl<<endl;
    //3) A copy function. Copies this chart's state onto the statechart passed as an argument.
    HEADER<<"  boost::shared_ptr<CellStatechart> Copy(boost::shared_ptr<CellStatechart> myNewStatechart);"<<endl<<endl;
    //4) Getter and setter methods:
    //   - for the state (used in archiving simulations ONLY)
    //   - for any chart-associated-variables (used in archiving simulations ONLY)
    //   - a setter method for the cell pointer.
    HEADER<<"  int GetState();"<<endl;
    HEADER<<"  std::vector<double> GetVariables();"<<endl;
    HEADER<<"  void SetState(int state);"<<endl;
    HEADER<<"  void SetVariables(std::vector<double> variableValues);"<<endl;
    HEADER<<"  void SetCell(CellPtr newCell);"<<endl<<endl;
    //5) Finally, a list of chart-associated-variables (doubles). To make the cell cycle work,
    //   we expect at a minimum to have one double here named TimeInPhase
    HEADER<<"  double TimeInPhase;"<<endl;
    HEADER<<"};"<<endl<<endl;


    //Standard declaration of our first responder Running. It subscribes to the event 
    //EvCheckCellData and says it will provide a response via its react function.
    HEADER <<"//FIRSTRESPONDER STATE"<<endl<<endl;
    HEADER <<"struct Running:  sc::simple_state<Running,CellStatechart,mpl::list< "<< FirstResponder.at(0).initial <<" > >{"<<endl;
    HEADER<< "  typedef sc::custom_reaction< EvCheckCellData > reactions;"<<endl;
    HEADER<< "  sc::result react( const EvCheckCellData & );"<<endl;
    HEADER<<"};"<<endl<<endl;


    //Now declare inner states:
    HEADER <<"//STATES"<<endl;
    for(int i=0; i<StateList.size(); i++){

        //COMPOUND STATES
        if(StateList.at(i).isSimple==false){
            HEADER<<"//--------------------------------------------------------------------------"<<endl<<
            "//--------------------------------------------------------------------------"<<endl;
            
            //STRUCTURAL DEFINITION; TEMPLATES
            HEADER << "struct " << StateList.at(i).name << ": sc::state<" 
            << StateList.at(i).name << ",";
            //Case out branching into orthogonal regions
            if(StateList.at(i).isOrthogonal==false){
                HEADER<< StateList.at(i).parent << ",";
            }else{
                HEADER<< StateList.at(i).parent << "::orthogonal<"<< StateList.at(i).orthogonalRegionNumber<<">,";
            }
            HEADER<<StateList.at(i).initial << ">{"<<endl;

            //CONSTRUCTOR
            HEADER<<"  "<<StateList.at(i).name <<"(my_context ctx);"<<endl; 
            
            //REACTIONS
            //subscribe to the update event for this state's region
            HEADER <<endl<<"  typedef mpl::list< sc::custom_reaction< Ev"<< StateList.at(i).region <<"Update >";
            //if head of region, also subscribe to handle goto events for all states in this region
            if(StateList.at(i).region.compare(StateList.at(i).name)==0){
                for(int j=0; j<StateList.size(); j++){
                    if(StateList.at(i).name.compare(StateList.at(j).region)==0 && StateList.at(j).isSimple==true){
                        HEADER<<","<<endl<<"   sc::custom_reaction< EvGoTo"<< StateList.at(j).name << " >";
                    }
                }
            }
            HEADER<<"   > reactions;"<<endl<<endl;
            
            //REACT FUNCTIONS
            HEADER<<"  sc::result react( const Ev"<< StateList.at(i).region <<"Update" << " & );"<<endl;
            //if head of region, specify how to react to goto events
            if(StateList.at(i).name.compare(StateList.at(i).region)==0){
                for(int j=0; j<StateList.size(); j++){
                    if(StateList.at(i).name.compare(StateList.at(j).region)==0 && StateList.at(j).isSimple==true){
                        HEADER<<"  sc::result react( const EvGoTo"<< StateList.at(j).name << " & ){return transit<"<< StateList.at(j).name <<">();};"<<endl;
                    }
                }
            }

            //end
            HEADER<<"};"<<endl<<endl;
        }
    };


    for(int i=0; i<StateList.size(); i++){

        //SIMPLE STATES
        if(StateList.at(i).isSimple==true){           
            HEADER<<"//--------------------------------------------------------------------------"<<endl<<
            "//--------------------------------------------------------------------------"<<endl;
            //STRUCTURAL DEFINITION; TEMPLATES            
            HEADER << "struct " << StateList.at(i).name << ": sc::state<" 
            << StateList.at(i).name << ",";
            if(StateList.at(i).isOrthogonal==false){
                HEADER<< StateList.at(i).parent << " >{"<<endl;
            }else{
                HEADER<< StateList.at(i).parent << "::orthogonal<"<< StateList.at(i).orthogonalRegionNumber<<" >{"<<endl;
            }
            
            //For the reserved mitosisPhase states, output standard variables and reactions
            if(  StateList.at(i).name.find("_G1",StateList.at(i).name.length()-6)!=string::npos 
               ||StateList.at(i).name.find("_G2",StateList.at(i).name.length()-6)!=string::npos 
               ||StateList.at(i).name.find("_M", StateList.at(i).name.length()-2)!=string::npos 
               ||StateList.at(i).name.find("_S", StateList.at(i).name.length()-2)!=string::npos){  
                
                HEADER<<"  double Duration;"<<endl;
                HEADER<<"  "<<StateList.at(i).name <<"(my_context ctx);"<<endl;
                //reactions
                HEADER <<endl<<"  typedef sc::custom_reaction< EvCellStateChart_CellCycleUpdate> reactions;"<<endl;
                //react functions
                HEADER<<"  sc::result react( const EvCellStateChart_CellCycleUpdate & );"<<endl;

            //else produce a standard constructor and reaction list
            }else{
                //CONSTRUCTOR
                HEADER<<"  "<< StateList.at(i).name <<"(my_context ctx);"<<endl;
                //REACTIONS
                HEADER <<endl<<"  typedef mpl::list< sc::custom_reaction< Ev"<< StateList.at(i).region <<"Update >";
                HEADER<<" > reactions;"<<endl;
                //REACT FUNCTIONS
                HEADER<<"  sc::result react( const " << "Ev"<< StateList.at(i).region <<"Update " << "& );"<<endl;
            }
            //end
            HEADER<<"};"<<endl<<endl;
        }
    }

    HEADER<<"#endif"<<endl;
    HEADER.close();



//-------------------------------------------------------------------------------
//                     Output C++ main file
//-------------------------------------------------------------------------------
ofstream MAIN;
char filenameMain[80];
strcpy(filenameMain,args[2]);
MAIN.open(strcat(filenameMain,".cpp"));

//boilerplate stuff
MAIN<< "#include <StatechartInterface.hpp>"<<endl;
MAIN<< "#include <"<< args[2] <<".hpp>"<<endl<<endl;

//STATECHART FUNCTIONS
MAIN<< "//--------------------STATECHART FUNCTIONS------------------------------"<<endl<<endl;
//constructor
MAIN<< "CellStatechart::CellStatechart(){"<<endl;
MAIN<< "        pCell=boost::shared_ptr<Cell>();"<<endl;
MAIN<< "        TimeInPhase=0;"<<endl;
MAIN<< "};"<<endl<<endl;

//Set cell
MAIN<< "void CellStatechart::SetCell(CellPtr newCell){"<<endl;
MAIN<< "     assert(newCell!=NULL);"<<endl;
MAIN<< "     pCell=newCell;"<<endl;
MAIN<< "};"<<endl<<endl;

//Get chart-associated-variables in array form for archiving.
MAIN<<"std::vector<double> CellStatechart::GetVariables(){"<<endl;
MAIN<<"    std::vector<double> variables;"<<endl;
MAIN<<"    variables.push_back(TimeInPhase);"<<endl;
MAIN<<"    return variables;"<<endl;
MAIN<<"}"<<endl;

//For archiving, write a method that takes a stored array and unpacks values for all variables.
MAIN<<"void CellStatechart::SetVariables(std::vector<double> variables){"<<endl;
MAIN<<"    TimeInPhase=variables.at(0);"<<endl;
MAIN<<"}"<<endl<<endl;

//For archiving, a function that encodes the state as an integer for saving.
MAIN<<"int CellStatechart::GetState(){"<<endl;
MAIN<<"  int state=1;"<<endl;
for(int i=0; i<StateList.size(); i++){
    if(StateList.at(i).isSimple==true){
        MAIN<<" if(state_cast<const "<< StateList.at(i).name <<"*>()!=0){ "<<endl;
        MAIN<<"     state=(state<<1)+1;"<<endl;
        MAIN<<" }else{state=(state<<1);}"<<endl;
    }
}
MAIN<<"return state;"<<endl<<endl;
MAIN<<"}"<<endl<<endl;

//For archiving, a function that takes an integer encoding the state and sets up that state.
MAIN<<"void CellStatechart::SetState(int state){"<<endl;
int counter=0;
for(int i=StateList.size()-1; i>=0; i--){
    if(StateList.at(i).isSimple==true){
        MAIN<<" if((state&"<< pow(2,counter) <<")/"<< pow(2,counter) <<"){ "<<endl;
        MAIN<<"     process_event(EvGoTo"<< StateList.at(i).name <<"());;"<<endl;
        MAIN<<" }"<<endl;
        counter=counter+1;
    }
}
MAIN<<"}"<<endl<<endl;


//Copy function takes in a newly minted statechart, initiates it, then copies this one's
//state onto it.
MAIN<< "boost::shared_ptr<CellStatechart> CellStatechart::Copy(boost::shared_ptr<CellStatechart> myNewStatechart){"<<endl;
MAIN<< "    myNewStatechart->initiate();"<<endl;
for(int i=0; i<StateList.size(); i++){
    if(StateList.at(i).isSimple==true){
        MAIN<< "    if(state_cast<const "<<StateList.at(i).name<<"*>()!=0){"<<endl;
        MAIN<< "        myNewStatechart->process_event(EvGoTo"<<StateList.at(i).name<<"());"<<endl;
        MAIN<< "    }"<<endl;
    }
}
MAIN<< "    return (myNewStatechart);"<<endl;
MAIN<< "};"<<endl<<endl;


MAIN<<"//--------------------FIRST RESPONDER------------------------------"<< endl;
MAIN<<"sc::result Running::react( const EvCheckCellData & ){"<<endl;
    for(int i=0; i<OrthogonalRegionNames.size(); i++){
    MAIN<<"    post_event(Ev"<<OrthogonalRegionNames.at(i)<<"Update());"<<endl;
    }
    MAIN<<"    return discard_event();"<<endl;
MAIN<<"};"<<endl;




//COMPOUND STATES
for (int i=0; i<StateList.size(); i++){
    if(StateList.at(i).isSimple==false){
        MAIN<< "//--------------------------------------------------------------------------" << endl;
        MAIN<< "//--------------------------------------------------------------------------" << endl;
    
        //standard constructor
        MAIN<< StateList.at(i).name << "::"<< StateList.at(i).name << "( my_context ctx ):";
        MAIN<< "my_base( ctx )" << "{};" << endl<< endl;
        
        //Define response to the update event for the region
        MAIN<<"sc::result "<< StateList.at(i).name <<"::react( const Ev"<< StateList.at(i).region <<"Update & ){"<<endl;
        //Grab cell pointer incase it's required
        MAIN<< "    CellPtr myCell=context<CellStatechart>().pCell;" <<endl;
        //For each possible transition, list inside an if with appropriate guard condition 
        for(int j=0; j<EventList.size(); j++){
            if(StateList.at(i).name.compare(EventList.at(j).from)==0){
                MAIN<<"    if("<< EventList.at(j).guard <<"){"<<endl;
                MAIN<<"        return transit<"<< EventList.at(j).to <<">();"<<endl;
                MAIN<<"    }"<<endl;
            }
        }
        if(StateList.at(i).isOrthogonal==true){
            MAIN<<"    return discard_event();"<<endl; //Only region heads get to discard events
        }else{
            MAIN<<"    return forward_event();"<<endl; //Otherwise keep forwarding
        }
        MAIN<<"};"<<endl;
        
    };
};


//LEAF/SIMPLE STATES
for (int i=0; i<StateList.size(); i++){
    if(StateList.at(i).isSimple==true){    
        MAIN<< "//--------------------------------------------------------------------------" << endl;
        MAIN<< "//--------------------------------------------------------------------------" << endl;
        
        //Make constructor, special one is required for the 4 mitosisPhase states
        MAIN<< StateList.at(i).name << "::"<< StateList.at(i).name << "( my_context ctx ):" << endl;
        MAIN<< "my_base( ctx )";

        //Handles action on entry.
        if(StateList.at(i).name.find("Meiosis")!=string::npos){
            MAIN<<"{"<<endl<<"    CellPtr myCell=context<CellStatechart>().pCell;"<<endl<<
            "    SetProliferationFlag(myCell,0.0);"<<endl<<"}"<<endl;

        }else if(StateList.at(i).name.find("_G1",StateList.at(i).name.length()-6)!=string::npos){
            MAIN<<"{ context<CellStatechart>().TimeInPhase = 0.0;"<<endl;
            MAIN<<" CellPtr myCell=context<CellStatechart>().pCell;"<<endl;
            MAIN<<" RandomNumberGenerator* p_gen = RandomNumberGenerator::Instance();"<<endl;
            MAIN<<" Duration=p_gen->NormalRandomDeviate(GetG2Duration(myCell),0.1*GetG2Duration(myCell));"<<endl;
            MAIN<<" SetCellCyclePhase(myCell,G_ONE_PHASE);"<<endl;
            MAIN<<"}"<<endl<<endl;

        }else if(StateList.at(i).name.find("_G2",StateList.at(i).name.length()-6)!=string::npos){
            MAIN<<"{ context<CellStatechart>().TimeInPhase = 0.0;"<<endl;
            MAIN<<" CellPtr myCell=context<CellStatechart>().pCell;"<<endl;
            MAIN<<" RandomNumberGenerator* p_gen = RandomNumberGenerator::Instance();"<<endl;
            MAIN<<" Duration=p_gen->NormalRandomDeviate(GetG2Duration(myCell),0.1*GetG2Duration(myCell));"<<endl;
            MAIN<<" SetCellCyclePhase(myCell,G_TWO_PHASE);"<<endl;
            MAIN<<"}"<<endl<<endl;

        }else if(StateList.at(i).name.find("_S",StateList.at(i).name.length()-2)!=string::npos){
            MAIN<<"{ context<CellStatechart>().TimeInPhase = 0.0;"<<endl;
            MAIN<<" CellPtr myCell=context<CellStatechart>().pCell;"<<endl;
            MAIN<<" RandomNumberGenerator* p_gen = RandomNumberGenerator::Instance();"<<endl;
            MAIN<<" Duration=p_gen->NormalRandomDeviate(GetG2Duration(myCell),0.1*GetG2Duration(myCell));"<<endl;
            MAIN<<" SetCellCyclePhase(myCell,S_PHASE);"<<endl;
            MAIN<<"}"<<endl<<endl;

        }else if(StateList.at(i).name.find("_M",StateList.at(i).name.length()-2)!=string::npos){
            MAIN<<"{ context<CellStatechart>().TimeInPhase = 0.0;"<<endl;
            MAIN<<" CellPtr myCell=context<CellStatechart>().pCell;"<<endl;
            MAIN<<" RandomNumberGenerator* p_gen = RandomNumberGenerator::Instance();"<<endl;
            MAIN<<" Duration=p_gen->NormalRandomDeviate(GetG2Duration(myCell),0.1*GetG2Duration(myCell));"<<endl;
            MAIN<<" SetCellCyclePhase(myCell,M_PHASE);"<<endl;
            MAIN<<"}"<<endl<<endl;
        }else{
        MAIN<< "{};" << endl<< endl;
        }
        
        //DEFINE REACTION TO UPDATE EVENTS
        MAIN<<"sc::result "<< StateList.at(i).name <<"::react( const Ev"<< StateList.at(i).region <<"Update & ){"<<endl;
        //Get pointer to cell if required
        MAIN<< "    CellPtr myCell=context<CellStatechart>().pCell;" <<endl<<endl;

        //Custom update for mitosis states. Advances time spent in this phase.
        if(StateList.at(i).name.find("_G2",StateList.at(i).name.length()-6)!=string::npos 
         ||StateList.at(i).name.find("_G1",StateList.at(i).name.length()-6)!=string::npos 
         ||StateList.at(i).name.find("_M",StateList.at(i).name.length()-2)!=string::npos 
         ||StateList.at(i).name.find("_S",StateList.at(i).name.length()-2)!=string::npos){
            MAIN<<"  context<CellStatechart>().TimeInPhase+=GetTimestep();"<<endl;           
        }
        
        if(StateList.at(i).name.find("Meiosis")!=string::npos){
            MAIN<<"    UpdateRadius(myCell);"<<endl;
        }

        //list transitions behind guards
        for(int j=0; j<EventList.size(); j++){
            if(StateList.at(i).name.compare(EventList.at(j).from)==0){

                //For mitosis states
                if(StateList.at(i).name.find("_G2",StateList.at(i).name.length()-6)!=string::npos 
                ||StateList.at(i).name.find("_G1",StateList.at(i).name.length()-6)!=string::npos 
                ||StateList.at(i).name.find("_M",StateList.at(i).name.length()-2)!=string::npos 
                ||StateList.at(i).name.find("_S",StateList.at(i).name.length()-2)!=string::npos){
                   MAIN<<"    if(context<CellStatechart>().TimeInPhase>=Duration){"<<endl;
                   if(EventList.at(j).from.find("_G2",EventList.at(j).from.length()-6)!=string::npos){
                        MAIN<<"        SetReadyToDivide(myCell,true);"<<endl;
                   } 

                //otherwise, if perfectly normal state, list transitions from event list   
                }else{

                if(EventList.at(j).guard.compare("")!=0){
                    MAIN<<"    if("<< EventList.at(j).guard <<"){"<<endl;
                }
                }

                MAIN<<"        return transit<"<< EventList.at(j).to <<">();"<<endl;
                if(EventList.at(j).guard.compare("")!=0){
                    MAIN<<"    }"<<endl;
                }

            }
        }
        

        if(StateList.at(i).isOrthogonal==true){
            MAIN<<"    return discard_event();"<<endl;
        }else{
            MAIN<<"    return forward_event();"<<endl;
        }
        MAIN<<"};"<<endl;
    };
};

MAIN.close();


//-------------------------------------------------------------------------------
//                     Output C++ Chaste interface header
//-------------------------------------------------------------------------------

ofstream INTER;
char filenameInter[80];
strcpy(filenameInter,args[2]);
INTER.open(strcat(filenameInter,"Interface.hpp"));

INTER<<"#ifndef "<< args[2] <<"INTERFACE_HPP_"<<endl<<
"#define "<< args[2] <<"INTERFACE_HPP_"<<endl<<endl<<
"//Include any Chaste headers you need."<<endl<<endl<<
"#include <Cell.hpp>"<<endl<<
"#include <AbstractCellPopulation.hpp>"<<endl<<
"#include <CellCyclePhases.hpp>"<<endl<<
"#include <StatechartCellCycleModelSerializable.hpp>"<<endl<<endl;

INTER<<"//Fill out all the functions that will get and set cell properties. Some common functions provided."<<endl<<endl;

INTER<<"//Getters for cell cycle model"<<endl;
INTER<<"double GetMDuration(CellPtr pCell){"<<endl;
INTER<<"    return pCell->GetCellCycleModel()->GetMDuration();"<<endl;
INTER<<"};"<<endl;
INTER<<"double GetSDuration(CellPtr pCell){"<<endl;
INTER<<"    return pCell->GetCellCycleModel()->GetSDuration();"<<endl;
INTER<<"};"<<endl;
INTER<<"double GetG1Duration(CellPtr pCell){"<<endl;
INTER<<"    return pCell->GetCellCycleModel()->GetG1Duration();"<<endl;
INTER<<"};"<<endl;
INTER<<"double GetG2Duration(CellPtr pCell){"<<endl;
INTER<<"    return pCell->GetCellCycleModel()->GetG2Duration();"<<endl;
INTER<<"};"<<endl<<endl;
INTER<<"//Setters for cell cycle model"<<endl;
INTER<<"void SetCellCyclePhase(CellPtr pCell, CellCyclePhase_ phase){"<<endl;
INTER<<"    AbstractCellCycleModel* model = pCell->GetCellCycleModel();"<<endl;
INTER<<"    dynamic_cast<StatechartCellCycleModelSerializable*>(model)->SetCellCyclePhase(phase);"<<endl;
INTER<<"}"<<endl<<endl;
INTER<<"void SetReadyToDivide(CellPtr pCell, bool Ready){"<<endl;
INTER<<"    AbstractCellCycleModel* model = pCell->GetCellCycleModel();"<<endl;
INTER<<"    dynamic_cast<StatechartCellCycleModelSerializable*>(model)->SetReadyToDivide(Ready);"<<endl;
INTER<<"};"<<endl<<endl;


INTER<<"//Misc"<<endl;
INTER<<"bool IsDead(CellPtr pCell){"<<endl;
INTER<<"     return pCell->IsDead();"<<endl;
INTER<<"};"<<endl;
INTER<<"double GetTimestep(){"<<endl;
INTER<<"     return SimulationTime::Instance()->GetTimeStep();"<<endl;
INTER<<"};"<<endl;
INTER<<"double GetTime(){"<<endl;
INTER<<"     return SimulationTime::Instance()->GetTime();"<<endl;
INTER<<"};"<<endl<<endl;


INTER<<"//Elegans Specific"<<endl;
INTER<<"void SetProliferationFlag(CellPtr pCell, double Flag){"<<endl;
INTER<<"    pCell->GetCellData()->SetItem(\"Proliferating\",0.0);"<<endl;
INTER<<"};"<<endl;
INTER<<"void   SetRadius(CellPtr pCell, double radius){"<<endl;
INTER<<"       pCell->GetCellData()->SetItem(\"Radius\",radius);"<<endl;
INTER<<"};"<<endl;
INTER<<"double GetDistanceFromDTC(CellPtr pCell){"<<endl;
INTER<<"    return pCell->GetCellData()->GetItem(\"DistanceAwayFromDTC\");"<<endl;
INTER<<"};"<<endl;
INTER<<"double GetMaxRadius(CellPtr pCell){"<<endl;
INTER<<"    return pCell->GetCellData()->GetItem(\"MaxRadius\");"<<endl;
INTER<<"};"<<endl<<endl;
INTER<<"void UpdateRadius(CellPtr pCell){"<<endl;
INTER<< "  double MaxRad = GetMaxRadius(pCell);"<<endl;
INTER<< "  double Rad = pCell->GetCellData()->GetItem(\"Radius\");"<<endl;
INTER<< "  if(Rad<MaxRad-0.1){"<<endl;
INTER<< "    SetRadius(pCell,Rad+=GetTimestep());"<<endl;
INTER<< "  }"<<endl;
INTER<<"};"<<endl;

INTER<<"#endif"<<endl;

INTER.close();

//-------------------------------------------------------------------------------
//                     Free all stored events + states 
//-------------------------------------------------------------------------------

printf("%s\n","\n\nNow go and do the following:");
printf("1) Check guard conditions in the main file  are correct. If neccessary replace with calls to functions in the ChasteInterface.cpp. Even if you think you've handled guards in the XML, just...check.\n");
printf("2) Check on entry/exit actions; anything that involves a state member variable.\n");
printf("3) The states G1,G2,S,M,Mitosis and Meiosis are reserved Chaste states for monitoring the cell cycle. Make sure you have them,\n that they transit G1->S->G2->M, and are enclosed inside Mitosis, which should be inside CellCycle.\n The guards you've set for them will be ignored and replaced by standard ones.\n\n");


return 0;
}