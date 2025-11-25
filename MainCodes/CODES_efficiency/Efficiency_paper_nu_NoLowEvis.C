// INCLUDES
// Definition of the variables of the tree
#include "/Users/carlosmm/Documents/Doctorado/ServiceTask/Task/tree_definitions_light/tree_utils_matcher.cpp"
#include "/Users/carlosmm/Documents/Doctorado/ServiceTask/Task/tree_definitions_light/Includes.h"
// Definition of the constants
#include "/Users/carlosmm/Documents/Doctorado/ServiceTask/Task/Constants/Constants.h"
// Definition of the functions to tune the histograms and lines and arrows
#include "/Users/carlosmm/Documents/Doctorado/ServiceTask/Task/Functions/TuneFunctions.cpp"


// MAIN CODE
void Efficiency_paper_nu_NoLowEvis(){
    // Declaration of the canvas
    TCanvas *c1= new TCanvas("name", "Efficiency", 1000, 800);  

    /*
    I create 2 pads in the canvas
    - The one on top to show the # of events
    - The one below to show the efficiency
    */
   // Create upper pad (events)
    TPad *pad1 = new TPad("events", "events", 0, 0.4, 1, 1.0); // (xlow, ylow, xup, yup)
    pad1->SetBottomMargin(0.1);  // no x-axis labels on upper pad
    pad1->SetGridx();          // optional grid
    pad1->Draw();

    TPad *pad2 = new TPad("efficiency", "efficiency", 0, 0.05, 1, 0.4);
    pad2->SetTopMargin(0.1);
    pad2->SetBottomMargin(0.3);
    pad2->SetGridx();
    pad2->Draw();


    // Declaration of the histograms --> efficiency of detecting neutrino interactions with the opFlash
    /*
    - total: all neutrino interactions
    - flashed: neutrino interactions that have one corresponding OpFlash
    */

    // enables automatic error storage for all subsequent TH1 objects
    TH1::SetDefaultSumw2();  

    // Interactions that have a flash
    TH1F *hist_Evis_flash = new TH1F("Evis_flash", "hist_Evis_flash",  30, 0, 2000);
    TH1F *hist_xdrift_flash = new TH1F("drift_flash", "hist_xdrift_flash",  30, 0, 200);

    // Interations that do NOT have a flash. It will also be used for the number of events
    TH1F *hist_Evis_all = new TH1F("Evis_all", "hist_Evis_all",  30, 0, 2000);
    TH1F *hist_xdrift_all = new TH1F("drift_all", "hist_xdrift_all",  30, 0, 200);


    // Range of the histogram. Change accordingly to what you may want
    double min_hist = -10.;
    double max_hist = 16.;
    std::vector<TH1F*> hists;               // vector to save the distribution of OpHits ONLY WHEN A NEUTRINO IS LOST
    std::vector<TPaveText*> pts;            // vector to save the text

    std::vector<std::vector<TLine*>> lines_start;        // vector to save the horizontal lines of the time of the beginning of the OpFlash for each event
    std::vector<std::vector<TLine*>> lines_finish;       // vector to save the horizontal lines of the time of the beginning of the OpFlash for each event
    std::vector<TLine*> lines_start_ev;                  // auxiliary vector to save the above one
    std::vector<TLine*> lines_finish_ev;                 // auxiliary vector to save the above one

    std::vector<std::vector<TArrow*>> arrows;            // arrow between the beginnig and the end of the OpFlash
    std::vector<TArrow*> arrows_ev;                      // auxiliary vector to save the above one

    // declaration of the TTrees
    TTree *event_tree;
    TTree *header_tree;

    // Declaration of the files 
    std::vector<string> filenames;
    filenames.push_back("/Users/carlosmm/Documents/Doctorado/ServiceTask/Task/Data/Matching_OldMC.root");
    filenames.push_back("/Users/carlosmm/Documents/Doctorado/ServiceTask/Task/Data/Matching_NewMC_big.root");

    // Variables to manage the file and the directory of its tree
    TFile *input_file;
    TDirectoryFile *tree_dir;

    for(string file : filenames){

        // Resetting the histograms for the different files
        hist_Evis_flash->Reset();
        hist_xdrift_flash->Reset();
        hist_Evis_all->Reset();
        hist_xdrift_all->Reset();

        // Initialization of the pointers
        input_file=nullptr;
        tree_dir=nullptr;
        event_tree=nullptr;
        header_tree=nullptr;

        // Put the filename as a TFile object
        input_file = new TFile(file.c_str());

        // Get the directory 'ana' inside the file

        tree_dir = (TDirectoryFile*)input_file->Get("flashrecoeff");
        // Get the event tree from the 'opanatree' directory
        event_tree =(TTree*)tree_dir->Get("FlashRecoEff");
        header_tree =(TTree*)tree_dir->Get("FlashRecoEffHeader");

        // I use the code in tree_utils.cpp to initialyze all the variables inside the TTrees. Look at tree_utils.cpp fro more info.
        set_branch_Light(event_tree);
        set_branch_header(header_tree);

        // Now I have access to all the Branches of the TTree
        // and I’ve stored them in the variables that are defined in tree_utils_matcher.
        // I can now use whichever variables I want, since I should already have them at my disposal :)

        // To access the variables, I need to loop over all the data in the corresponding Branch:
        //  - Using an index i, go through all the entries that the tree has
        //  - By calling event_tree->GetEntry(i); you obtain ALL the variables of that tree entry
        //  - Then you can do whatever you want with that entry

        cout << "-------- You are reading the file: " << file << "------" <<endl<<endl;

        // define more variables here
        ////// Variables for the OpHit distribution //////
        double abs_time;    // value of the time of the OpFlash
        double start_time, finish_time;     // start and finish time of the OpFlash
        double ymin, ymax;                  // maximum and minimum value of the gPad
        int max_OpHit;                      // maximum value of the OpHit of a certain OpFlah

        ////// Variables for efficiency plots //////
        int num_interactions_flash;         // Event by event count of the flashed interactions
        int num_interactions_true;          // Event by event count of the true interactions

        int total_num_interactions = 0;             // Accumulated number of interactions
        int total_num_flashed_interactions = 0;     // Accumulated number of flashed interactions
        int total_coincident_interactions = 0;    // Accumulated number of coincident interactions
        int total_lowLight_events = 0;          // Accumulated number of low light events (no flash, no coincident interaction)
    
        double Evis;                    // visible energy of the neutrino interaction
        double drift;                   // drift distance of the neutrino interaction

        int matched_ID = -1;               // ID of a particle of an interaction that matches with the OpFlash
        int previous_matched_ID = -1;      // previous matched ID to check if a neutrino produces 2 OpFlashes
        int nu_index = -1;                 // index of the neutrino that caused the interaction
        std::vector<double> FlashTime_v;        // vector to save the time of the interactions that have produced an OpFlash
        std::vector<double> InteractionTime_v;        // vector to save the time of the interactions that have produced an OpFlash

        // Loop over all entries of the TTree
        int num_events=event_tree->GetEntries();
        int Niters = num_events;
            for (int i=0; i<Niters; i++){
                num_interactions_flash=0;    // Each event I reset the number of neutrinos that have been flashed
                num_interactions_true=0;    // Each event I reset the number of true neutrinos inside the TPC

                // Get the entry 'i' of the event tree
                event_tree->GetEntry(i);
                if(i%10000==0)  cout<<endl<<endl<<"           -------- Event number: " << i << "------" <<endl;
                // Each entry of the tree is an MCTruth object (1 truth interaction)
                
                ////////////////////////////////////////////////////////////////////////////////////////
                ////// Calculate the # of interactions in the event that have produced an OpFlash //////
                ////////////////////////////////////////////////////////////////////////////////////////
                if( abs(IntPDG)==14 || abs(IntPDG)==12 ){ // I only consider neutrino interactions
                    if(abs(InteractionTime)<1.3e6){
                        // Calculate the (weighted) drift coordinate and visible energy
                        Evis = DepositedEnergy;
                        drift = abs(dEPromX);

                        if(Evis>200){

                            // Case in which there is an OpFlash corresponding to the interaction
                            if(RecoFlash==true){
                                // I fill the histograms of flashed interactions
                                hist_Evis_flash->Fill(Evis);
                                hist_xdrift_flash->Fill(drift);

                                num_interactions_flash++;
                                total_num_flashed_interactions++;

                                // I save the time of the OpFlash
                                FlashTime_v.push_back(FlashTime);
                                
                            }else if(CoincidentInteraction==true){
                                total_coincident_interactions++;
                            }

                            if(RecoFlash==false && CoincidentInteraction==false){
                                total_lowLight_events++;
                            }else if(RecoFlash==false && CoincidentInteraction==true){
                                cout<<"Flash time: "<<FlashTime<<" us, Interaction time: "<<InteractionTime/1000.<<" us"<<endl;
                            }
                            // In any case, the 'all' histograms are filled here
                            hist_Evis_all->Fill(Evis);
                            hist_xdrift_all->Fill(drift);
                            total_num_interactions++;
                            num_interactions_true++;
                
                            // I save the time of the interaction
                            InteractionTime_v.push_back(InteractionTime);      
                        }
                    }
                }
            } // End loop over entries in the TTree

        // Print the efficiency
        cout<<endl<<"----- Efficiency stats: -----"<<endl;
        cout<<"-----------------------------"<<endl;
        cout<<"EFFICIENCY: "<<(double)total_num_flashed_interactions/(double)total_num_interactions * 100<<" %"<<endl;
        cout<<"Fraction of coincident interactions: "<<(double)total_coincident_interactions/(double)total_num_interactions * 100<<" %"<<endl;

        // Now I create the efficiency histograms
        // Create a clone to store the result of the divisions:
        // Efficiency
        TH1F *hist_efficiency_drift = (TH1F*) hist_xdrift_flash->Clone("hist_efficiency_drift");
        hist_efficiency_drift->SetTitle("Efficiency vs drift");
        TH1F *hist_efficiency_Evis = (TH1F*) hist_Evis_flash->Clone("hist_efficiency_Evis");
        hist_efficiency_Evis->SetTitle("Efficiency vs Evis");

        // Perform bin-by-bin division
        // Efficiency
        hist_efficiency_drift->Divide(hist_xdrift_all);
        hist_efficiency_Evis->Divide(hist_Evis_all);

        // Now the histograms are ready to be tuned and saved
        TuneHist_eff(hist_efficiency_drift);
        hist_efficiency_drift->GetXaxis()->SetTitle("<d_{drift}> (cm)");
        hist_efficiency_drift->GetYaxis()->SetTitle("Efficiency");

        TuneHist_eff(hist_efficiency_Evis);
        hist_efficiency_Evis->GetXaxis()->SetTitle("E_{vis} (MeV)");
        hist_efficiency_Evis->GetYaxis()->SetTitle("Efficiency");

        if(file.find("OldMC") != std::string::npos){
            // Show the number of events
            TPaveText *pt1 = new TPaveText(0.7, 0.8, 0.9, 0.9, "NDC");  
            pt1->SetFillColor(0);   // transparent background
            pt1->SetBorderSize(1);  // small border
            pt1->SetTextSize(0.05);
            pt1->SetTextAlign(12);
            std::string msg1 = "# Events: " + std::to_string((int)hist_xdrift_all->GetEntries());
            // Add text line
            pt1->AddText(msg1.c_str());

            // Show the efficiency
            TPaveText *pt2 = new TPaveText(0.7, 0.8, 0.9, 0.9, "NDC");  
            pt2->SetFillColor(0);   // transparent background
            pt2->SetBorderSize(1);  // small border
            pt2->SetTextSize(0.05);
            pt2->SetTextAlign(12);
            // show the efficiency with 2 decimals
            double efficiency = (double)total_num_flashed_interactions / (double)total_num_interactions * 100.0;
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(2) << efficiency;  // two decimals
            std::string msg2 = "Efficiency: " + oss.str() + "%";
            pt2->AddText(msg2.c_str());

            // Plot the # of events
            pad1->cd();
            pad1->SetLogy();  // log scale only on the event histogram pad
            TuneHist(hist_xdrift_all);
            hist_xdrift_all->GetYaxis()->SetMoreLogLabels();
            hist_xdrift_all->SetTitle("Old MC");
            hist_xdrift_all->GetXaxis()->SetTitle("");
            hist_xdrift_all->GetYaxis()->SetTitle("# Events");
            hist_xdrift_all->Draw("hist");
            pt1->Draw("SAME");
            
            // Plot the efficiency
            pad2->cd();
            pad2->SetGrid();          // enables both x and y grids
            hist_efficiency_drift->SetTitle("Efficiency");
            hist_efficiency_drift->GetYaxis()->SetTitle("Efficiency");
            hist_efficiency_drift->Draw("E3");   // shadow
            hist_efficiency_drift->Draw("P SAME");  // points on top
            hist_efficiency_drift->SetMinimum(0.3);
            hist_efficiency_drift->SetMaximum(1.7);

            TLine* l1 = new TLine(0, 1, 200, 1);            // Line to show the efficiency=1
            TuneLine_eff(l1);
            l1->Draw("SAME");
            pt2->Draw("SAME");
            c1->SaveAs("Efficiency_all/OldMC/Efficiency_drift_nu_highEvis.pdf");

            // Plot the # of events
            pad1->cd();
            pad1->SetLogy();  // log scale only on the event histogram pad
            TuneHist(hist_Evis_all);
            hist_Evis_all->GetYaxis()->SetMoreLogLabels();
            hist_Evis_all->SetTitle("Old MC");
            hist_Evis_all->GetXaxis()->SetTitle("");
            hist_Evis_all->GetYaxis()->SetTitle("# Events");
            hist_Evis_all->Draw("hist");
            pt1->Draw("SAME");

            // Plot the efficiency
            pad2->cd();
            pad2->SetGrid();          // enables both x and y grids
            hist_efficiency_Evis->SetTitle("Efficiency");
            hist_efficiency_Evis->GetYaxis()->SetTitle("Efficiency");
            hist_efficiency_Evis->Draw("E3");   // shadow
            hist_efficiency_Evis->Draw("P SAME");  // points on top  
            hist_efficiency_Evis->SetMinimum(0.3);
            hist_efficiency_Evis->SetMaximum(1.7);

            TLine* l2 = new TLine(0, 1, 2000, 1);            // Line to show the efficiency=1
            TuneLine_eff(l2);     
            l2->Draw("SAME");
            pt2->Draw("SAME");
            c1->SaveAs("Efficiency_all/OldMC/Efficiency_Evis_nu_highEvis.pdf");
        }else if(file.find("NewMC") != std::string::npos){
            // Show the number of events
            TPaveText *pt1 = new TPaveText(0.7, 0.8, 0.9, 0.9, "NDC");  
            pt1->SetFillColor(0);   // transparent background
            pt1->SetBorderSize(1);  // small border
            pt1->SetTextSize(0.05);
            pt1->SetTextAlign(12);
            std::string msg1 = "# Events: " + std::to_string((int)hist_xdrift_all->GetEntries());
            // Add text line
            pt1->AddText(msg1.c_str());

            // Show the efficiency
            TPaveText *pt2 = new TPaveText(0.7, 0.8, 0.9, 0.9, "NDC");  
            pt2->SetFillColor(0);   // transparent background
            pt2->SetBorderSize(1);  // small border
            pt2->SetTextSize(0.05);
            pt2->SetTextAlign(12);
            // show the efficiency with 2 decimals
            double efficiency = (double)total_num_flashed_interactions / (double)total_num_interactions * 100.0;
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(2) << efficiency;  // two decimals
            std::string msg2 = "Efficiency: " + oss.str() + "%";
            pt2->AddText(msg2.c_str());

            // Plot the # of events
            pad1->cd();
            pad1->SetLogy();  // log scale only on the event histogram pad
            TuneHist(hist_xdrift_all);
            hist_xdrift_all->GetYaxis()->SetMoreLogLabels();
            hist_xdrift_all->SetTitle("Fall Production validation BNB+cosmics");
            hist_xdrift_all->GetXaxis()->SetTitle("");
            hist_xdrift_all->GetYaxis()->SetTitle("# Events");
            hist_xdrift_all->Draw("hist");
            pt1->Draw("SAME");

            // Plot the efficiency
            pad2->cd();
            pad2->SetGrid();          // enables both x and y grids
            hist_efficiency_drift->SetTitle("Efficiency");
            hist_efficiency_drift->Draw("E3");   // shadow
            hist_efficiency_drift->Draw("P SAME");  // points on top
            hist_efficiency_drift->SetMinimum(0.3);
            hist_efficiency_drift->SetMaximum(1.7);

            TLine* l1 = new TLine(0, 1, 200, 1);            // Line to show the efficiency=1
            TuneLine_eff(l1);
            l1->Draw("SAME");
            pt2->Draw("SAME");
            c1->SaveAs("Efficiency_all/NewMC/Efficiency_drift_nu_highEvis.pdf");

            // Plot the # of events
            pad1->cd();
            pad1->SetLogy();  // log scale only on the event histogram pad
            TuneHist(hist_Evis_all);
            hist_Evis_all->GetYaxis()->SetMoreLogLabels();
            hist_Evis_all->SetTitle("Fall Production validation BNB+cosmics");
            hist_Evis_all->GetXaxis()->SetTitle("");
            hist_Evis_all->GetYaxis()->SetTitle("# Events");
            hist_Evis_all->Draw("hist");
            pt1->Draw("SAME");

            // Plot the efficiency
            pad2->cd();
            pad2->SetGrid();          // enables both x and y grids
            hist_efficiency_Evis->SetTitle("Efficiency");
            hist_efficiency_Evis->Draw("E3");   // shadow
            hist_efficiency_Evis->Draw("P SAME");  // points on top
            hist_efficiency_Evis->SetMinimum(0.3);
            hist_efficiency_Evis->SetMaximum(1.7);

            TLine* l2 = new TLine(0, 1, 2000, 1);            // Line to show the efficiency=1
            TuneLine_eff(l2);     
            l2->Draw("SAME");
            pt2->Draw("SAME");
            c1->SaveAs("Efficiency_all/NewMC/Efficiency_Evis_nu_highEvis.pdf");
        }
    } // Finish loop over files
   
}
