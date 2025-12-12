
/*
Exponential in 1D function to represent the expected temporal distribution of OpHits
Parameters par:
- par[0] = Amplitude
- par[1] = Start time

Values of x:
- x[0] = x axis (time position)
*/
double Exp1D(double *x, double *par)
{
    // x[0] = x axis (z position)
    // x[1] = y axis (y position)

    double X = x[0];

    double A   = par[0];    // Contribution of BOTH, slow and fast, light components
    double As, Af;          // Individual contributions of slow and fast light components
    // From the literature, the fast component contributes ~23% of the total light yield
    // Therefore, it can be calculated that:
    As = A*(1./(1 + 0.23*(SLOW_LIGHT_TIME_CTE/FAST_LIGHT_TIME_CTE)));
    Af = A - As;
    double t0  = par[1];

    double exp_value =
        Af * exp( -((X-t0)/(FAST_LIGHT_TIME_CTE)) ) + As * exp( -((X-t0)/(SLOW_LIGHT_TIME_CTE)) );

    return exp_value;
}



/*
Gassian in 2D function to fit the spatial distribution of OpHits
Parameters par:
- par[0] = Amplitude
- par[1] = Mean x
- par[2] = Sigma x
- par[3] = Mean y
- par[4] = Sigma y

Values of x:
- x[0] = x axis (z position)
- x[1] = y axis (y position)
*/
double gauss2D(double *x, double *par)
{
    // x[0] = x axis (z position)
    // x[1] = y axis (y position)

    double X = x[0];
    double Y = x[1];

    double A   = par[0];
    double mx  = par[1];
    double sx  = par[2];
    double my  = par[3];
    double sy  = par[4];

    double gauss =
        A * exp( -0.5 * ( ((X - mx)*(X - mx))/(sx*sx)
                        + ((Y - my)*(Y - my))/(sy*sy) ) );

    return gauss;
}

/*
From a TGraph2D, get the maximum Z value and the corresponding X and Y values
Parameters:
- g: pointer to the TGraph2D
- xmax: reference to store the X value corresponding to the maximum Z
- ymax: reference to store the Y value corresponding to the maximum Z
Returns:
- maxZ: maximum Z value found in the TGraph2D
*/
double GetMaxZ(TGraph2D* g, double& xmax, double& ymax)
{
    double maxZ = -1e300;   // very small initial value

    double x, y, z;
    for (int i = 0; i < g->GetN(); i++) {
        g->GetPoint(i, x, y, z);
        if (z > maxZ){
            maxZ = z;
            xmax = x;   // values in reference in the fucntion
            ymax = y;
        }
    }

    return maxZ;
}

/*
From a TGraph2D and a TF2 fit function, calculate the residuals (TGraoh2D data - TF2 fit)
Parameters:
- g: pointer to the TGraph2D
- fit: pointer to the TF2 fit function
Returns:
- residuals: pointer to the TGraph2D containing the residuals
*/
TGraph2D* SubtractFit(TGraph2D* g, TF2* fit)
{
    TGraph2D* residuals = new TGraph2D();

    double x, y, z;

    for (int i = 0; i < g->GetN(); i++) {

        g->GetPoint(i, x, y, z);

        double z_fit = fit->Eval(x, y);
        double z_res = z - z_fit;

        residuals->SetPoint(i, x, y, z_res);
    }

    return residuals;
}