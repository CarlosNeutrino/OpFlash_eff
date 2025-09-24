/////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////// Functions to tune histograms, plots, lines, ... ////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

//Function to tune the histograms (all except the colors, that I will choose)
void TuneHist(TH1F* hist){
    hist->SetStats(0);

    hist->GetXaxis()->SetTitleSize(0.04);
    hist->GetYaxis()->SetTitleSize(0.04);
    hist->GetXaxis()->SetLabelSize(0.03);
    hist->GetYaxis()->SetLabelSize(0.03);

    hist->SetLineStyle(1);
    hist->SetLineWidth(2);
    hist->SetFillStyle(1001);       //1001-->filling, 0-->no filling 
    hist->SetMarkerColor(kBlack);
    hist->SetMarkerStyle(21);
    hist->SetMarkerSize(0.);

    hist->SetLineColor(kBlue);
    hist->SetFillColorAlpha(kBlue-3, 0.75);

    hist->GetXaxis()->SetTitle("Absolute time (#mu s)");
    hist->GetYaxis()->SetTitle("# photo-electrons (all channels)");

    hist->GetXaxis()->SetTitleOffset(0.88);  // Adjust the offset of the X-axis title
    hist->GetYaxis()->SetTitleOffset(1.3);  // Adjust the offset of the Y-axis title
    
}

// Function to tune the plots (TF1)

// Function to tune the lines that mark the start of the OpFlash
// functions to tune the lines
void TuneLine_start(TLine* l){
    l->SetLineColor(kRed);
    l->SetLineStyle(1);         // Solid line
    l->SetLineWidth(1);
}

// Function to tune the lines that mark the finish of the OpFlash   
void TuneLine_finish(TLine* l){
    l->SetLineColor(kRed);
    l->SetLineStyle(2);         // Dashed line
    l->SetLineWidth(1);
}

// functions to tune the arrows
void TuneArrow(TArrow* arrow){
    arrow->SetLineColor(kRed-2);
    arrow->SetLineWidth(1);
}
