/*

------------------------------ PDS analyzer ------------------------------           
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

/*
---------- Configuration parameters ----------
- SaveMCTruth, SaveMCParticles, ...: booleans to save different data products at different stages of the simulation/reconstruction chain.
- Verbosity: shows some printous while running the analyzer.
- MakePerTrackTree: option to make a tree with the number of SimPhotons per MCParticle (OpAnaPerTrackTree).
- MakePDSGeoTree: option to dump the PDS mapping in a TTree (PDSMapTree)
- UseSimPhotonsLite: use SimPhotonsLite or SimPhotons (default is true, i.e. use SimPhotonsLite)
- KeepPDGCode: specify what PDG codes will be stored in the TTree from the MC particles list. If no PDGs are specified, it saves all the MC particles (default is [], so it saves everything).
- MCTruthOrigin: specify the MCTruth origins that will be saved. Default is [1], i.e. neutrino generated events.
- MCTruthPDG: specify the PDG codes of the candidate vertex particles for the MCTruth. Defaults is [12, -12, 14, -14], i.e. BNB neutrinos.
- PDTypes: vector specifying the PD types for which it will save the SimPhotons (default is ["pmt_coated", "pmt_uncoated"])
- G4BufferBoxX, G4BufferBoxY, G4BufferBoxZ: only store MC particle trajectories inside the boundaries defined by the G4BufferBox variables.
- G4BeamWindow: only store MC particles and trajectories in the time window defined by this parameter. Default is [-10000, 12000] #ns.
*/

////////////////////// Map for PDS ////////////////////////
// Variables to store the PDS mapping
std::vector<int>* OpDetID = nullptr;        // Optical detector ID
std::vector<double>* OpDetX = nullptr;      // Optical detector X position (in cm)
std::vector<double>* OpDetY = nullptr;      // Optical detector Y position (in cm)
std::vector<double>* OpDetZ = nullptr;      // Optical detector Z position (in cm)
std::vector<int>* OpDetType = nullptr;      // Optical detector type (0: CoatedPMT, 1: UncoatedPMT, 2: VUV XARAPUCA and 3: VIS XARAPUCA)

/////////////////////// Event Variables ///////////////////////

// General event information
unsigned int eventID;     // Event ID: number assigned to a event
unsigned int runID;       // Run ID: number assigned to each run
unsigned int subrunID;    // SubRun ID: number assigned to each subrun

////////////// Generator stage //////////////
/*
They store information regarging all the primary neutrino interactions in a given readout window. 
Loop over the std::vector to get the energy, interaction vertex and interaction time corresponding to the 'i' neutrino interaction.
*/

std::vector<double>* nuvX = nullptr;                   // true neutrino interaction vertex (x axis)
std::vector<double>* nuvY = nullptr;                   // true neutrino interaction vertex (y axis)
std::vector<double>* nuvZ = nullptr;                   // true neutrino interaction vertex (z axis)
std::vector<double>* nuvT = nullptr;                   // true neutrino interaction time (in ns)
std::vector<double>* nuvE = nullptr;                   // true neutrino energy in GeV

////////////// G4 stage //////////////
/*
Example: Consider a 1207 MeV beam nu_μ. It undergoes a charged current interaction, leading to a μ-
 (E=574 MeV) and a proton (E=1543 MeV). The first entry in the previous vectors will have the following values: 
  E[0]=0.574, process[0]=primary, trackID[0]=1, motherID[0]=0, PDGCode[0]=13, E[1]=1.543, PDGCode[1]=2212... 
  If e.g. the muon decays to a Michel electron, there will be an entry with PDGCode=11, motherID=1 and process=Decay.
*/

std::vector<double>* E = nullptr;                          // Primary energy of each MC particle (in GeV)
std::vector<std::string>* process = nullptr;               // Primary process of MC each particle
std::vector<int>* trackID = nullptr;                       // MC particle ID
std::vector<int>* motherID = nullptr;                      // ID of the mother MC particle
std::vector<int>* PDGcode = nullptr;                       // PDG code
bool InTimeCosmics;                      // Returns true if there is a cosmic interaction during the BNB spill ??
bool InTimeCosmicsTime;                  // Time (in ns) of the InTime-cosmic interaction ??

