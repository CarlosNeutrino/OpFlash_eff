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
void Grid3D_light_coincident(){
    bool DoFit = true;
    // Declaration of the canvas
    TCanvas *c1= new TCanvas("name", "Efficiency", 1000, 800);  

     /*
    I create 2 pads in the canvas
    - The one on top to show the temporal distribution of OpHits
    - The one below to show the spatial distribution of OpHits
    */
   // Create upper pad --> temporal distribution
    TPad *pad1 = new TPad("events", "events", 0, 0.6, 1, 1.0); // (xlow, ylow, xup, yup)
    pad1->SetBottomMargin(0.2);  // no x-axis labels on upper pad
    pad1->SetGridx();          // optional grid
    pad1->Draw();

    // Create lower pad --> spatial distribution
    TPad *pad2 = new TPad("efficiency TPC", "efficiency TPC", 0., 0., 1., 0.6);
    pad2->SetTopMargin(0.1);
    pad2->SetBottomMargin(0.1);
    pad2->SetRightMargin(0.2);
    pad2->SetLeftMargin(0.1);
    pad2->Draw();

    // Define a function for expected distribution of light (1D exponential)
    TF1 *exp_f = new TF1("ExpHit", Exp1D, -1500, 1500., 2); // 2 = number of parameters
    /*
    Parameters:
    - Amplitude (#PE in the time t_0)
    - Start time
    */
   // I will cange the range in each representation

    // Define a 2D gaussian. 
    TF2 *GaussFit = new TF2("GaussFit", gauss2D, Z_MIN, Z_MAX, Y_MIN, Y_MAX, 5); // 5 = number of parameters
    GaussFit->SetParameters(1000., 0., 10., 250., 10.); // Initial parameters for the fit
    /*
    Parameters:
    - Amplitude
    - Mean x
    - Sigma x
    - Mean y
    - Sigma y
    */
    // I will change the parameters in each fit later

    // enables automatic error storage for all subsequent TH1 objects
    TH1::SetDefaultSumw2();  
    gStyle->SetPalette(kViridis);    // Scale color for plots

    // Range of the histogram. Change accordingly to what you may want
    double min_hist = -15.;
    double max_hist = 15.;
    double CoincidentInteractionTime;
    double CoincidentInteractionTime_flashed;

    // Histograms to save the lines and arrow to deliminate an opFlash
    std::vector<TLine*> lines_start_ev;                  // auxiliary vector to save the above one
    std::vector<TLine*> lines_finish_ev;                 // auxiliary vector to save the above one
    std::vector<TArrow*> arrows_ev;                      // auxiliary vector to save the above one

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
        int pdgcode_struct;            // Only to know if the interaction is neutrino or cosmic
        TVector3 vertex_struct;
    };
    struct Event_Info_flashed {
        unsigned int event_ID_struct;
        unsigned int run_ID_struct;
        unsigned int subrun_ID_struct;
        double time_struct;
        int pdgcode_struct;            // Only to know if the interaction is neutrino or cosmic
        TVector3 vertex_struct;
    };
    // Create vecctor to save the event information
    std::vector<Event_Info> ID_interactions;
    std::vector<Event_Info_flashed> ID_interactions_flashed;

     // Loop over all entries of the TTree
    int num_events=eff_tree->GetEntries();
    int Niters = num_events;

    for (int i=0; i<Niters; i++){

        // Get the entry 'i' of the event tree
        eff_tree->GetEntry(i);
        if(i%10000==0)  cout<<endl<<endl<<"           -------- Event number: " << i << "------" <<endl;
        // Each entry of the tree is an MCTruth object (1 truth interaction)
        
        if(abs(InteractionTime)<1.3e6){
            // Case in which there is an OpFlash corresponding to the interaction
            if(CoincidentInteraction==true && RecoFlash==false){
                TVector3 vertex_pos;
                vertex_pos.SetXYZ(dEPromX, dEPromY, dEPromZ);
                // Save the eventID and interactiontime of the coincident interaction
                ID_interactions.push_back({eventID_eff, runID_eff, subrunID_eff, InteractionTime/1000., IntPDG, vertex_pos}); // time in us
            }
        } 
    }// End loop over entries in the TTree

    // Find the average deposition of energy position of the interaction that DOES cause an OpFlash
    for (int i=0; i<Niters; i++){
        // Get the entry 'i' of the event tree
        eff_tree->GetEntry(i);
        if(i%10000==0)  cout<<endl<<endl<<"           -------- Event number: " << i << "------" <<endl;
        // Each entry of the tree is an MCTruth object (1 truth interaction)
        if(abs(InteractionTime)<1.3e6){
            // Case in which there is an OpFlash corresponding to the interaction
            // Loop over all previous saved interactions that caused an OpFlash
            for (const auto& inter : ID_interactions) {
                if(eventID_eff == inter.event_ID_struct && runID_eff == inter.run_ID_struct && subrunID_eff == inter.subrun_ID_struct){
                    //cout<<"Matching interactions with times: "<<InteractionTime/1000.<<" and "<<inter.time_struct<<" ?"<<endl;
                    if( (abs(InteractionTime/1000. - inter.time_struct) < 8.) && (RecoFlash==true) ){
                        //cout<<"Found an interaction that ALSO caused an OpFlash! in interaction number: "<<i<<endl;
                        TVector3 vertex_pos;
                        vertex_pos.SetXYZ(dEPromX, dEPromY, dEPromZ);
                        // Save the eventID and interactiontime of the coincident interaction
                        ID_interactions_flashed.push_back({eventID_eff, runID_eff, subrunID_eff, InteractionTime/1000., IntPDG, vertex_pos}); // time in us
                    }
                }
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
        int InteractionPDG_flashed;         // From the struct, to know if the interaction is neutrino or cosmic
        TVector3 vertex_interaction;    // From the struct, Vertex of the coincident interaction
        TVector3 vertex_interaction_flashed;    // From the struct, Vertex of the coincident interaction

        int total_case1=0;  // number of coincident interaction AFTER the OpFlash
        int total_case2=0;  // number of coincident interaction BEFORE the OpFlash
        int total_case3=0; // Number of interactions that are NOT coincident with any OpFlash
        int total_case1_beam=0;         // number of coincident interaction AFTER the OpFlash due to beam neutrino
        int total_case1_cosmic=0;       // number of coincident interaction AFTER the OpFlash due to cosmic
        int total_case2_beam=0;         // number of coincident interaction BEFORE the OpFlash due to beam neutrino
        int total_case2_cosmic=0;       // number of coincident interaction BEFORE the OpFlash due to cosmic

        // Loop over all entries of the TTree
        int num_events=event_tree->GetEntries();
        int num_pdsmaps=pdsmap_tree->GetEntries();
        int Niters = num_events;
        for (int i=0; i<Niters; i++){

            // Get the entry 'i' of the event tree
            event_tree->GetEntry(i);
            pdsmap_tree->GetEntry(0);   // Get any entry of the map

            cout<<endl<<"           -------- Event number: " << i << "------" <<endl<<endl;

            // Create a NEW histogram for this event
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
                        InteractionPDG = inter.pdgcode_struct;
                        vertex_interaction = inter.vertex_struct;
                    }
                }
                // Count the different cases
                /*
                if(abs(InteractionPDG)==12 || abs(InteractionPDG)==14){
                    if(coincident_interaction==true && CoincidentInteractionTime > flash_time->at(f)){
                        total_case1++;
                        if(flash_time->at(f)>2. || flash_time->at(f)<0.1){
                            total_case1_cosmic++;
                        }else{
                            total_case1_beam++;
                        }
                    }else if(coincident_interaction==true && CoincidentInteractionTime < flash_time->at(f)){
                        total_case2++;
                        if(flash_time->at(f)>2. || flash_time->at(f)<0.1){
                            total_case2_cosmic++;
                        }else{
                            total_case2_beam++;
                        }
                    }else if(coincident_interaction==false){
                        total_case3++;
                    }
                }
                */
                
                if(coincident_interaction==true && CoincidentInteractionTime > flash_time->at(f)){  // I only want to plot the events that have a coincident interaction with the OpFlash

                    // Get the information about the interaction that causes the OpFlash
                    for (const auto& inter : ID_interactions_flashed) {
                        if(inter.event_ID_struct == eventID
                            && inter.run_ID_struct == runID 
                            && inter.subrun_ID_struct == subrunID
                            && abs(inter.time_struct - flash_time->at(f)) < 2.) {   // If the eventID matches and the time of the interaction is close to the time of the OpFlash within the veto window
                            
                            CoincidentInteractionTime_flashed = inter.time_struct;
                            InteractionPDG_flashed = inter.pdgcode_struct;
                            vertex_interaction_flashed = inter.vertex_struct;
                        }
                    }

                    cout<<"The time of the interactions are: "<<CoincidentInteractionTime<<" (causing no OpFlash) and "<<CoincidentInteractionTime_flashed<<" (causing OpFlash)"<<endl;


                    // Define the histogram limits using the OpFlash times
                    // Get the time of the flash
                    min_hist = flash_time->at(f) - 1.;
                    max_hist = flash_time->at(f) + VETO_TIME + 1.;
                    TString hname = Form("Histogram_%d", i);
                    TH1F *h_OpHit = new TH1F(hname, "time distribution",  (max_hist - min_hist)/BIN_SIZE, min_hist, max_hist);

                    // I reset the histogram that had been created 
                    h_OpHit->Reset();

                    // I loop over all OpHits that have been registered
                    for(int j=0; j<flash_ophit_starttime->at(f).size(); j++){
                        // I fill each histogram with the time of the OpHit. 
                        // I weight the histogram with the number of photo-electrons in each OpHit
                        abs_time = flash_ophit_starttime->at(f).at(j) + flash_ophit_risetime->at(f).at(j);
                        h_OpHit->Fill( abs_time, flash_ophit_pe->at(f).at(j) );
                    }

                    // I write the number of OpFlashes in each histogram (each event)
                    // Define the box position in NDC (normalized device coordinates)
                    TPaveText *pt = new TPaveText(0.45, 0.64, 0.9, 0.9, "NDC");  
                    pt->SetFillColor(0);   // transparent background
                    pt->SetBorderSize(1);  // small border
                    pt->SetTextSize(0.06);
                    pt->SetTextAlign(12);

                    // Build the message
                    std::string msg_1 = "# Event: " + std::to_string(i);
                    std::string msg_2 = "# OpFlash: " + std::to_string(f);
                    std::string msg_3 = "Coincident interaction time: " + std::to_string(CoincidentInteractionTime) + " #mu s";

                    // Add text line
                    pt->AddText(msg_1.c_str());
                    pt->AddText(msg_2.c_str());
                    pt->AddText(msg_3.c_str());

                    // I reset the TPave text at the beginning of each event
                    lines_start_ev.clear();
                    lines_finish_ev.clear();
                    arrows_ev.clear();

                    // I plot the veto time
                    // I get the start and end time of the flash
                    start_time = flash_time->at(f);
                    finish_time = start_time + VETO_TIME;

                    // I get the minimun and maximum of the gPad
                    gPad->Update();
                    ymin = 0;

                    // I create the lines and arrow that I will need
                    TLine* l1 = new TLine(start_time, ymin, start_time, 100000);            // Start of the OpFlash
                    TLine* l2 = new TLine(finish_time, ymin, finish_time, 100000);          // Finish of the OpFlash
                    TArrow* arrow = new TArrow(start_time, 100, finish_time, 100, 0.04, "->");     // Arrow connecting the lines. The last two options are the size of the arrow and the ends

                    lines_start_ev.push_back(l1);
                    lines_finish_ev.push_back(l2);
                    arrows_ev.push_back(arrow);

                    // I draw the histogram 
                    TuneHist(h_OpHit);
                    pad1->cd();
                    h_OpHit->Draw("hist");
                    // Get the maximum of the hists_OpHit
                    double line_max = h_OpHit->GetMaximum()*0.6;

                    // Draw on canvas
                    pt->Draw("same");

                    // I draw the lines and arrows
                    for(int h=0; h<lines_start_ev.size(); h++){
                        // Redefine ymax of the lines according to the canvas
                        lines_start_ev.at(h)->SetY2(line_max);
                        lines_finish_ev.at(h)->SetY2(line_max);

                        // I tune the lines and arrow to look better
                        TuneLine_start(lines_start_ev.at(h));
                        TuneLine_finish(lines_finish_ev.at(h));
                        TuneArrow(arrows_ev.at(h));

                        // Redefine the y of the arrows
                        double arrow_y = line_max * 0.9; // 75% of max height
                        arrows_ev.at(h)->SetY1(arrow_y);
                        arrows_ev.at(h)->SetY2(arrow_y);

                        // Draw on canvas
                        if(lines_start_ev.at(h)->GetX1()>min_hist && lines_finish_ev.at(h)->GetX2()<max_hist){  // I only draw the lines if they are in the range of the histogram
                            lines_start_ev.at(h)->Draw("same");
                            lines_finish_ev.at(h)->Draw("same");
                            arrows_ev.at(h)->Draw("same");
                        }
                    }

                    // Draw the exponenctial function
                    // Find the start time of the flash in the histogram as the first time bin with content
                    // Find the bin with the maximum PE value
                    int maxBin = h_OpHit->GetMaximumBin();

                    // Extract amplitude (max number of PE)
                    double amplitude = h_OpHit->GetBinContent(maxBin);

                    // Extract start time (x value at that bin)
                    double start_time_exp = h_OpHit->GetBinCenter(maxBin);
                    exp_f->SetParameters(amplitude, start_time_exp); // Set the parameters of the function
                    exp_f->SetRange(min_hist, max_hist);  // Set the range of the function
                    TuneGraph_1D(exp_f);
                    exp_f->Draw("same");

                    // I calculate the amount of light that each OpFlash has in each PMT in a map
                    std::map<int, double> map_ID_PE_TPC0;    // map with the ID of each PMT and the total PE that it has detected in this OpFlash
                    std::map<int, double> map_ID_PE_TPC1;    // map with the ID of each PMT and the total PE that it has detected in this OpFlash

                    // I accumulate the amount of PE in each PMT
                    for(int j=0; j<flash_ophit_starttime->at(f).size(); j++){
                        if( (flash_ophit_starttime->at(f).at(j) > min_hist) && (flash_ophit_starttime->at(f).at(j) < max_hist) ){     // Get the OpHits in the same temporal window

                            // Get the channel of the OpHit and its position
                            int opch = flash_ophit_ch->at(f).at(j);
                            for(int k=0; k<OpDetID->size(); k++){
                                if(OpDetID->at(k) == opch){
                                    // Get the position of the OpDet
                                    double x_opdet = OpDetX->at(k);
                                    double y_opdet = OpDetY->at(k);
                                    double z_opdet = OpDetZ->at(k);

                                    // I take the OpHits in TPC0 (x<0)
                                    if(x_opdet < 0){
                                        // I fill the histogram with the position of the OpDet
                                        // I weight the histogram with the number of photo-electrons in each OpHit
                                        if(OpDetType->at(k) == 0 || OpDetType->at(k) == 1){  // I only take into account COATED PMTs
                                            map_ID_PE_TPC0[opch] += flash_ophit_pe->at(f).at(j);
                                        }else{
                                            cout<<"OpDet Type different from COATED PMT: " << OpDetType->at(k) << endl;
                                        }
                                    }else{  // I now take the OpHits in TPC1 (x>0)
                                        if(OpDetType->at(k) == 0 || OpDetType->at(k) == 1){  // I only take into account COATED PMTs
                                            map_ID_PE_TPC1[opch] += flash_ophit_pe->at(f).at(j);
                                        }// End if (OpDetType->at(k) == 0 || OpDetType->at(k) == 1 )
                                    }// End if(x_opdet > 0){
                                }// End if(OpDetID->at(k) == opch){
                            }// End for(int k=0; k<OpDetID->size(); k++){
                        }// End if( (flash_ophit_starttime->at(f).at(j) > min_hist) && (flash_ophit_starttime->at(f).at(j) < max_hist) ){
                    }// End for(int j=0; j<flash_ophit_starttime->at(f).size(); j++){

                    // Get the position of each PMT and add it to a TGraph2D
                    TGraph2D *gr2d_TPC0 = new TGraph2D();
                    for(auto const& [ID, PE] : map_ID_PE_TPC0){
                        // Get the position of the OpDet
                        for(int k=0; k<OpDetID->size(); k++){
                            if(OpDetID->at(k) == ID){
                                double x_opdet = OpDetX->at(k);
                                double y_opdet = OpDetY->at(k);
                                double z_opdet = OpDetZ->at(k);

                                gr2d_TPC0->SetPoint(gr2d_TPC0->GetN(), z_opdet, y_opdet, PE);
                            }
                        }
                    }

                    // Same for the other TPC
                    TGraph2D *gr2d_TPC1 = new TGraph2D();
                    for(auto const& [ID, PE] : map_ID_PE_TPC1){
                        // Get the position of the OpDet
                        for(int k=0; k<OpDetID->size(); k++){
                            if(OpDetID->at(k) == ID){
                                double x_opdet = OpDetX->at(k);
                                double y_opdet = OpDetY->at(k);
                                double z_opdet = OpDetZ->at(k);

                                gr2d_TPC1->SetPoint(gr2d_TPC1->GetN(), z_opdet, y_opdet, PE);
                            }
                        }
                    }

                    if(DoFit==true){
                        double max_x = 0.0;
                        double max_y = 0.0;
                        double max_z = 0.0;

                        // Create the plots for the fit residuals
                        TGraph2D* gr2d_TPC0_susbtracted = new TGraph2D();
                        TGraph2D* gr2d_TPC1_susbtracted = new TGraph2D();

                        // create the marker to show the vertex
                        TMarker *cross = new TMarker(vertex_interaction.Z(), vertex_interaction.Y(), 2);  // 2 = cross "×"
                        cout<<"Drawing vertex in: ("<<vertex_interaction.Z()<<", "<<vertex_interaction.Y()<<")" << endl;
                        cross->SetMarkerColor(kRed);
                        cross->SetMarkerSize(2.0);
                        cross->SetMarkerStyle(29);  // 29 = big cross (star)

                        // create the marker to show the vertex of the flashed interaction
                        TMarker *cross_flashed = new TMarker(vertex_interaction_flashed.Z(), vertex_interaction_flashed.Y(), 2);  // 2 = cross "×"
                        cout<<"Drawing vertex in: ("<<vertex_interaction_flashed.Z()<<", "<<vertex_interaction_flashed.Y()<<")" << endl;
                        cross_flashed->SetMarkerColor(kMagenta);
                        cross_flashed->SetMarkerSize(2.0);
                        cross_flashed->SetMarkerStyle(29);  // 29 = big cross (star)

                        // Fit the spatial distribution of OpHits in each TPC
                        if(gr2d_TPC1->GetN() == 0){
                            GaussFit->SetBit(kCannotPick);  // draws flat on top
                            // Get the values of maximum Z (#PEs), X (Z position) and Y (Y position)
                            max_z = GetMaxZ(gr2d_TPC0, max_x, max_y);

                            GaussFit->SetParameters(max_z, max_x, 100., max_y, 100.); // parameters of the plot
                            GaussFit->SetRange(Z_MIN, Z_MAX, Y_MIN, Y_MAX); // Set the range of the function again for drawing

                            gr2d_TPC0_susbtracted = SubtractFit(gr2d_TPC0, GaussFit);
                            // Mask the negative values
                            int n = gr2d_TPC0_susbtracted->GetN();
                            double x, y, z;

                            for (int i = 0; i < n; i++) {
                                gr2d_TPC0_susbtracted->GetPoint(i, x, y, z); // get the i-th point
                                if (z < 0) z = 1;        // mask: set z to 0 if < 0
                                gr2d_TPC0_susbtracted->SetPoint(i, x, y, z); // update the point
                            }
                            pad2->cd();
                            TuneGraph_2D(gr2d_TPC0_susbtracted, "Spatial distribution residuals, TPC0");
                            gr2d_TPC0_susbtracted->Draw("COLZ");
                            cross->Draw("SAME");
                            cross_flashed->Draw("SAME");
                            
                            c1->Update();
                            c1->Modified();
                            gPad->Update();

                        }else if(gr2d_TPC0->GetN() == 0){
                            GaussFit->SetBit(kCannotPick);  // draws flat on top
                            max_z = GetMaxZ(gr2d_TPC1, max_x, max_y);

                            GaussFit->SetParameters(max_z, max_x, 100., max_y, 100.); // Initial parameters for the fit
                            GaussFit->SetRange(Z_MIN, Z_MAX, Y_MIN, Y_MAX); // Set the range of the function again for drawing

                            gr2d_TPC1_susbtracted = SubtractFit(gr2d_TPC1, GaussFit);
                            // Mask the negative values
                            int n = gr2d_TPC1_susbtracted->GetN();
                            double x, y, z;

                            for (int i = 0; i < n; i++) {
                                gr2d_TPC1_susbtracted->GetPoint(i, x, y, z); // get the i-th point
                                if (z < 0) z = 1;        // mask: set z to 0 if < 0
                                gr2d_TPC1_susbtracted->SetPoint(i, x, y, z); // update the point
                            }

                            pad2->cd();
                            TuneGraph_2D(gr2d_TPC1_susbtracted, "Spatial distribution residuals, TPC1");
                            gr2d_TPC1_susbtracted->Draw("COLZ");  
                            cross->Draw("SAME");
                            cross_flashed->Draw("SAME");
                            
                            c1->Update();
                            c1->Modified();
                            gPad->Update();
                        }

                        // Create the legend (x1, y1, x2, y2 in NDC coordinates)
                        TLegend *leg_fit = new TLegend(0.10, 0.75, 0.38, 0.88);

                        // Add marker entries
                        leg_fit->AddEntry(cross,         "Coincident interaction vertex", "p");
                        leg_fit->AddEntry(cross_flashed, "Flashed interaction vertex","p");

                        // Legend appearance
                        leg_fit->SetBorderSize(0);                 // no border
                        leg_fit->SetTextSize(0.03);
                        leg_fit->SetFillColorAlpha(kWhite, 0.5);   // 50% transparent background
                        // Draw legend
                        leg_fit->Draw("same");

                        // I save the canvas
                        c1->SaveAs(Form("Distributions_3D_coincident_fit/Distribution_%i_%i.pdf", i, f));
                    }
                    // In any case, draw the unfitted distribution
                    // create the marker to show the vertex
                    TMarker *cross = new TMarker(vertex_interaction.Z(), vertex_interaction.Y(), 2);  // 2 = cross "×"
                    cout<<"Drawing vertex in: ("<<vertex_interaction.Z()<<", "<<vertex_interaction.Y()<<")" << endl;
                    cross->SetMarkerColor(kRed);
                    cross->SetMarkerSize(2.0);
                    cross->SetMarkerStyle(29);  // 29 = big cross (star)

                    // create the marker to show the vertex of the flashed interaction
                    TMarker *cross_flashed = new TMarker(vertex_interaction_flashed.Z(), vertex_interaction_flashed.Y(), 2);  // 2 = cross "×"
                    cout<<"Drawing vertex in: ("<<vertex_interaction_flashed.Z()<<", "<<vertex_interaction_flashed.Y()<<")" << endl;
                    cross_flashed->SetMarkerColor(kMagenta);
                    cross_flashed->SetMarkerSize(2.0);
                    cross_flashed->SetMarkerStyle(29);  // 29 = big cross (star)

                    // Create the plots
                    // TPC0
                    if(gr2d_TPC1->GetN() == 0){
                        pad2->cd();
                        TuneGraph_2D(gr2d_TPC0, "Spatial distribution, TPC0");    
                        gr2d_TPC0->Draw("COLZ");
                        cross->Draw("SAME");
                        cross_flashed->Draw("SAME");
                        gr2d_TPC0->GetXaxis()->SetRangeUser(Z_MIN, Z_MAX);  // x-axis
                        gr2d_TPC0->GetYaxis()->SetRangeUser(Y_MIN, Y_MAX);  // y-axis
                    }else if(gr2d_TPC0->GetN() == 0){
                        // TPC1
                        pad2->cd();
                        TuneGraph_2D(gr2d_TPC1, "Spatial distribution, TPC1");     
                        gr2d_TPC1->Draw("COLZ");
                        cross->Draw("SAME");
                        cross_flashed->Draw("SAME");
                        gr2d_TPC1->GetXaxis()->SetRangeUser(Z_MIN, Z_MAX);  // x-axis
                        gr2d_TPC1->GetYaxis()->SetRangeUser(Y_MIN, Y_MAX);  // y-axis
                    }
                    // Create the legend (x1, y1, x2, y2 in NDC coordinates)
                    TLegend *leg = new TLegend(0.10, 0.75, 0.38, 0.88);

                    // Add marker entries
                    leg->AddEntry(cross,         "Coincident interaction vertex", "p");
                    leg->AddEntry(cross_flashed, "Flashed interaction vertex","p");

                    // Legend appearance
                    leg->SetBorderSize(0);                 // no border
                    leg->SetTextSize(0.03);
                    leg->SetFillColorAlpha(kWhite, 0.5);   // 50% transparent background
                    // Draw legend
                    leg->Draw("same");

                    // I save the canvas
                    c1->SaveAs(Form("Distributions_3D_coincident/Distribution_%i_%i.pdf", i, f));

                }
                
            }// End for(int f=0; f<nopflash; f++){
        } // Finish loop over entries
        /*
        cout<<"Fraction of interaction in case 1: "<<double(total_case1)/((total_case1+total_case2+1./0.8920*total_case3))<<endl;
        cout<<"   -  of which beam neutrino: " << double(total_case1_beam)/double(total_case1) << endl;
        cout<<"   -  of which cosmic: " << double(total_case1_cosmic)/double(total_case1) << endl;
        cout<<"Fraction of interaction in case 2: "<<double(total_case2)/((total_case1+total_case2+1./0.8920*total_case3))<<endl;
        cout<<"   -  of which beam neutrino: " << double(total_case2_beam)/double(total_case2) << endl;
        cout<<"   -  of which cosmic: " << double(total_case2_cosmic)/double(total_case2) << endl;
        cout<<"Fraction of interaction in case 3: "<<double(1./0.8920*total_case3)/((total_case1+total_case2+1./0.8920*total_case3))<<endl;
        */
    } // Finish loop over files
   
}
