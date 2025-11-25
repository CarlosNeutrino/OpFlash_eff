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
void OpHit_width(){
    // Declaration of the canvas
    TCanvas *c1= new TCanvas("name", "OpHit distribution", 1000, 800);  

    // Range of the histogram. Change accordingly to what you may want
    double min_hist = 0.;
    double max_hist = 0.4;

    // Declaration of the histogram --> distribution of OpHits normalized to the number of photo-electrons
    TH1F *hist_fast = new TH1F("OpHit width distribution", "OpHit width distribution",  20, min_hist, max_hist);
    TH2F *hist2D_fast = new TH2F("OpHit width vs PE", "OpHit width vs PE",  20, min_hist, max_hist, 20, 1., 500.);

    TH1F *hist_slow = new TH1F("OpHit width distribution", "OpHit width distribution",  20, min_hist, max_hist);
    TH2F *hist2D_slow = new TH2F("OpHit width vs PE", "OpHit width vs PE",  20, min_hist, max_hist, 20, 1., 500.);

    // declaration of the OpAna TTree
    TTree *event_tree;
    TTree *pdsmap_tree;

    // Declaration of the files 
    std::vector<string> filenames;
    filenames.push_back("/Users/carlosmm/Documents/Doctorado/ServiceTask/Task/Data/v10_06/opana_tree_big.root");

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

        // Loop over all entries of the TTree
        int num_events=event_tree->GetEntries();
        num_events=2000;  // for testing purposes, I only use 200 events
        for (int i=0; i<num_events; i++){
            // Get the entry 'i' of the event tree
            event_tree->GetEntry(i);

            // I loop over one OpFlash
            for(int f=0; f<nopflash; f++){
                // I calculate the time of the FIRST OpHit of the OpFlash
                double min_time = 1e9;
                for(int j=0; j<flash_ophit_starttime->at(f).size(); j++){
                    abs_time = flash_ophit_starttime->at(f).at(j) + flash_ophit_risetime->at(f).at(j);
                    if( abs_time < min_time ){
                        min_time = abs_time;
                    }
                }

                // I loop over all the OpHits of the OpFlash
                for(int j=0; j<flash_ophit_starttime->at(f).size(); j++){
                    double ophit_time = flash_ophit_starttime->at(f).at(j) + flash_ophit_risetime->at(f).at(j);
                    double ophit_width = flash_ophit_width->at(f).at(j);
                    double ophit_pe = flash_ophit_pe->at(f).at(j);

                    // I fill the histogram only with OpHits that are within a certain time from the first OpHit
                    if( ((ophit_time - min_time) > 0) && ((ophit_time - min_time) <  FAST_LIGHT_TIME) ){
                        hist_fast->Fill(ophit_width);
                        hist2D_fast->Fill(ophit_width, ophit_pe);
                    }else if( ((ophit_time - min_time) > FAST_LIGHT_TIME) && ((ophit_time - min_time) <  FAST_LIGHT_TIME + SLOW_LIGHT_TIME) ){
                        hist_slow->Fill(ophit_width);
                        hist2D_slow->Fill(ophit_width, ophit_pe);
                    }
                } // Finish loop over OpHits of the OpFlash
            }
        }

    } // Finish loop over files

    
    // I draw the histogram 1D --> fast light
    TuneHist_OpHitDistribution(hist_fast);
    hist_fast->GetXaxis()->SetTitle("Width of the OpHit (#mu s)");
    hist_fast->GetYaxis()->SetTitle("# events");
    hist_fast->Draw("hist");
    // I save the histogram
    c1->SaveAs("Width_OpHit/FastLight/OpHit_width.pdf");

    // I draw the histogram 1D --> slow light
    TuneHist_OpHitDistribution(hist_slow);
    hist_slow->GetXaxis()->SetTitle("Width of the OpHit (#mu s)");
    hist_slow->GetYaxis()->SetTitle("# events");
    hist_slow->Draw("hist");
    // I save the histogram
    c1->SaveAs("Width_OpHit/SlowLight/OpHit_width.pdf");

    // I draw the histogram 2D --> fast light
    c1->SetRightMargin(0.15);    // larger so the titles fit inside
    TuneHist_2D(hist2D_fast);
    hist2D_fast->GetZaxis()->SetTitleOffset(1.3);  // Adjust the offset of the Z-axis title
    hist2D_fast->GetXaxis()->SetTitle("Width of the OpHit (#mu s)");
    hist2D_fast->GetYaxis()->SetTitle("# PE of the OpHit");
    hist2D_fast->GetZaxis()->SetTitle("# events");
    hist2D_fast->Draw("COLZ"); 
    gPad->SetLogz();   // make the z-axis logarithmic
    // I save the histogram
    c1->SaveAs("Width_OpHit/FastLight/OpHit_width_PE_2D.pdf");

    // I draw the histogram 2D --> slow light
    c1->SetRightMargin(0.15);    // larger so the titles fit inside
    TuneHist_2D(hist2D_slow);
    hist2D_slow->GetZaxis()->SetTitleOffset(1.3);  // Adjust the offset of the Z-axis title
    hist2D_slow->GetXaxis()->SetTitle("Width of the OpHit (#mu s)");
    hist2D_slow->GetYaxis()->SetTitle("# PE of the OpHit");
    hist2D_slow->GetZaxis()->SetTitle("# events");
    hist2D_slow->Draw("COLZ"); 
    gPad->SetLogz();   // make the z-axis logarithmic
    // I save the histogram
    c1->SaveAs("Width_OpHit/SlowLight/OpHit_width_PE_2D.pdf");
    
   
}