/////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////// Functions to tune histograms, plots, lines, ... ////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

//Function to tune the histograms (all except the colors, that I will choose)
void TuneHist(TH1F* hist){
    hist->SetStats(0);

    hist->GetXaxis()->SetTitleSize(0.06);
    hist->GetYaxis()->SetTitleSize(0.06);
    hist->GetXaxis()->SetLabelSize(0.05);
    hist->GetYaxis()->SetLabelSize(0.05);

    hist->SetLineStyle(1);
    hist->SetLineWidth(2);
    hist->SetFillStyle(1001);       //1001-->filling, 0-->no filling 
    hist->SetMarkerColor(kBlack);
    hist->SetMarkerStyle(21);
    hist->SetMarkerSize(0.);

    hist->SetLineColor(kBlue);
    hist->SetFillColorAlpha(kAzure-8, 0.75);

    hist->GetXaxis()->SetTitle("Absolute time (#mu s)");
    hist->GetYaxis()->SetTitle("# photo-electrons (all channels)");

    hist->GetXaxis()->SetTitleOffset(0.88);  // Adjust the offset of the X-axis title
    hist->GetYaxis()->SetTitleOffset(0.7);  // Adjust the offset of the Y-axis title
    
}

void TuneHist_OpHitDistribution(TH1F* hist){
    hist->SetStats(0);

    hist->GetXaxis()->SetTitleSize(0.04);
    hist->GetYaxis()->SetTitleSize(0.04);
    hist->GetXaxis()->SetLabelSize(0.04);
    hist->GetYaxis()->SetLabelSize(0.04);

    hist->SetLineStyle(1);
    hist->SetLineWidth(2);
    hist->SetFillStyle(1001);       //1001-->filling, 0-->no filling 
    hist->SetMarkerColor(kBlack);
    hist->SetMarkerStyle(21);
    hist->SetMarkerSize(0.);

    hist->SetLineColor(kBlue);
    hist->SetFillColorAlpha(kAzure-8, 0.75);

    hist->GetXaxis()->SetTitle("Absolute time (#mu s)");
    hist->GetYaxis()->SetTitle("# photo-electrons (all channels)");

    hist->GetXaxis()->SetTitleOffset(1.);  // Adjust the offset of the X-axis title
    hist->GetYaxis()->SetTitleOffset(1.4);  // Adjust the offset of the Y-axis title
    
}

void TuneHist_2D(TH2F* hist){
    hist->SetStats(0);

    hist->GetXaxis()->SetTitleSize(0.04);
    hist->GetYaxis()->SetTitleSize(0.04);
    hist->GetZaxis()->SetTitleSize(0.04);
    hist->GetXaxis()->SetLabelSize(0.03);
    hist->GetYaxis()->SetLabelSize(0.03);
    hist->GetZaxis()->SetLabelSize(0.03);

    hist->SetLineStyle(1);
    hist->SetLineWidth(2);
    hist->SetFillStyle(0);       //1001-->filling, 0-->no filling 
    hist->SetMarkerColor(kBlack);
    hist->SetMarkerStyle(21);
    hist->SetMarkerSize(1.);

    hist->GetXaxis()->SetTitle("PMT Z position (cm)");
    hist->GetYaxis()->SetTitle("PMT Y position (cm)");
    hist->GetZaxis()->SetTitle("# photo electrons");

    hist->GetXaxis()->SetTitleOffset(1.0);  // Adjust the offset of the X-axis title
    hist->GetYaxis()->SetTitleOffset(1.3);  // Adjust the offset of the Y-axis title
    hist->GetZaxis()->SetTitleOffset(1.4);  // Adjust the offset of the Z-axis title


}

void TuneGraph_2D(TGraph2D *gr, string title){
    gr->GetXaxis()->SetTitleSize(0.04);
    gr->GetYaxis()->SetTitleSize(0.04);
    gr->GetZaxis()->SetTitleSize(0.04);
    gr->GetXaxis()->SetLabelSize(0.03);
    gr->GetYaxis()->SetLabelSize(0.03);
    gr->GetZaxis()->SetLabelSize(0.03);

    // Set axis
    gr->GetXaxis()->SetLimits(Z_MIN, Z_MAX);  // z-axis
    gr->GetYaxis()->SetRangeUser(Y_MIN, Y_MAX);  // y-axis

    //hist->SetLineStyle(1);
    //hist->SetLineWidth(2);
    gr->SetFillStyle(1001);       //1001-->filling, 0-->no filling 
    gr->SetMarkerStyle(21);
    gr->SetMarkerSize(1.);

    std::string fullTitle = title + "; z [cm]; y [cm]; # photo electrons";
    gr->SetTitle(fullTitle.c_str());

    gr->GetXaxis()->SetTitleOffset(1.1);
    gr->GetYaxis()->SetTitleOffset(0.8);
    gr->GetZaxis()->SetTitleOffset(1.);  // Adjust the offset of the Z-axis title

    gr->SetNpx(20);  // number of bins in x (default ~40)
    gr->SetNpy(20);  // number of bins in y (default ~40)
}

void TuneHist_eff(TH1F* hist){
    hist->SetStats(0);

    hist->GetXaxis()->SetTitleSize(0.07);
    hist->GetYaxis()->SetTitleSize(0.07);
    hist->GetXaxis()->SetLabelSize(0.062);
    hist->GetYaxis()->SetLabelSize(0.062);

    hist->SetLineStyle(1);
    hist->SetLineWidth(2);
    hist->SetFillStyle(1001);       //1001-->filling, 0-->no filling 
    hist->SetMarkerColor(kBlack);
    hist->SetMarkerStyle(21);
    hist->SetMarkerSize(0.);

    hist->SetLineColor(kBlue);
    hist->SetFillColorAlpha(kAzure-8, 0.75);

    hist->GetXaxis()->SetTitle("Absolute time (#mu s)");
    hist->GetYaxis()->SetTitle("# photo-electrons (all channels)");

    hist->GetXaxis()->SetTitleOffset(0.88);  // Adjust the offset of the X-axis title
    hist->GetYaxis()->SetTitleOffset(0.7);  // Adjust the offset of the Y-axis title
    
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

void TuneLine_eff(TLine* l){
    l->SetLineColor(kRed);
    l->SetLineStyle(2);         
    l->SetLineWidth(2);
}

// functions to tune the arrows
void TuneArrow(TArrow* arrow){
    arrow->SetLineColor(kRed-2);
    arrow->SetLineWidth(1);
}
