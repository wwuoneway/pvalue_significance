// File: SimpleCount.h
// Using RooStats to find the discovery significance (p-value of the background-only hypothesis)
// for n_obs and known b.
#ifndef SIMPLECOUNT_H
#define SIMPLECOUNT_H

#include <TStopwatch.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <TMath.h>
#include <RooPlot.h>
#include <RooAbsPdf.h>
#include <RooWorkspace.h>
#include <RooDataSet.h>
#include <RooGlobalFunc.h>
#include <RooFitResult.h>
#include <RooRandom.h>
#include <RooPoisson.h>
#include <RooStats/ProfileLikelihoodCalculator.h>
#include <RooStats/LikelihoodInterval.h>
#include <RooStats/LikelihoodIntervalPlot.h>
#include <RooStats/BayesianCalculator.h>
#include <RooStats/MCMCCalculator.h>
#include <RooStats/MCMCInterval.h>
#include <RooStats/MCMCIntervalPlot.h>
#include <RooStats/ProposalHelper.h>
#include <RooStats/FeldmanCousins.h>
#include <RooStats/PointSetInterval.h>
#include <RooStats/HypoTestResult.h>
#include <RooStats/ToyMCSampler.h>
#include <RooStats/ProfileLikelihoodTestStat.h>

using namespace RooFit;
using namespace RooStats;
using namespace std;

namespace SimpleCount {

  double pValue(double n_obs, double b);
    
  double significanceFromPValue(double pValue);
  
  double pValue(double n_obs, vector<double> b_vec, vector<string> sysname_vec, vector<vector<double>> berr_vec);
}

#endif
