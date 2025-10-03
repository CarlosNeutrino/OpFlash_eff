/*
Code for matching OpFlash with MCTruth using time AND the index of the neutrino causing the interaction
RECEIVES: 
- the time of the OpFlash (variable start_time) -> OpFlash_time (in micro-s) 
- the event tree, including truth information -> tree
- The index of the event of the tree to look at -> i
- BY REFERENCE

RETURNS: 
- the ID of the MCTruth that matches in time with the OpFlash

RIGHT NOW IT IS ONLY RELIABLE FOR MATCHING THE NEUTRINO INTERACTIONS
*/


int OpFlash_interaction_matcher(double OpFlash_time, TTree* tree, int i, int &nu_index) {

    // Load variables to read from the tree JUST for the event i
    tree->GetEntry(i);

    int gen_ID = -1;   // Default value if no match is found
    double int_time;    // time of the interaction (neutrino or not) 
    double nu_time;     // time of the neutrino interaction
    nu_index = -1;      // Default value of the index of the neutrino that caused the interaction

    // Loop over all the MCTruth interactions in the event to find a match in time with the OpFlash
    for(int j=0; j<MCP_time->size(); j++) {

        if(process->at(j)=="primary"){ // I look for the primary particle of the interaction

            double int_time = MCP_time->at(j)/1000.;  // Get the time of the j-th interaction in mu-s
            
            
                /*
                cout<<"The OpFlash time is: "<<OpFlash_time<<" mu-s. " <<endl<<
                "       The interaction time is: "<<int_time<<" mu-s"<<endl<<endl;
                */
            
                     
            

            // Check if the OpFlash time is within a certain window of the MCTruth time
            if (std::abs(OpFlash_time - int_time) < TIME_WINDOW) {  // 10.0 ns window to check if this is the interaction    
                gen_ID = trackID->at(j);  // Match found, store the ID
            }
        }
    }

    if( gen_ID<20000000 && gen_ID != -1 ){
        // I look for the index of the neutrino that caused the interaction
        cout<<"LOOKING FOR THE NEUTRINO INDEX"<<endl;
        for(int k=0; k<nuvE->size(); k++){
            nu_time = nuvT->at(k)/1000.; // time of the neutrino interaction in mu-s

            cout<<"   Neutrino index "<<k<<" with time "<<nu_time<<" mu-s"<<endl;
            cout<<"       The OpFlash time is: "<<OpFlash_time<<" mu-s"<<endl;

            if( std::abs(nu_time-OpFlash_time) < TIME_WINDOW){ // I compare the time of the neutrino interaction with the time of the interaction
                nu_index = k; // I store the index of the neutrino that caused the interaction
            }
        }
    }

    return gen_ID;
}