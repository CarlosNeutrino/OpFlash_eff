// Detector geometry (in cm)
const double Y_MIN = -200;
const double Y_MAX = +200;
const double X_MIN = -200;
const double X_MAX = +200;
const double Z_MIN = 0.;
const double Z_MAX = 500.;


// Constants from the OpFlash reconstruction algorithm
const double TOTAL_TIME = 3000;    // temporal size of the DAQ window in μs
const double BIN_SIZE = 0.010;     // temporal size of the bin (temporal size of the OpHits) in μs
const double VETO_TIME = 8;        // temporal size of the veto time of the OpFlash in μs
const int PE_MIN_PMT = 6;       // minimum value of photo-electrons for a OpHit to contribute to a start of an OpFlash
const int PE_MIN = 10;          // minimum value of photo-electrons for the sum of contributing OpHits to start an OpFlash

// Light components time constants and windows
const double FAST_LIGHT_TIME_CTE = 6./1000.;            // Time constant for fast light component (in μs)
const double SLOW_LIGHT_TIME_CTE = 1590./1000.;         // Time constant for fast light component (in μs)
//const double FAST_LIGHT_TIME = FAST_LIGHT_TIME_CTE*5;             // Time window for detecting 99% of the fast light component (in μs). I take into account the travel of the photons to make this time longer
const double FAST_LIGHT_TIME = 0.2;
const double SLOW_LIGHT_TIME = 4.77;              // Time window for detecting 95% of the slow light component (in μs)

// Numeric constants
const double PI = TMath::Pi();

// Window for matching OpFlash with MCTruth (in ns)
const double TIME_WINDOW = 0.05; // 50 ns = 0.05μs