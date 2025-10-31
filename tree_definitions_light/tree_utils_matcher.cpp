/*

------------------------------ FlassRecoEff ------------------------------           
                      Created by Francisco Nicolás
*/

#include "Includes.h"       // ROOT related includes + using namespace std
using namespace std;
/*
Declaration of the variables inside the tree for the analyzer. The .root file will include:
- TDirectoryFile *ana --> the trees are stored here
    - TTree *PDSMapTree --> Creates a tree with the optical channel IDs, position (X, Y and Z in cm) and the photon detector type 
                            (0: CoatedPMT, 1: UncoatedPMT, 2: VUV XARAPUCA and 3: VIS XARAPUCA).
    - TTree *OpAnaTree --> TTree with the analysis variables (described below). Each entry in the TTree corresponds to an event.
    - TTree *OpAnaPerTrackTree --> TTree with the number of SimPhotons produced by each MC particle. 
                                    Each entry in the TTree corresponds to one MC particle in one event. 
                                    Requires SimPhotons (not SimPhotonsLite).
*/

////////////////////// Map for PDS ////////////////////////
// Variables to store the PDS mapping
std::vector<std::vector<int>> fCoatedUncoatedMap;       // Map with the pairs of [coated, uncoated] PMT IDs for the PMTs. 
// As there are more coated than uncoated PMTs, some uncoated PMTs will appear x4 times (next to their closest coated PMTs)

/////////////////////// Event Variables ///////////////////////

// General event information
unsigned int eventID_eff;     // Event ID: number assigned to a event
unsigned int runID_eff;       // Run ID: number assigned to each run
unsigned int subrunID_eff;    // SubRun ID: number assigned to each subrun

string InputFileName;   // Input file name

////////////// Generator stage //////////////
int Origin;              // Origin of the interaction (1=beam, 2=cosmic, 4=single interaction)
int NPrimaryParticles;   // Number of primary particles in the interaction
double E_eff;                // Energy of the primary particle (neutrino for neutrino interactions)
double DepositedEnergy;  // Total deposited energy inside the TPCs
double dEPromX, dEPromY, dEPromZ;  // Mean position of the energy deposition in each coordinate inside the detector (weighted by deposited energy)
double dETPCBalance;       // energy in TPC0 - energy in TPC1 / total energy
double DepositedEnergyOut; // Total deposited energy outside the TPCs
double dEPromXOut, dEPromYOut, dEPromZOut; // Mean position of the energy deposition in each coordinate outside the detector (weighted by deposited energy)
int IntPDG;                      // PDG code of the interacting particle (neutrino for neutrino interactions)
double IntX, IntY, IntZ;       // coordinates of the interaction vertex
double InteractionTime;          // Time of the interaction. If it is a cosmic, when time it is inside the detector
double VisEnT0;                  // First time when the particle is inside the detector (time of the first energy deposition)
double VisEnMaxT;                // Final time when the particle is inside the detector (time of the last energy deposition)
double VisEnMaxTContained;       // (?) Final time light is produced in the interaction 
bool DirtInteraction;            // neutrino Interaction happens outside the detector

bool CoincidentInteraction;      // boolean for the case in which there are 2 interactions coincideny: their veto would collide --> they do NOT create different OpFlashes
bool MultipleTPCInteraction;     // boolean for the case in which 1 interaction deposits energy in both TPCs
bool SaturatedEvent;             // boolean for the case in which there is saturation in at least one PMT in the event