// Start and end momenta of each MC particle at the g4 stage
std::vector<double>* StartPx = nullptr;   
std::vector<double>* StartPy = nullptr;
std::vector<double>* StartPz = nullptr;
std::vector<double>* EndPx = nullptr;
std::vector<double>* EndPy = nullptr;
std::vector<double>* EndPz = nullptr;
std::vector<double>* MCP_time = nullptr;

// Deposited energy
/*
Following the previous example, to read the energy deposition values and their locations
 induced by the primary proton you can take energydep[1], energydepX[1]...
*/

std::vector<std::vector<double>>* energydep = nullptr;            // Energy deposition (in MeV) at each G4 tracking step. It's saved for each MC particle
std::vector<std::vector<double>>* energydepX = nullptr;           // Location (in cm) of each energy deposition.
std::vector<std::vector<double>>* energydepY = nullptr;
std::vector<std::vector<double>>* energydepZ = nullptr;        
std::vector<double>* dEpromX = nullptr;                           // Average X, Y, Z (in cm) location of the energy depositions. It's saved for the two TPCs (vector size is 2)
std::vector<double>* dEpromY = nullptr;
std::vector<double>* dEpromZ = nullptr;
std::vector<double>* dEspreadX = nullptr;                         // X, Y, Z standard deviation of the energy depositions. It's saved for the two TPCs (vector size is 2)
std::vector<double>* dEspreadY = nullptr;
std::vector<double>* dEspreadZ = nullptr;
std::vector<std::vector<double>>* dElowedges = nullptr;           // X, Y, Z coordinates of the lowest energy deposition. It's saved for the two TPCs (vector size is 2)
std::vector<std::vector<double>>* dEmaxedges = nullptr;           // X, Y, Z coordinates of the maximum energy deposition. It's saved for the two TPCs (vector size is 2)

// Scintillation photons
/*
SBND has 312 PDs, hence the size of the SimPhotonsperOpChVUV(VIS) is 312.
You can obtain the number of VUV photons reaching the coated PMT with ID 144 by taking SimPhotonsperOpChVUV[144]. 
The size of the SimPhotonsLiteVUV(VIS) is also 312. 
Each vector in the 'vector of vecrtors' contains the times (in ns) in which each photon gets to the given PD. 
Imagine 567 VUV photons reach the coated PMT with ID 144. The size of the vector SimPhotonsLiteVUV[144] will be 567.
*/

std::vector<double>* SimPhotonsperOpChVUV = nullptr;                   // Number of true photons at each PD (VUV wavelength)
std::vector<double>* SimPhotonsperOpChVIS = nullptr;                   // Number of true photons at each PD (visible wavelength)
std::vector<std::vector<double>>* SimPhotonsLiteVUV = nullptr;         // Photon arrival times at G4 stage (VUV). In ns.
std::vector<std::vector<double>>* SimPhotonsLiteVIS = nullptr;         // Photon arrival times at G4 stage (VIS). In ns.
/*
double NPhotons;                                            // Integrated number of photons in the events per PD type.
*/

////////////// detsim stage //////////////
/*
The PMT/XARAPUCA output signals (including electronic response) are stored in the previous vectors. 
We only save the regions of the waveforms going above a certain thereshold (region of interest or ROIs). 
The ADC values of each identifeid ROI correspond to an entry in the SignalsDigi "vector of vectors". 
Note that we may have more than one ROI per PD, so the size of the SignalsDigi branch will be in general different than the number of PDs (312). 
To get the start time and the channel corresponding to the 'i' ROI get the StampTime and OpChDigi with index 'i'.
*/

std::vector<std::vector<double>>* SignalsDigi = nullptr;       // Digitized signals (ADC values)
std::vector<double>* StampTime = nullptr;                      // Start time of each digitized waveform (in $\mu$s)
std::vector<int>* OpChDigi = nullptr;                          // Associated PD ID

////////////// reco stage //////////////
/*
Same scheme followed, but storing the deconvolved signals instead of the raw waveforms.
*/

std::vector<std::vector<double>>* SignalsDeco = nullptr;       // Deconvolved signals
std::vector<double>* StampTimeDeco = nullptr;                  // Start time of each digitized waveform (in μs)
std::vector<int>* OpChDeco = nullptr;                          // 	Associated PD ID

