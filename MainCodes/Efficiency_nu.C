/*
Code for plotting cout'ing some correlations between flash variables and interactions (cosmic, neutrino, michelle e-)
*/

// INCLUDES
// Definition of the variables of the tree
#include "/Users/carlosmm/Documents/Doctorado/ServiceTask/Task/tree_definitions_light/tree_utils.cpp"
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
void Efficiency_nu(){
    // Declaration of the canvas
    TCanvas *c1= new TCanvas("name", "Efficiency", 1000, 800);  

    // Declaration of the histograms --> efficiency of detecting neutrino interactions with the opFlash
    /*
    - total: all neutrino interactions
    - flashed: neutrino interactions that have one corresponding OpFlash
    */

    // enables automatic error storage for all subsequent TH1 objects
    TH1::SetDefaultSumw2();  

    // neutrinos that SHOULD be reconstructed: vertex inside TPC
    TH1F *hist_Enu_true_TPC = new TH1F("Enu_total", "Eff_hist_Enu_total",  30, 0, 5000);
    TH1F *hist_drift_true_TPC = new TH1F("drift_total", "Eff_hist_drift_total",  30, 0, 200);
    TH1F *hist_Evis_true_TPC = new TH1F("Evis_total", "Eff_hist_Evis_total",  30, 0, 5000);
    TH1F *hist_MeanDrift_true_TPC = new TH1F("MeanDrift_total", "Eff_hist_MeanDrift_total",  30, 0, 200);

    // neutrinos that leave an OpFlash: vertex MAYBE NOT inside TPC + have one OpFlash
    TH1F *hist_Enu_flashed = new TH1F("Enu_flashed", "Eff_hist_Enu_flashed",  30, 0, 5000);
    TH1F *hist_drift_flashed = new TH1F("drift_flashed", "Eff_hist_drift_flashed",  30, 0, 200);
    TH1F *hist_Evis_flashed = new TH1F("Evis_flashed", "Eff_hist_Evis_flashed",  30, 0, 5000);
    TH1F *hist_MeanDrift_flashed = new TH1F("MeanDrift_flashed", "Eff_hist_MeanDrift_flashed",  30, 0, 200);

    // neutrinos that are reconstructed and SHOULD be reconstrcted: vertex inside TPC + have OpFlash
    TH1F *hist_Enu_flashed_TPC = new TH1F("Enu_flashed_TPC", "Eff_hist_Enu_flashed_TPC",  30, 0, 5000);
    TH1F *hist_drift_flashed_TPC = new TH1F("drift_flashed_TPC", "Eff_hist_drift_flashed_TPC",  30, 0, 200);
    TH1F *hist_Evis_flashed_TPC = new TH1F("Evis_flashed_TPC", "Eff_hist_Evis_flashed_TPC",  30, 0, 5000);
    TH1F *hist_MeanDrift_flashed_TPC = new TH1F("MeanDrift_flashed_TPC", "Eff_hist_MeanDrift_flashed_TPC",  30, 0, 200);

    // neutrinos that are NOT reconstructed and SHOULD be reconstrcted: vertex inside TPC + NO OpFlash
    TH1F *hist_Enu_lost = new TH1F("Enu_lost", "Eff_hist_Enu_lost",  30, 0, 5000);   
    TH1F *hist_drift_lost = new TH1F("drift_lost", "Eff_hist_drift_lost",  30, 0, 200);
    TH1F *hist_Evis_lost = new TH1F("Evis_lost", "Eff_hist_Evis_lost",  30, 0, 5000); 
    TH1F *hist_MeanDrift_lost = new TH1F("MeanDrift_lost", "Eff_hist_MeanDrift_lost",  30, 0, 200); 

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

    // declaration of the OpAna TTree
    TTree *event_tree;
    TTree *pdsmap_tree;

    // Declaration of the files 
    std::vector<string> filenames;
    filenames.push_back("/Users/carlosmm/Documents/Doctorado/ServiceTask/Task/Data/opana_tree_big.root");

    // Variables to manage the file and the directory of its tree
    TFile *input_file;
    TDirectoryFile *tree_dir;

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
        ////// Variables for the OpHit distribution //////
        double abs_time;    // value of the time of the OpFlash
        double start_time, finish_time;     // start and finish time of the OpFlash
        double ymin, ymax;                  // maximum and minimum value of the gPad
        int max_OpHit;                      // maximum value of the OpHit of a certain OpFlah

        ////// Variables for efficiency plots //////
        int num_neutrinos_reco;         // Event by event count of the reconstructed neutrinos
        int num_neutrinos_true;         // Event by event count of the true neutrinos inside the TPC

        int num_neutrinos_flashed_TPC = 0;               // number of neutrinos in the event that have a corresponding OpFlash and the interaction vertex is inside the TPC
        int num_neutrinos_flashed = 0;                   // number of neutrinos in the event that have a corresponding OpFlash and the interaction vertex MAY NOT BE inside the TPC
        int num_neutrinos_true_TPC = 0;                  // number of true neutrinos inside the TPC in the event
        int num_neutrinos_lost = 0;                      // number of neutrinos in the event that do NOT have a corresponding OpFlash and the interaction vertex is inside the TPC    

        double Enu;                    // visible energy of the neutrino interaction
        double drift;                   // drift distance of the neutrino interaction
        bool nu_in_TPC;                 // boolean to know if the neutrino interaction is inside the TPC   

        int matched_ID = -1;               // ID of a particle of an interaction that matches with the OpFlash
        int previous_matched_ID = -1;      // previous matched ID to check if a neutrino produces 2 OpFlashes
        int nu_index = -1;                 // index of the neutrino that caused the interaction
        std::vector<double> time_nu;        // vector to save the time of the neutrinos that have produced an OpFlash

        // Loop over all entries of the TTree
        int num_events=event_tree->GetEntries();
        int Niters = num_events;
        for (int i=0; i<Niters; i++){
            num_neutrinos_reco=0;    // Each event I reset the number of neutrinos that have been flashed
            num_neutrinos_true=0;    // Each event I reset the number of true neutrinos inside the TPC
            nu_in_TPC=false;         // I reset the boolean that tells me if the neutrino interaction is inside the TPC

            // Get the entry 'i' of the event tree
            event_tree->GetEntry(i);

            time_nu.clear(); // I reset the vector that saves the time of the neutrinos that have produced an OpFlash
            for(int j=0; j<nuvX->size(); j++){
                if( nuvX->at(j)>X_MIN && nuvX->at(j)<X_MAX && 
                    nuvY->at(j)>Y_MIN && nuvY->at(j)<Y_MAX && 
                    nuvZ->at(j)>Z_MIN && nuvZ->at(j)<Z_MAX)
                {
                    nu_in_TPC=true;
                }
            }

            if(nu_in_TPC==true){
                cout<<endl<<"           -------- Event number: " << i << "------" <<endl<<endl;



                if(nuvX->size()>=2){
                    for(int j=0; j<trackID->size(); j++){
                        if(trackID->at(j)<20000000 ) // I only print the primary particles from the neutrino interaction
                            cout<<"Particle "<<trackID->at(j)<<" with PDG code "<<PDGcode->at(j)<<" has mother: "<<motherID->at(j)<<endl;
                    }
                }
        
                // I loop over the particles to see the 'creation process' of each one
                /*
                for(int j=0; j<process->size(); j++){
                    if(process->at(j)=="primary" && trackID->at(j)<20000000){ // I only print the primary particles of the interaction that are not cosmics (cosmics have ID > 2e7)
                        cout<<"Particle "<<trackID->at(j)<<" with PDG code "<<PDGcode->at(j)<<" was created by "<<process->at(j)<<endl;
                        cout<<"      It was created at time: "<<MCP_time->at(j)/1000<<" mu-s"<<endl<<endl;
                    }
                }
                cout<<endl<<endl;
                */
                
                /////////////////////////////////////////////////////////////////////////////////////
                ////// Calculate the # of neutrinos in the event that have produced an OpFlash //////
                /////////////////////////////////////////////////////////////////////////////////////
        
                // Match the flashes with particles. See if they are neutrino
                for(int j=0; j<nopflash; j++){
                    // I match the OpFlash with the interaction. SO FAR ONLY WORKS FOR NEUTRINO INTERACTIONS
                    matched_ID = OpFlash_interaction_matcher(flash_time->at(j), event_tree, i, nu_index);

                    //cout<<"The ID match is: " <<matched_ID<<endl;
                    // I check if the interaction is neutrino
                    if(matched_ID<20000000 && matched_ID!=-1){ // The cosmics are the only particles with ID > 2e7. If no match is found, ID=-1

                        //cout<<"The matched neutrino is: "<<matched_ID<<" with index "<<nu_index<<endl;

                        /*
                        I need to take into account that:
                        - 1 neutrino can produce 2 OpFlashes if it crosses from 1 TPC to another
                        - There may be interactions with just 1 neutrino outside the TPC that produce an OpFlash.
                            In this case, I do not want to count them out for the purity calculation
                        */
                        if(matched_ID!=previous_matched_ID){  // I check if the neutrino has already produced an OpFlash
                        
                            num_neutrinos_reco++;    // Number of neutrinos that have produced an OpFlash IN THIS INTERACTION
                            
                            // Find the Enu and drift of the neutrino that caused this interaction
                            drift = 200 - std::abs(nuvX->at(nu_index));   // Distance of the particle with the anode plane in the drift axis 
                            Enu = nuvE->at(nu_index) * 1000;             // Visible energy in MeV (nuvE is in GeV)

                            // I fill the neutrino-only efficiency histograms
                            hist_Enu_flashed->Fill(Enu);
                            hist_drift_flashed->Fill(drift);
                            num_neutrinos_flashed++;

                            /////////////////////////////////////////////////////////////////////////////////////
                            //////////////// Of these, see how many of them were inside the TPC /////////////////
                            /////////////////////////////////////////////////////////////////////////////////////

                            if(nuvX->at(nu_index)>X_MIN && nuvX->at(nu_index)<X_MAX && 
                                nuvY->at(nu_index)>Y_MIN && nuvY->at(nu_index)<Y_MAX && 
                                nuvZ->at(nu_index)>Z_MIN && nuvZ->at(nu_index)<Z_MAX)
                            {
                                
                                // I fill the neutrino-only efficiency histograms for neutrinos inside the TPC
                                hist_Enu_flashed_TPC->Fill(Enu);
                                hist_drift_flashed_TPC->Fill(drift);
                                num_neutrinos_flashed_TPC++;


                                // I save in a vector the time nuvT of this neutrino
                                time_nu.push_back(nuvT->at(nu_index)); // I save the time of the neutrino that has produced an OpFlash
                            }
                        }

                    }else{  // In case it is not a neutrino
                        matched_ID=previous_matched_ID; // I keep the previous matched ID if the current one is not a neutrino. this way I always save the last matched neutrino interaction
                    }
                    // I save the previous matched ID to check if a neutrino produces 2 OpFlashes (and not count it twice)
                    previous_matched_ID = matched_ID;
                }

                // I fill the efficiency histograms for ALL the neutrino, also the ones that have not been flashed
                for (int k=0; k<nuvE->size(); k++){

                    if(nuvX->at(k)>X_MIN && nuvX->at(k)<X_MAX && 
                    nuvY->at(k)>Y_MIN && nuvY->at(k)<Y_MAX && 
                    nuvZ->at(k)>Z_MIN && nuvZ->at(k)<Z_MAX){
                        num_neutrinos_true++; // I add the number of real (true) neutrinos in the event that happened inside the TPC

                        Enu = nuvE->at(k) * 1000;             // Visible energy in MeV (nuvE is in GeV)
                        drift = 200 - std::abs(nuvX->at(k));   // Distance of the particle with the anode plane in the drift axis 

                        // Fill the histograms
                        hist_Enu_true_TPC->Fill(Enu);
                        hist_drift_true_TPC->Fill(drift);
                        num_neutrinos_true_TPC++;

                        // I check if the neutrino has been found in the previous loop
                        if( std::find(time_nu.begin(), time_nu.end(), nuvT->at(k)) == time_nu.end() ){
                            // If the time of this neutrino is not in the vector, it means it has not produced any OpFlash
                            // I fill the histograms of lost neutrinos
                            hist_Enu_lost->Fill(Enu);
                            hist_drift_lost->Fill(drift);
                            num_neutrinos_lost++;
                        }
                    }
                } 
            } // Finish if(nu_in_TCPC==true)

            // I have finished calculating the variables for the efficiency plots for this event
            cout<<"----- Efficiency stats: -----"<<endl;

            cout<<"-----------------------------"<<endl;
            cout<<"EFFICIENCY: "<<(double)num_neutrinos_flashed_TPC/(double)num_neutrinos_true_TPC * 100<<" %"<<endl;
            cout<<"PURITY: "<<(double)num_neutrinos_flashed_TPC/(double)num_neutrinos_flashed * 100<<" %"<<endl;
            cout<<"LOST RATIO: "<<(double)num_neutrinos_lost/(double)num_neutrinos_true_TPC * 100<<" %"<<endl;

            // In case I lose a neutrino, plot the distribution of OpHits to see why and how
            if(num_neutrinos_true!=num_neutrinos_reco && num_neutrinos_true!=0){

                // Create a NEW histogram for this event
                TString hname = Form("Histogram_%d", i);
                TH1F *h = new TH1F(hname, "OpHit distribution",  (max_hist - min_hist)/BIN_SIZE, min_hist, max_hist);

                // I reset the histogram that had been created 
                h->Reset();

                // Analysis of the variables
                // I loop over all OpHits that have been registered
                for(int j=0; j<nophits; j++){
                    // I fill each histogram with the time of the OpHit. 
                    // I weight the histogram with the number of photo-electrons in each OpHit
                    abs_time = ophit_startT->at(j) + ophit_riseT->at(j);
                    h->Fill( abs_time, ophit_pe->at(j) );
                }

                // I write the number of OpFlashes in each histogram (each event)
                // Define the box position in NDC (normalized device coordinates)
                TPaveText *pt = new TPaveText(0.6, 0.75, 0.9, 0.9, "NDC");  
                pt->SetFillColor(0);   // transparent background
                pt->SetBorderSize(1);  // small border
                pt->SetTextSize(0.02);
                pt->SetTextAlign(12);

                // Build the message
                std::string msg_1 = "# Event: " + std::to_string(i);
                std::string msg_2 = "# Neutrinos in event: " + std::to_string(num_neutrinos_true);


                // I say if there was an InTimeCosmic in the event
                std::string InTimeCosmic_Str;
                if (InTimeCosmics==true) {
                    InTimeCosmic_Str= "true";
                    cout<<"   There was an InTimeCosmic in the event"<<endl<<endl<<endl<<endl<<endl;
                } else {
                    InTimeCosmic_Str = "false";
                }
                std::string msg_3 = std::string("# There is InTimeCosmic: ") + InTimeCosmic_Str;
                std::string msg_4 = std::string("# Neutrinos reconstructed (inside TPC): ") + std::to_string(num_neutrinos_reco);

                // Add text line
                pt->AddText(msg_1.c_str());
                pt->AddText(msg_2.c_str());
                pt->AddText(msg_3.c_str());
                pt->AddText(msg_4.c_str());


                // I fill the vector with the histogram and TPaveText of each event
                hists.push_back(h);
                pts.push_back(pt);

                // I loop over the OpFlashes to plot the veto time
                for(int j=0; j<nopflash; j++){
                    // I get the start and end time of the flash

                    start_time = flash_time->at(j);
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
                }

                // I fill the vector of vectors of lines and arrows
                lines_start.push_back(lines_start_ev);
                lines_finish.push_back(lines_finish_ev);
                arrows.push_back(arrows_ev);

                // I reset the TPave text at the end of each event
                lines_start_ev.clear();
                lines_finish_ev.clear();
                arrows_ev.clear();
            }
        }

        // Now I create the efficiency and purity histograms

        // Create a clone to store the result of the divisions:
        // Efficiency
        TH1F *hist_efficiency_drift = (TH1F*) hist_drift_flashed_TPC->Clone("hist_efficiency_drift");
        hist_efficiency_drift->SetTitle("Efficiency vs drift");
        TH1F *hist_efficiency_Enu = (TH1F*) hist_Enu_flashed_TPC->Clone("hist_efficiency_Enu");
        hist_efficiency_Enu->SetTitle("Efficiency vs Enu");

        // Purity
        TH1F *hist_purity_drift = (TH1F*) hist_drift_flashed_TPC->Clone("hist_purity_drift");
        hist_purity_drift->SetTitle("Purity vs drift");
        TH1F *hist_purity_Enu = (TH1F*) hist_Enu_flashed_TPC->Clone("hist_purity_Enu");
        hist_purity_Enu->SetTitle("Purity vs Enu");

        // Fraction of lost neutrinos
        TH1F *hist_drift_lost_ratio = (TH1F*) hist_drift_lost->Clone("hist_drift_lost_ratio");  
        hist_drift_lost_ratio->SetTitle("Fraction of lost neutrinos vs drift");
        TH1F *hist_Enu_lost_ratio = (TH1F*) hist_Enu_lost->Clone("hist_Enu_lost_ratio");
        hist_Enu_lost_ratio->SetTitle("Fraction of lost neutrinos vs Enu");

        // Perform bin-by-bin division
        // Efficiency
        hist_efficiency_drift->Divide(hist_drift_true_TPC);
        hist_efficiency_Enu->Divide(hist_Enu_true_TPC);

        // Purity
        hist_purity_drift->Divide(hist_drift_flashed);
        hist_purity_Enu->Divide(hist_Enu_flashed);

        // Lost neutrinos
        hist_drift_lost_ratio->Divide(hist_drift_true_TPC);
        hist_Enu_lost_ratio->Divide(hist_Enu_true_TPC);

        // Now the histograms are ready to be tuned and saved
        TuneHist(hist_efficiency_drift);
        hist_efficiency_drift->GetXaxis()->SetTitle("Drift distance neutrino vertex (cm)");
        hist_efficiency_drift->GetYaxis()->SetTitle("Efficiency");

        TuneHist(hist_efficiency_Enu);
        hist_efficiency_Enu->GetXaxis()->SetTitle("Neutrino energy (MeV)");
        hist_efficiency_Enu->GetYaxis()->SetTitle("Efficiency");

        TuneHist(hist_purity_drift);
        hist_purity_drift->GetXaxis()->SetTitle("Drift distance neutrino vertex (cm)");
        hist_purity_drift->GetYaxis()->SetTitle("Purity");

        TuneHist(hist_purity_Enu);
        hist_purity_Enu->GetXaxis()->SetTitle("Neutrino energy (MeV)");
        hist_purity_Enu->GetYaxis()->SetTitle("Purity");

        TuneHist(hist_drift_lost_ratio);
        hist_drift_lost_ratio->GetXaxis()->SetTitle("Drift distance neutrino vertex (cm)");
        hist_drift_lost_ratio->GetYaxis()->SetTitle("Fraction lost neutrinos");

        TuneHist(hist_Enu_lost_ratio);
        hist_Enu_lost_ratio->GetXaxis()->SetTitle("Neutrino energy (MeV)");
        hist_Enu_lost_ratio->GetYaxis()->SetTitle("Fraction lost neutrinos");

        // I save the histograms
        hist_efficiency_drift->Draw("pe");
        c1->SaveAs("Efficiency_nu/Efficiency_drift.pdf");
        hist_efficiency_Enu->Draw("pe");
        c1->SaveAs("Efficiency_nu/Efficiency_Enu.pdf");
        hist_purity_drift->Draw("pe");
        c1->SaveAs("Efficiency_nu/Purity_drift.pdf");
        hist_purity_Enu->Draw("pe");
        c1->SaveAs("Efficiency_nu/Purity_Enu.pdf");
        hist_drift_lost_ratio->Draw("pe");
        c1->SaveAs("Efficiency_nu/LostNeutrinos_drift.pdf");
        hist_Enu_lost_ratio->Draw("pe");
        c1->SaveAs("Efficiency_nu/LostNeutrinos_Enu.pdf");

    } // Finish loop over files

    
    // I loop over all histograms of the distribution of OpHits when a neutrino is lost
    for(int k=0; k<hists.size(); k++){
        // I draw the histogram 
        TuneHist(hists.at(k));
        hists.at(k)->Draw("hist");
        // Get the maximum of the hists
        double line_max = hists.at(k)->GetMaximum();

        // Draw on canvas
        pts.at(k)->Draw("same");

        // I draw the lines and arrows
        for(int h=0; h<lines_start.at(k).size(); h++){
            // Redefine ymax of the lines according to the canvas
            lines_start.at(k).at(h)->SetY2(line_max);
            lines_finish.at(k).at(h)->SetY2(line_max);

            // I tune the lines and arrow to look better
            TuneLine_start(lines_start.at(k).at(h));
            TuneLine_finish(lines_finish.at(k).at(h));
            TuneArrow(arrows.at(k).at(h));

            // Redefine the y of the arrows
            double arrow_y = line_max * 0.90; // 90% of max height
            arrows.at(k).at(h)->SetY1(arrow_y);
            arrows.at(k).at(h)->SetY2(arrow_y);

            // Draw on canvas
            if(lines_start.at(k).at(h)->GetY1()>min_hist && lines_start.at(k).at(h)->GetY1()<max_hist){  // I only draw the lines if they are in the range of the histogram
                lines_start.at(k).at(h)->Draw("same");
                lines_finish.at(k).at(h)->Draw("same");
                arrows.at(k).at(h)->Draw("same");
            }
        }

        // I save the histogram
        c1->SaveAs(Form("NeutrinoMissing/Distribution_%i.pdf", k));
    }
   
}
