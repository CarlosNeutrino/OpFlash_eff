/*
Code for plotting cout'ing some correlations between flash variables and interactions (cosmic, neutrino, michelle e-)
*/

// INCLUDES
// Definition of the variables of the tree
#include "/Users/carlosmm/Documents/Doctorado/ServiceTask/Task/tree_definitions_light/tree_utils.cpp"
#include "/Users/carlosmm/Documents/Doctorado/ServiceTask/Task/tree_definitions_light/tree_utils_matcher.cpp"
#include "/Users/carlosmm/Documents/Doctorado/ServiceTask/Task/tree_definitions_light/Includes.h"
// Definition of the constants
#include "/Users/carlosmm/Documents/Doctorado/ServiceTask/Task/Constants/Constants.h"
// Definition of the functions to tune the histograms and lines and arrows
#include "/Users/carlosmm/Documents/Doctorado/ServiceTask/Task/Functions/TuneFunctions.cpp"
// Definition of the function to match OpFlash with interaction
#include "/Users/carlosmm/Documents/Doctorado/ServiceTask/Task/Functions/OpFlash_interaction_matcher.cpp"
/* Definition of functions to do auxiliary tasks:
- Calculate the visible energy of the interaction
- Calculate the mean drift position of the interaction 
*/
#include "/Users/carlosmm/Documents/Doctorado/ServiceTask/Task/Functions/AuxFuncs.cpp"


