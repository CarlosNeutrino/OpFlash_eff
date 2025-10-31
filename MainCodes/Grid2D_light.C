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
void Grid2D_light(){
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
    TPad *pad2 = new TPad("efficiency TPC", "efficiency TPC", 0., 0., 1, 0.6);
    pad2->SetTopMargin(0.1);
    pad2->SetBottomMargin(0.1);
    pad2->SetRightMargin(0.2);
    pad2->SetLeftMargin(0.1);
    pad2->SetGridx();
    pad2->Draw();

    


    // enables automatic error storage for all subsequent TH1 objects
    TH1::SetDefaultSumw2();  
    gStyle->SetPalette(kViridis);    // Scale color for plots

    // Range of the histogram. Change accordingly to what you may want
    double min_hist = -15.;
    double max_hist = 15.;

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
    std::vector<std::pair<int, double>> ID_interactions;

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
            if(CoincidentInteraction==true){
                // Save the eventID and interactiontime of the coincident interaction
                ID_interactions.push_back(std::make_pair(eventID_eff, InteractionTime/1000.));
                cout<<"   Saved interaction with eventID "<<eventID_eff<<" and time "<<InteractionTime/1000.<<" mu-s"<<endl<<endl;
            }
        } // End loop over entries in the TTree
    }



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

        // Loop over all entries of the TTree
        int num_events=event_tree->GetEntries();
        int num_pdsmaps=pdsmap_tree->GetEntries();
        cout<<"Number of maps: " << num_pdsmaps << endl;
        int Niters = num_events;
        Niters=100;   // For testing purposesG
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

                // See if the event contains an interaction that is coincident with this OpFlash
                bool coincident_interaction = false;
                for(const auto& pair : ID_interactions){
                    //cout<<"   Checking interaction with eventID "<<pair.first<<" and time "<<pair.second<<" mu-s"<<endl;
                    //cout<<"       Current eventID is "<<eventID<<" and OpFlash time is "<<flash_time->at(f)<<" mu-s"<<endl;
                    if(pair.first == eventID && abs(pair.second - flash_time->at(f)) < 8.){   // If the eventID matches and the time of the interaction is close to the time of the OpFlash within the veto window
                        coincident_interaction = true;
                        cout<<"   Found coincident interaction with eventID "<<pair.first<<" and time "<<pair.second<<" mu-s"<<endl<<endl;
                    }
                }
                
                if(coincident_interaction==true){  // I only want to plot the events that have a coincident interaction with the OpFlash

                    // Define the histogram limits using the OpFlash times
                    // Get the time of the flash
                    min_hist = flash_time->at(f) - 1.;
                    max_hist = flash_time->at(f) + VETO_TIME + 1.;
                    TString hname = Form("Histogram_%d", i);
                    TH1F *h_OpHit = new TH1F(hname, "temporal distribution",  (max_hist - min_hist)/BIN_SIZE, min_hist, max_hist);

                    // I reset the histogram that had been created 
                    h_OpHit->Reset();

                    // Analysis of the variables
                    // I loop over all OpHits that have been registered
                    for(int j=0; j<flash_ophit_starttime->at(f).size(); j++){
                        // I fill each histogram with the time of the OpHit. 
                        // I weight the histogram with the number of photo-electrons in each OpHit
                        abs_time = flash_ophit_starttime->at(f).at(j) + flash_ophit_risetime->at(f).at(j);
                        h_OpHit->Fill( abs_time, flash_ophit_pe->at(f).at(j) );
                    }

                    // I write the number of OpFlashes in each histogram (each event)
                    // Define the box position in NDC (normalized device coordinates)
                    TPaveText *pt = new TPaveText(0.6, 0.72, 0.9, 0.9, "NDC");  
                    pt->SetFillColor(0);   // transparent background
                    pt->SetBorderSize(1);  // small border
                    pt->SetTextSize(0.06);
                    pt->SetTextAlign(12);

                    // Build the message
                    std::string msg_1 = "# Event: " + std::to_string(i);
                    std::string msg_2 = "# OpFlash: " + std::to_string(f);

                    // Add text line
                    pt->AddText(msg_1.c_str());
                    pt->AddText(msg_2.c_str());

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
                    double line_max = h_OpHit->GetMaximum()*0.75;

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

                    // Create the plots
                    // TPC0
                    if(gr2d_TPC1->GetN() == 0){
                        pad2->cd();
                        TuneGraph_2D(gr2d_TPC0, "Spatial distribution, TPC0");    
                        gr2d_TPC0->Draw("COLZ");
                        gr2d_TPC0->GetXaxis()->SetRangeUser(Z_MIN, Z_MAX);  // x-axis
                        gr2d_TPC0->GetYaxis()->SetRangeUser(Y_MIN, Y_MAX);  // y-axis
                    }else if(gr2d_TPC0->GetN() == 0){
                        // TPC1
                        pad2->cd();
                        TuneGraph_2D(gr2d_TPC1, "Spatial distribution, TPC1");     
                        gr2d_TPC1->Draw("COLZ");
                        gr2d_TPC1->GetXaxis()->SetRangeUser(Z_MIN, Z_MAX);  // x-axis
                        gr2d_TPC1->GetYaxis()->SetRangeUser(Y_MIN, Y_MAX);  // y-axis
                    }

                    
                    ///////////////////////////////////
                    // ALTERNATIVE: DO 2D-HISTOGRAMS //
                    ///////////////////////////////////
                    /*
                    // Create a NEW histogram for this event
                    ////////////////////////////////////
                    // Spatial Distribution of OpHits //
                    ////////////////////////////////////
                    TString hname_2D_TPC0 = Form("Histogram_2D_TPC0_%d", i);
                    TH2F *h_OpHit_2D_TPC0 = new TH2F(hname_2D_TPC0, "Espatial distribution, TPC0",  10, Z_MIN, Z_MAX, 8, Y_MIN, Y_MAX);

                    TString hname_2D_TPC1 = Form("Histogram_2D_TPC1_%d", i);
                    TH2F *h_OpHit_2D_TPC1 = new TH2F(hname_2D_TPC1, "Spatial distribution, TPC1",  8, Z_MIN, Z_MAX, 8, Y_MIN, Y_MAX);

                    // Fill the histogram with the OpHits in TPC0 (x<0)
                    // I loop over all OpHits that have been registered
                    for(int j=0; j<flash_ophit_starttime->at(f).size(); j++){
                        if( (flash_ophit_starttime->at(f).at(j) > min_hist) && (flash_ophit_starttime->at(f).at(j) < max_hist) ){     // Get the OpHits in the same temporal window

                            // Get the channel of the OpHit
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
                                        if(OpDetType->at(k) == 0 || OpDetType->at(k) == 1 ){  // I only take into account PMTs
                                            h_OpHit_2D_TPC0->Fill( z_opdet, y_opdet, flash_ophit_pe->at(f).at(j) );
                                        }
                                    }else{  // I now take the OpHits in TPC1 (x>0)
                                        if(OpDetType->at(k) == 0 || OpDetType->at(k) == 1 ){  // I only take into account PMTs
                                            h_OpHit_2D_TPC1->Fill( z_opdet, y_opdet, flash_ophit_pe->at(f).at(j) );  
                                        }// End if (OpDetType->at(k) == 0 || OpDetType->at(k) == 1 )
                                    }// End if(x_opdet > 0){
                                }// End if(OpDetID->at(k) == opch){
                            }// End for(int k=0; k<OpDetID->size(); k++){
                        }// End if( (flash_ophit_starttime->at(f).at(j) > min_hist) && (flash_ophit_starttime->at(f).at(j) < max_hist) ){
                    }// End for(int j=0; j<flash_ophit_starttime->at(f).size(); j++){


                    // Look for the maximum value of both histograms to set the same scale
                    double zmin = std::min(h_OpHit_2D_TPC0->GetMinimum(0), h_OpHit_2D_TPC1->GetMinimum(0)); // Get the minimum value (0 to avoid getting the minimum different from 0)
                    double zmax = std::max(h_OpHit_2D_TPC0->GetMaximum(), h_OpHit_2D_TPC1->GetMaximum());   // Get the maximum value

                    h_OpHit_2D_TPC0->SetMinimum(zmin);
                    h_OpHit_2D_TPC0->SetMaximum(zmax);
                    h_OpHit_2D_TPC1->SetMinimum(zmin);
                    h_OpHit_2D_TPC1->SetMaximum(zmax); 

                    pad2->cd();
                    // I draw the histogram pad 2
                    TuneHist_2D(h_OpHit_2D_TPC0);
                    h_OpHit_2D_TPC0->Draw("colz");

                    pad3->cd();
                    // I draw the histogram pad 3
                    TuneHist_2D(h_OpHit_2D_TPC1);
                    h_OpHit_2D_TPC1->Draw("colz");
                    **/

                    // I save the canvas
                    c1->SaveAs(Form("Distributions_3D/Distribution_%i_%i.pdf", i, f));

                }
                
            }// End for(int f=0; f<nopflash; f++){
        } // Finish loop over entries
    } // Finish loop over files
   
}
