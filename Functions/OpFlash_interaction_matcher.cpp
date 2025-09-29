/*
Code for matching OpFlash with MCTruth using time
RECEIVES: 
- the time of the OpFlash (variable start_time) -> OpFlash_time (in micro-s) 
- the event tree, including truth information -> tree
- The index of the event of the tree to look at -> i

RETURNS: 
- the ID of the MCTruth that matches in time with the OpFlash

*/


int OpFlash__interaction_matcher(int OpFlash_time, TTree* tree, int i) {

    // Load variables to read from the tree JUST for the event i
    tree->GetEntry(i);

    int gen_ID = -1;   // Default value if no match is found
    double int_time;    // time of the interaction (neutrino or not) 

    // Loop over all the MCTruth interactions in the event to find a match in time with the OpFlash
    for(j=0; j<MCP_time->size(); j++) {
        double int_time = MCP_time->at(j)*1000;  // Get the time of the j-th interaction in ns

        // Check if the OpFlash time is within a certain window of the MCTruth time
        if (std::abs(OpFlash_time - nu_time) < TIME_WINDOW) {  // 50.0 ns window to check if this is the interaction
            if(process->at(j)=="primary")    // I look for the primary particle of the interaction
                gen_ID = trackID->at(j);  // Match found, store the ID
        }
    }

    return gen_ID;
}