////////////// Reco OpFlash information //////////////
bool RecoFlash;                  // OpFlash has a corresponding reconstructed MCT interaction
int NFlash;                      // Number of OpFlashes in the event
// Information about the reconstructed flash:
double FlashTotalPE;             // photoelectrons
double FlashTotalPECo;           // photoelectrons in coated PMTs
double FlashTotalPECoVUV;        // photoelectrons in coated VUV PMTs
double FlashTotalPEUnco;         // photoelectrons in uncoated PMTs
double FlashCompleteness;        // completeness of the flash (reconstructed PE / MC PE)
double FlashX, FlashY, FlashZ; // (?) Position of the flash
std::vector<double>* FlashPE_v = nullptr;               // photoelectrons per channel
std::vector<double>* FlashPEresolution_v = nullptr;     // resolution per channel
std::vector<bool>* FlashSatChannels_v = nullptr;        // saturated channels
std::vector<unsigned int>* FlashWaveSizeChannels_v = nullptr;   // waveform size per channel
std::vector<unsigned int>* FlashWaveMinADC_v = nullptr;         // min ADC value per channel
std::vector<unsigned int>* FlashWaveMaxADC_v = nullptr;         // max ADC value per channel
unsigned int MaxWaveformSize; 
double FlashTime;    // Time of the flash

////////////// MonteCarlo OpFlash information //////////////
double MCTotalPE;        // total photoelectrons in 1 flash from MC
double MCTotalPECo;      // total photoelectrons in coated PMTs from MC
double MCTotalPECoVUV;   // total photoelectrons in coated VUV PMTs from MC
double MCTotalPEUnco;    // total photoelectrons in uncoated PMTs from MC
std::vector<double>* MCPE_v = nullptr;      // photoelectrons per channel from MC
std::vector<double>* MCPeakPE_v = nullptr;  // peak photoelectrons per channel from MC 
std::vector<double>* MCPeakPEN_v = nullptr; // peak photoelectrons per channel from MC normalized to number of photons
double MCSimPhotonMinT;          // min time of the sim photons contributing to the flash
double MCSimPhotonMinD;          // min distance of the sim photons contributing to the flash

////////////// Truth Matching OpFlash<->MC Interaction //////////////
bool MatchedSlice;           // slice id matched with the flash
double SliceCompleteness;    // completeness of the slice
double SlicePurity;          // purity of the slice
bool GoodFlashMatch;         // flash matched to a good slice (compared to the MC matching)
double FlashMatchingScore;   // score of the flash matching
double FlashMatchingDepEn;   // deposited energy of the matched slice --> visible energy
std::vector<double>* FlashMatchingScore_v = nullptr;    // Score given to the matching


