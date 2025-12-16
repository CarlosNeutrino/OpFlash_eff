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
void eta(){
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

    // Create lower pad --> eta distribution
    TPad *pad2 = new TPad("efficiency TPC", "efficiency TPC", 0., 0., 1., 0.6);
    pad2->SetTopMargin(0.1);
    pad2->SetBottomMargin(0.1);
    pad2->SetRightMargin(0.1);
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
        int pdgcode;            // Only to know if the interaction is neutrino or cosmic
        TVector3 vertex;
    };
    // Create vecctor to save the event information
    std::vector<Event_Info> ID_interactions;

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
        TVector3 vertex_interaction;    // From the struct, Vertex of the coincident interaction

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
                        InteractionPDG = inter.pdgcode;
                        vertex_interaction = inter.vertex;
                    }
                }
                
                if(coincident_interaction==true && CoincidentInteractionTime > flash_time->at(f)){  // I only want to plot the events that have a coincident interaction with the OpFlash

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
                    double start_time = h_OpHit->GetBinCenter(maxBin);
                    exp_f->SetParameters(amplitude, start_time); // Set the parameters of the function
                    exp_f->SetRange(min_hist, max_hist);  // Set the range of the function
                    TuneGraph_1D(exp_f);
                    exp_f->Draw("same");

                    // I calculate the amount of PEs that each OpFlash has in each PMT in 
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

                    // Now I have the total amount of PE that each PMT has detected in this OpFlash
                    // I make a loop in time to get the points:
                    /*
                    - x: time in bins of 200ns
                    - y: value of eta=(#PE uncoated / #PE coated)
                    */
                    TGraph* gr_eta = new TGraph();
                    gr_eta->SetTitle("eta distribution; time (#mu s); #eta (#PE uncoated / #PE coated)");
                    double TimeInterval = 0.8; // 800 ns
                    for(double time = flash_time->at(f); time < flash_time->at(f) + 8.; time += TimeInterval){
                       double PE_coated = 0.;
                       double PE_uncoated = 0.;

                       // Loop over all OpHits of the OpFlash
                       for(int j=0; j<flash_ophit_starttime->at(f).size(); j++){
                           double ophit_time = flash_ophit_starttime->at(f).at(j) + flash_ophit_risetime->at(f).at(j);
                           if(ophit_time >= time && ophit_time < time + TimeInterval){   // If the OpHit is in the time bin
                               int opch = flash_ophit_ch->at(f).at(j);
                               // Get the type of the OpDet
                               for(int k=0; k<OpDetID->size(); k++){
                                   if(OpDetID->at(k) == opch){
                                       if(OpDetType->at(k) == 0){  // Coated PMT
                                           PE_coated += flash_ophit_pe->at(f).at(j);
                                       }else if(OpDetType->at(k) == 1){    // Uncoated PMT
                                           PE_uncoated += flash_ophit_pe->at(f).at(j);
                                       }
                                   }
                               }
                           }
                       } // End loop over all OpHits of the OpFlash
                       // Fill the tGraph
                        int n = gr_eta->GetN();   // Get current number of points
                        gr_eta->SetPoint(n, time+0.1, PE_uncoated/PE_coated); // Append new point
                    }

                    // I draw the eta graph
                    pad2->cd();
                    TuneTGraph_1D(gr_eta);
                    gr_eta->Draw("AL");
                    // I save the canvas
                    c1->SaveAs(Form("CoincidentInteractions/Distribution_%i_%i.pdf", i, f));
                }  
            }// End for(int f=0; f<nopflash; f++){
        } // Finish loop over entries
    } // Finish loop over files
   
}