// MAIN CODE
void FastSlowLight(){
    bool DoFit = false;
    // Declaration of the canvas
    TCanvas *c1= new TCanvas("name", "Efficiency", 1000, 800);  

    // enables automatic error storage for all subsequent TH1 objects
    TH1::SetDefaultSumw2();  
    gStyle->SetPalette(kViridis);    // Scale color for plots

    TH1F *hist_Coincident = new TH1F("hist_Coincident", "Ratio fast/slow light",  50, 0, 4);
    TH1F *hist_NoCoincident = new TH1F("hist_NoCoincident", "Ratio fast/slow light",  50, 0, 4);

    // Range of the histogram. Change accordingly to what you may want
    double min_hist = 0.;
    double max_hist = 1.;
    double CoincidentInteractionTime;

    // declaration of the OpAna TTree
    TTree *event_tree;
    TTree *pdsmap_tree;
    TTree *eff_tree;

    // Declaration of the files 
    // File for the OpAna information
    std::vector<string> filenames;
    filenames.push_back("/Users/carlosmm/Documents/Doctorado/ServiceTask/Task/Data/opana_tree_NewMC_Fall.root");
    // File for the efficiency information
    string filename_eff = "/Users/carlosmm/Documents/Doctorado/ServiceTask/Task/Data/Matching_NewMC_big.root";

    // Variables to manage the file and the directory of its tree
    TFile *input_file;
    TDirectoryFile *tree_dir;

    // Also for the efficiency file
    // Variables to manage the file and the directory of its tree
    TFile *input_file_eff;
    TDirectoryFile *tree_dir_eff;

    // Initialization of the pointers
    input_file_eff=nullptr;
    tree_dir_eff=nullptr;
    eff_tree=nullptr;

    // Put the filename as a TFile object
    input_file_eff = new TFile(filename_eff.c_str());

    // Get the directory 'ana' inside the file
    tree_dir_eff = (TDirectoryFile*)input_file_eff->Get("flashrecoeff");
    // Get the event tree from the 'opanatree' directory
    eff_tree =(TTree*)tree_dir_eff->Get("FlashRecoEff");
    set_branch_Light(eff_tree);

    // Define vector to save the eventID and time of the interaction that are 'coincident' with an OpFlash
    // First, define struct to save 3 variables together
    struct Event_Info {
        unsigned int event_ID_struct;
        unsigned int run_ID_struct;
        unsigned int subrun_ID_struct;
        double time_struct;
        int pdgcode;            // Only to know if the interaction is neutrino or cosmic
    };
    // Create vecctor to save the event information
    std::vector<Event_Info> ID_interactions;

     // Loop over all entries of the TTree
    int num_events=eff_tree->GetEntries();
    int Niters = num_events;

    for (int i=0; i<Niters; i++){

        // Get the entry 'i' of the event tree
        eff_tree->GetEntry(i);
        // cout<<"EventID eff: " << eventID_eff << ", RunID eff: " << runID_eff << ", SubrunID eff: " << subrunID_eff << endl;

        if(i%10000==0)  cout<<endl<<endl<<"           -------- Interaction number: " << i << "------" <<endl;
        // Each entry of the tree is an MCTruth object (1 truth interaction)
        
        if(abs(InteractionTime)<1.3e6){

            // Case in which there is an OpFlash corresponding to the interaction
            if(CoincidentInteraction==true && RecoFlash==false){
                // Save the eventID and interactiontime of the coincident interaction
                ID_interactions.push_back({eventID_eff, runID_eff, subrunID_eff, InteractionTime/1000., IntPDG}); // time in us
            }
        } // End loop over entries in the TTree
    }

    cout<<endl<<endl<<endl<<endl<<endl<<"----------------- Finished saving the coincident interactions -----------------"<<endl<<endl<<endl<<endl<<endl;

    // Now I loop over all the files that I have with the OpHit information
    for(string file : filenames){
        // Initialization of the pointers
        input_file=nullptr;
        tree_dir=nullptr;
        event_tree=nullptr;
        pdsmap_tree=nullptr;

        // Put the filename as a TFile object
        input_file = new TFile(file.c_str());

        // Get the directory 'ana' inside the file
        tree_dir = (TDirectoryFile*)input_file->Get("opanatree");
        // Get the event tree from the 'ana' directory
        event_tree =(TTree*)tree_dir->Get("OpAnaTree");
        pdsmap_tree =(TTree*)tree_dir->Get("PDSMapTree");

        // I use the code in tree_utils.cpp to initialyze all the variables inside the TTrees. Look at tree_utils.cpp fro more info.
        set_branch_OpAna(event_tree);
        set_branch_PDS_map(pdsmap_tree);

        // Now I have access to all the Branches of the TTree
        // and I’ve stored them in the variables that are defined in tree_utils.
        // I can now use whichever variables I want, since I should already have them at my disposal :)

        // To access the variables, I need to loop over all the data in the corresponding Branch:
        //  - Using an index i, go through all the entries that the tree has
        //  - By calling event_tree->GetEntry(i); you obtain ALL the variables of that tree entry
        //  - Then you can do whatever you want with that entry

        cout << "-------- You are reading the file: " << file << "------" <<endl<<endl;

        // define more variables here
        double abs_time;    // value of the time of the OpFlash
        double start_time, finish_time;     // start and finish time of the OpFlash
        double ymin, ymax;                  // maximum and minimum value of the gPad
        int max_OpHit;                      // maximum value of the OpHit of a certain OpFlah

        int InteractionPDG;         // From the struct, to know if the interaction is neutrino or cosmic

        // Loop over all entries of the TTree
        int num_events=event_tree->GetEntries();
        int num_pdsmaps=pdsmap_tree->GetEntries();
        int Niters = num_events;
        for (int i=1; i<Niters; i++){

            // Get the entry 'i' of the event tree
            event_tree->GetEntry(i);
            pdsmap_tree->GetEntry(0);   // Get any entry of the map

            if(i%100==0) cout<<endl<<"           -------- Event number: " << i << "------" <<endl<<endl;

            // cout<<"EventID opana: " << eventID << ", RunID opana: " << runID << ", SubrunID opana: " << subrunID << endl;

            ////////////////////////////////////////////
            // Distribution of OpHits of each opFlash //
            ////////////////////////////////////////////
            for(int f=0; f<nopflash; f++){
                InteractionPDG = 0;   // Reset the PDG code of the interaction

                // See if the event contains an interaction that is coincident with this OpFlash
                bool coincident_interaction = false;

                for (const auto& inter : ID_interactions) {
                    if(inter.event_ID_struct == eventID
                        && inter.run_ID_struct == runID 
                        && inter.subrun_ID_struct == subrunID
                        && abs(inter.time_struct - flash_time->at(f)) < 8.) {   // If the eventID matches and the time of the interaction is close to the time of the OpFlash within the veto window
                        
                        CoincidentInteractionTime = inter.time_struct;
                        coincident_interaction = true;
                        InteractionPDG = inter.pdgcode;
                    }
                }
                
                if(coincident_interaction==true && CoincidentInteractionTime > flash_time->at(f)){  // I select the events that have an interaction coincident with the OpFlash and that the interaction happens AFTER the flash

                    //cout<<"Found coincident interaction for OpFlash!! " << endl;
                    // Find the minimum value of flash_time->at(f) - OpHit time to reference every time to this one
                    double min_time = 1e9;
                    for(int j=0; j<flash_ophit_starttime->at(f).size(); j++){
                        abs_time = flash_ophit_starttime->at(f).at(j) + flash_ophit_risetime->at(f).at(j);
                        if( (abs_time - flash_time->at(f)) < min_time ){
                            min_time = abs_time - flash_time->at(f);
                        }
                    }
                    min_time = min_time + 0.001; // Add 1 ns to avoid having zero difference

                    // I calculate the total of light due to fast and slow components
                    double total_fast_light = 0.;
                    double total_slow_light = 0.;
                    for(int j=0; j<flash_ophit_starttime->at(f).size(); j++){
                        abs_time = flash_ophit_starttime->at(f).at(j) + flash_ophit_risetime->at(f).at(j) - min_time;
                        //cout<<"Abs time: " << abs_time <<", Time of the OpFlash start: " << flash_time->at(f) <<", Difference: " << abs_time - flash_time->at(f) << endl;
                        if( ( abs_time - flash_time->at(f) ) > 0. && ( abs_time - flash_time->at(f) ) < FAST_LIGHT_TIME ){
                            total_fast_light += flash_ophit_pe->at(f).at(j);
                        }else if( ( abs_time - flash_time->at(f) ) >= FAST_LIGHT_TIME && ( abs_time - flash_time->at(f) ) < FAST_LIGHT_TIME + SLOW_LIGHT_TIME ){
                            total_slow_light += flash_ophit_pe->at(f).at(j);
                        }
                    }
                    // Fill the histogram with the ratio of fast light and slow light
                    if(total_slow_light > 0){
                        double ratio_fast_slow = total_fast_light / total_slow_light;
                        hist_Coincident->Fill(ratio_fast_slow);
                    }

                }else if(coincident_interaction==false){  // Case in which there is NO interaction coincident with the OpFlash
                    // Find the minimum value of flash_time->at(f) - OpHit time to reference every time to this one
                    //cout<<"NO Found coincident interaction for OpFlash!! " << endl;
                    double min_time = 1e9;
                    for(int j=0; j<flash_ophit_starttime->at(f).size(); j++){
                        abs_time = flash_ophit_starttime->at(f).at(j) + flash_ophit_risetime->at(f).at(j);
                        if( (abs_time - flash_time->at(f)) < min_time ){
                            min_time = abs_time - flash_time->at(f);
                        }
                    }
                    min_time = min_time + 0.001; // Add 1 ns to avoid having zero difference

                    // I calculate the total of light due to fast and slow components
                    double total_fast_light = 0.;
                    double total_slow_light = 0.;
                    // Find the number of OpHits at a different distance to the OpFlash start time
                    for(int j=0; j<flash_ophit_starttime->at(f).size(); j++){
                        abs_time = flash_ophit_starttime->at(f).at(j) + flash_ophit_risetime->at(f).at(j) - min_time;
                        //cout<<"Abs time: " << abs_time <<
                        //    ", Time of the OpFlash start: " << flash_time->at(f) <<
                        //    ", Difference: " << abs_time - flash_time->at(f) << endl;
                        if( ( abs_time - flash_time->at(f) ) >0. && ( abs_time - flash_time->at(f) ) < FAST_LIGHT_TIME ){
                            total_fast_light += flash_ophit_pe->at(f).at(j);
                        }else if( ( abs_time - flash_time->at(f) ) >= FAST_LIGHT_TIME && ( abs_time - flash_time->at(f) ) < FAST_LIGHT_TIME + SLOW_LIGHT_TIME ){
                            total_slow_light += flash_ophit_pe->at(f).at(j);
                        }
                    }
                    // Fill the histogram with the ratio of fast light and slow light
                    if(total_slow_light > 0){
                        double ratio_fast_slow = total_fast_light / total_slow_light;
                        hist_NoCoincident->Fill(ratio_fast_slow);
                    }
                }
            }// End for(int f=0; f<nopflash; f++){
        } // Finish loop over entries
    } // Finish loop over files

    // Use log scale
    gPad->SetLogy(true);

    // Tune the histograms
    TuneHist(hist_Coincident);
    hist_Coincident->Scale(1.0 / hist_Coincident->Integral());
    hist_Coincident->GetXaxis()->SetTitle("Fast light #PE/Slow light #PE");
    hist_Coincident->GetYaxis()->SetTitle("# Events");
    TuneHist(hist_NoCoincident);
    hist_NoCoincident->Scale(1.0 / hist_NoCoincident->Integral());
    hist_NoCoincident->GetXaxis()->SetTitle("Fast light #PE/Slow light #PE");
    hist_NoCoincident->GetYaxis()->SetTitle("# Events");

    // Select color for the histograms
    // Coincident histograms:
    hist_Coincident->SetLineColor(kRed);
    hist_Coincident->SetFillColorAlpha(kRed-2, 0.5);
    // NoCoincident histograms:
    hist_NoCoincident->SetLineColor(kBlue);
    hist_NoCoincident->SetFillColorAlpha(kAzure-8, 0.5);

    /*
    // Create stacked histogram
    // Create a stack
    THStack* hstack = new THStack("hs", "Fast/Slow light;Fast light #PE/Slow light #PE; # Events");
    hstack->Add(hist_Coincident);
    hstack->Add(hist_NoCoincident);
    // Set the Y-axis range
    //hstack->SetMinimum(1);
    hstack->Draw("HIST");
    */

    // Draw the histograms
    hist_Coincident->Draw("HIST");
    hist_NoCoincident->Draw("HIST SAME");

    // Add a legend
    auto leg = new TLegend(0.65, 0.8, 0.9, 0.9);
    leg->AddEntry(hist_Coincident, "Coincident interaction", "f");
    leg->AddEntry(hist_NoCoincident, "No coincident interaction", "f");
    leg->SetTextSize(0.02); // Adjust to the preferred size
    leg->Draw("SAME");

    c1->Update();
    c1->SaveAs("FastSlowLight/FastSlowLight_200ns_norm_Log.pdf");
   
}