//Function to set all the branches of the tree containing event information
void set_branch_Light(TTree* tree) {

    //////////// General event information ////////////
    tree->SetBranchAddress("eventID", &eventID_eff);
    tree->SetBranchAddress("runID", &runID_eff);
    tree->SetBranchAddress("subrunID", &subrunID_eff);

    //////////// Generator stage ////////////
    tree->SetBranchAddress("Origin", &Origin);
    tree->SetBranchAddress("NPrimaryParticles", &NPrimaryParticles);
    tree->SetBranchAddress("E", &E);
    tree->SetBranchAddress("DepositedEnergy", &DepositedEnergy);
    tree->SetBranchAddress("dEPromX", &dEPromX);
    tree->SetBranchAddress("dEPromY", &dEPromY);
    tree->SetBranchAddress("dEPromZ", &dEPromZ);
    tree->SetBranchAddress("dETPCBalance", &dETPCBalance);
    tree->SetBranchAddress("DepositedEnergyOut", &DepositedEnergyOut);
    tree->SetBranchAddress("dEPromXOut", &dEPromXOut);
    tree->SetBranchAddress("dEPromYOut", &dEPromYOut);
    tree->SetBranchAddress("dEPromZOut", &dEPromZOut);
    tree->SetBranchAddress("IntPDG", &IntPDG);
    tree->SetBranchAddress("IntX", &IntX);
    tree->SetBranchAddress("IntY", &IntY);
    tree->SetBranchAddress("IntZ", &IntZ);
    tree->SetBranchAddress("InteractionTime", &InteractionTime);
    tree->SetBranchAddress("VisEnT0", &VisEnT0);
    tree->SetBranchAddress("VisEnMaxT", &VisEnMaxT);
    tree->SetBranchAddress("VisEnMaxTContained", &VisEnMaxTContained);
    tree->SetBranchAddress("DirtInteraction", &DirtInteraction);
    tree->SetBranchAddress("CoincidentInteraction", &CoincidentInteraction);
    tree->SetBranchAddress("MultipleTPCInteraction", &MultipleTPCInteraction);
    tree->SetBranchAddress("SaturatedEvent", &SaturatedEvent);

    //////////// Reco OpFlash information ////////////
    tree->SetBranchAddress("RecoFlash", &RecoFlash);
    tree->SetBranchAddress("NFlash", &NFlash);
    tree->SetBranchAddress("FlashTotalPE", &FlashTotalPE);
    tree->SetBranchAddress("FlashTotalPECo", &FlashTotalPECo);
    tree->SetBranchAddress("FlashTotalPECoVUV", &FlashTotalPECoVUV);
    tree->SetBranchAddress("FlashTotalPEUnco", &FlashTotalPEUnco);
    tree->SetBranchAddress("FlashCompleteness", &FlashCompleteness);
    tree->SetBranchAddress("FlashX", &FlashX);
    tree->SetBranchAddress("FlashY", &FlashY);
    tree->SetBranchAddress("FlashZ", &FlashZ);
    tree->SetBranchAddress("FlashPE_v", &FlashPE_v);
    tree->SetBranchAddress("FlashPEresolution_v", &FlashPEresolution_v);
    tree->SetBranchAddress("FlashSatChannels_v", &FlashSatChannels_v);
    tree->SetBranchAddress("FlashWaveSizeChannels_v", &FlashWaveSizeChannels_v);
    tree->SetBranchAddress("FlashWaveMinADC_v", &FlashWaveMinADC_v);
    tree->SetBranchAddress("FlashWaveMaxADC_v", &FlashWaveMaxADC_v);
    tree->SetBranchAddress("MaxWaveformSize", &MaxWaveformSize);
    tree->SetBranchAddress("FlashTime", &FlashTime);

    //////////// MonteCarlo OpFlash information ////////////
    tree->SetBranchAddress("MCTotalPE", &MCTotalPE);
    tree->SetBranchAddress("MCTotalPECo", &MCTotalPECo);
    tree->SetBranchAddress("MCTotalPECoVUV", &MCTotalPECoVUV);
    tree->SetBranchAddress("MCTotalPEUnco", &MCTotalPEUnco);
    tree->SetBranchAddress("MCPE_v", &MCPE_v);
    tree->SetBranchAddress("MCPeakPE_v", &MCPeakPE_v);
    tree->SetBranchAddress("MCPeakPEN_v", &MCPeakPEN_v);
    tree->SetBranchAddress("MCSimPhotonMinT", &MCSimPhotonMinT);
    tree->SetBranchAddress("MCSimPhotonMinD", &MCSimPhotonMinD);

    //////////// Truth Matching OpFlash<->MC Interaction ////////////
    tree->SetBranchAddress("MatchedSlice", &MatchedSlice);
    tree->SetBranchAddress("SliceCompleteness", &SliceCompleteness);
    tree->SetBranchAddress("SlicePurity", &SlicePurity);
    tree->SetBranchAddress("GoodFlashMatch", &GoodFlashMatch);
    tree->SetBranchAddress("FlashMatchingScore", &FlashMatchingScore);
    tree->SetBranchAddress("FlashMatchingDepEn", &FlashMatchingDepEn);
    tree->SetBranchAddress("FlashMatchingScore_v", &FlashMatchingScore_v);

}


void set_branch_header(TTree* tree) {
    tree->SetBranchAddress("runID", &runID_eff);
    tree->SetBranchAddress("subrunID", &subrunID_eff);
    tree->SetBranchAddress("InputFileName", &InputFileName);
}
