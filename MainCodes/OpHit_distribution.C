/*
Code for plotting the distribution of OpHits
*/

// INCLUDES
// Definition of the variables of the tree
#include "/Users/carlosmm/Documents/Doctorado/ServiceTask/Task/tree_definitions_light/tree_utils.cpp"
#include "/Users/carlosmm/Documents/Doctorado/ServiceTask/Task/tree_definitions_light/Includes.h"
// Definition of the constants
#include "/Users/carlosmm/Documents/Doctorado/ServiceTask/Task/Constants/Constants.h"
// Definition of the functions to tune the histograms and lines and arrows
#include "/Users/carlosmm/Documents/Doctorado/ServiceTask/Task/Functions/TuneFunctions.cpp"


// MAIN CODE
void OpHit_distribution(){
    // Declaration of the canvas
    TCanvas *c1= new TCanvas("name", "OpHit distribution", 1000, 800);  

    // Declaration of the histogram --> distribution of OpHits normalized to the number of photo-electrons
    std::vector<TH1F*> hists;               // vector to save the histogram
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
    filenames.push_back("/Users/carlosmm/Documents/Doctorado/ServiceTask/Task/Data/opana_tree.root");

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

        cout << "-------- You are reading the file: " << file << "------" <<endl;

        // define more variables here

        double abs_time;    // value of the time of the OpFlash

        double start_time, finish_time;     // start and finish time of the OpFlash
        double ymin, ymax;                  // maximum and minimum value of the gPad
        int max_OpHit;                      // maximum value of the OpHit of a certain OpFlah

        // Loop over all entries of the TTree
        int num_events=event_tree->GetEntries();
        for (int i=0; i<num_events; i++){

            // I loop over the particles to see the 'creation process' of each one
            /*
            for(int j=0; j<process->size(); j++){
                cout<<"Particle "<<trackID->at(j)<<" with PDG code "<<PDGcode->at(j)<<" was created by "<<process->at(j)<<endl;
                cout<<"      The mother of this particle is: "<<motherID->at(j)<<endl;
            }
            cout<<endl<<endl;
            */

            // Get the entry 'i' of the event tree
            event_tree->GetEntry(i);

            // Range of the histogram. Change accordingly to what you may want
            double min_hist = -100;
            double max_hist = 100;

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
            TPaveText *pt = new TPaveText(0.65, 0.84, 0.9, 0.9, "NDC");  
            pt->SetFillColor(0);   // transparent background
            pt->SetBorderSize(1);  // small border
            pt->SetTextSize(0.03);
            pt->SetTextAlign(12);

            // Build the message
            std::string msg = "# OpFlashes: " + std::to_string(nopflash);

            // Add text line
            pt->AddText(msg.c_str());

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

    } // Finish loop over files

    
    // I loop over all histograms
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
            lines_start.at(k).at(h)->Draw("same");
            lines_finish.at(k).at(h)->Draw("same");
            arrows.at(k).at(h)->Draw("same");
        }

        // I save the histogram
        c1->SaveAs(Form("Histograms/Distribution_%i.pdf", k));
    }
   
}