// Pulse finder <---------- This is the part that I will use
int nophits;                                // Total number of reconstructed OpHits
std::vector<int>* ophit_opch = nullptr;                // Optical channel corresponding to the reconstructed OpHit
std::vector<double>* ophit_peakT = nullptr;            // Waveform bin in which the OpHit gets the maximum value (in μs)
std::vector<double>* ophit_startT = nullptr;           // Start of the OpHit (in μs)
std::vector<double>* ophit_riseT = nullptr;            // OpHit rise time, relative to the StartTime (in μs)
std::vector<double>* ophit_width = nullptr;            // With of the OpHit (in μs)
std::vector<double>* ophit_amplitude = nullptr;        // Amplitude of the OpHit (in ADC units)
std::vector<double>* ophit_area = nullptr;             // Area of the OpHit (in μs x ADC units)
std::vector<double>* ophit_pe = nullptr;               // Reconstructed number of PE

// Clustering among different photo-detectos (a.k.a. OpFlash)
int nopflash;                                       // Total number of reconstructed OpFlash objects
std::vector<double>* flash_time = nullptr;                     // t0 of the reconstructed OpFlashes
std::vector<double>* flash_total_pe = nullptr;                 // Integrated (all optical channels) number of photoelectrons in each OpFlash
std::vector<std::vector<double>>* flash_pe_v = nullptr;        // Vector containing the reconstructed number of photoelectron in each optical channel for each OpFlash

std::vector<double>* flash_x = nullptr;      // X, Y, Z position of the reconstructed OpFlash --> SEE HOW THIS IS CALCULATED
std::vector<double>* flash_y = nullptr;
std::vector<double>* flash_z = nullptr;
std::vector<double>* flash_xerr = nullptr;      // X, Y, Z position of the reconstructed OpFlash --> SEE HOW THIS IS CALCULATED
std::vector<double>* flash_yerr = nullptr;
std::vector<double>* flash_zerr = nullptr;

// Save the attributes of the OpHits associated to each OpFlash
std::vector<std::vector<double>>* flash_ophit_time = nullptr;
std::vector<std::vector<double>>* flash_ophit_risetime = nullptr;
std::vector<std::vector<double>>* flash_ophit_starttime = nullptr;
std::vector<std::vector<double>>* flash_ophit_amp = nullptr;
std::vector<std::vector<double>>* flash_ophit_area = nullptr;
std::vector<std::vector<double>>* flash_ophit_width = nullptr;
std::vector<std::vector<double>>* flash_ophit_pe = nullptr;
std::vector<std::vector<double>>* flash_ophit_ch = nullptr;


//Function to set all the branches of the tree containing event information
void set_branch_OpAna(TTree* tree) {

    //////////// General event information ////////////
    tree->SetBranchAddress("eventID", &eventID);
    tree->SetBranchAddress("runID", &runID);
    tree->SetBranchAddress("subrunID", &subrunID);

    //////////// Generator stage ////////////
    tree->SetBranchAddress("nuvX", &nuvX);
    tree->SetBranchAddress("nuvY", &nuvY);
    tree->SetBranchAddress("nuvZ", &nuvZ);
    tree->SetBranchAddress("nuvT", &nuvT);
    tree->SetBranchAddress("nuvE", &nuvE);

    //////////// G4 stage ////////////
    tree->SetBranchAddress("E", &E);
    tree->SetBranchAddress("process", &process);
    tree->SetBranchAddress("trackID", &trackID);
    tree->SetBranchAddress("motherID", &motherID);
    tree->SetBranchAddress("PDGcode", &PDGcode);
    tree->SetBranchAddress("InTimeCosmics", &InTimeCosmics);

    tree->SetBranchAddress("InTimeCosmicsTime", &InTimeCosmicsTime);
    tree->SetBranchAddress("StartPx", &StartPx);
    tree->SetBranchAddress("StartPy", &StartPy);
    tree->SetBranchAddress("StartPz", &StartPz);
    tree->SetBranchAddress("EndPx", &EndPx);
    tree->SetBranchAddress("EndPy", &EndPy);
    tree->SetBranchAddress("EndPz", &EndPz);
    tree->SetBranchAddress("MCP_time", &MCP_time);

    tree->SetBranchAddress("energydep", &energydep);
    tree->SetBranchAddress("energydepX", &energydepX);
    tree->SetBranchAddress("energydepY", &energydepY);
    tree->SetBranchAddress("energydepZ", &energydepZ);
    //tree->SetBranchAddress("dEpromX", &dEpromX);
    //tree->SetBranchAddress("dEpromY", &dEpromY);
    //tree->SetBranchAddress("dEpromZ", &dEpromZ);
    //tree->SetBranchAddress("dEspreadX", &dEspreadX);
    //tree->SetBranchAddress("dEspreadY", &dEspreadY);
    //tree->SetBranchAddress("dEspreadZ", &dEspreadZ);
    tree->SetBranchAddress("dElowedges", &dElowedges);
    tree->SetBranchAddress("dEmaxedges", &dEmaxedges);

    //tree->SetBranchAddress("SimPhotonsperOpChVUV", &SimPhotonsperOpChVUV);
    //tree->SetBranchAddress("SimPhotonsperOpChVIS", &SimPhotonsperOpChVIS);
    //tree->SetBranchAddress("SimPhotonsLiteVUV", &SimPhotonsLiteVUV);
    //tree->SetBranchAddress("SimPhotonsLiteVIS", &SimPhotonsLiteVIS);

    //////////// detsim stage ////////////
    //tree->SetBranchAddress("SignalsDigi", &SignalsDigi);
    //tree->SetBranchAddress("StampTime", &StampTime);
    //tree->SetBranchAddress("OpChDigi", &OpChDigi);

    //////////// reco stage ////////////
    //tree->SetBranchAddress("SignalsDeco", &SignalsDeco);
    //tree->SetBranchAddress("StampTimeDeco", &StampTimeDeco);
    //tree->SetBranchAddress("OpChDeco", &OpChDeco);

    //////////// Pulse finder ////////////
    tree->SetBranchAddress("nophits", &nophits);
    tree->SetBranchAddress("ophit_opch", &ophit_opch);
    tree->SetBranchAddress("ophit_peakT", &ophit_peakT);
    tree->SetBranchAddress("ophit_startT", &ophit_startT);
    tree->SetBranchAddress("ophit_riseT", &ophit_riseT);  // corregido
    tree->SetBranchAddress("ophit_width", &ophit_width);
    tree->SetBranchAddress("ophit_amplitude", &ophit_amplitude);
    tree->SetBranchAddress("ophit_area", &ophit_area);
    tree->SetBranchAddress("ophit_pe", &ophit_pe);

    //////////// OpFlash clustering ////////////
    tree->SetBranchAddress("nopflash", &nopflash);
    tree->SetBranchAddress("flash_time", &flash_time);
    tree->SetBranchAddress("flash_total_pe", &flash_total_pe);
    tree->SetBranchAddress("flash_pe_v", &flash_pe_v);
    tree->SetBranchAddress("flash_x", &flash_x);
    tree->SetBranchAddress("flash_y", &flash_y);
    tree->SetBranchAddress("flash_z", &flash_z);

    tree->SetBranchAddress("flash_ophit_time", &flash_ophit_time);
    tree->SetBranchAddress("flash_ophit_risetime", &flash_ophit_risetime);
    tree->SetBranchAddress("flash_ophit_starttime", &flash_ophit_starttime);
    tree->SetBranchAddress("flash_ophit_amp", &flash_ophit_amp);
    tree->SetBranchAddress("flash_ophit_area", &flash_ophit_area);
    tree->SetBranchAddress("flash_ophit_width", &flash_ophit_width);
    tree->SetBranchAddress("flash_ophit_pe", &flash_ophit_pe);
    tree->SetBranchAddress("flash_ophit_ch", &flash_ophit_ch);

}

void set_branch_PDS_map(TTree* tree) {
    tree->SetBranchAddress("OpDetID", &OpDetID);
    tree->SetBranchAddress("OpDetX", &OpDetX);
    tree->SetBranchAddress("OpDetY", &OpDetY);
    tree->SetBranchAddress("OpDetZ", &OpDetZ);
    tree->SetBranchAddress("OpDetType", &OpDetType);
}
