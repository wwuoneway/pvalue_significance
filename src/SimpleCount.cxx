// File: SimpleCount.cxx
// useful references:
//  - https://twiki.cern.ch/twiki/bin/view/Main/LearningRoostats
//  - https://indico.cern.ch/event/322208/attachments/623138/857502/main.pdf

#include <iostream>
#include <string>
#include "SimpleCount.h"

double SimpleCount::pValue(double n_obs, double bkg) {
  
  cout << "calculate p-value using RooStats for simple counting experiment" << endl;

  // new workspace
  RooWorkspace* wspace = new RooWorkspace("wspace");

  // - create variable n with given range and put it in workspace
  RooRealVar* n = (RooRealVar*) wspace->factory("n[81, 0., 1000.]");
  RooRealVar* s = (RooRealVar*) wspace->factory("s[0.,300.]");
  RooRealVar* b = (RooRealVar*) wspace->factory("b[0.,300.]");

  wspace->factory("sum::splusb(s,b)");
  RooAbsPdf* model = (RooAbsPdf*) wspace->factory("Poisson::model(n,splusb)");
  
  //s->setVal(0);
  //s->setConstant();

  b->setVal(bkg);
  b->setConstant();
  
  wspace->defineSet("obs","n");
  wspace->defineSet("POI", "s");
  wspace->defineSet("nuisPar", "b");
  
  // create data set, set data value, and put it in workspace
  RooDataSet* data = new RooDataSet("data", "data", RooArgSet(*n)); // empty
  n->setVal(n_obs);
  data->add(*n);
  wspace->import(*data);
  wspace->Print();

  // - make ModelConfig object and put it in workspace
  ModelConfig* modelConfig = new ModelConfig("SimpleCount");
  modelConfig->SetWorkspace(*wspace);
  modelConfig->SetPdf(*wspace->pdf("model"));
  modelConfig->SetParametersOfInterest(*wspace->set("POI"));
  modelConfig->SetNuisanceParameters(*wspace->set("nuisPar"));
  wspace->import(*modelConfig);
  //wspace->writeToFile("SimpleCount.root");
  
  // background-only hypothesis (s = 0)
  // - here try profile likelihood
  ProfileLikelihoodCalculator plc(*data, *modelConfig);
  RooArgSet* nullParams = (RooArgSet*)(*wspace->set("POI")).snapshot();
  nullParams->setRealValue("s",0);
  plc.SetNullParameters(*nullParams);
  HypoTestResult* htr = plc.GetHypoTest();
  double pvalue = htr->NullPValue();

  return pvalue;
}

double SimpleCount::significanceFromPValue(double pValue) {
  //double significance = TMath::NormQuantile(1 - pValue);
  double significance = RooStats::PValueToSignificance(pValue);
  return significance;
}

double SimpleCount::pValue(double n_obs, vector<double> b_vec, vector<string> sysname_vec, vector<vector<double>> berr_vec) {

  cout << "calculate p-value using RooStats for simple counting experiment" << endl;

  // new workspace
  RooWorkspace* wspace = new RooWorkspace("wspace");

  // make model
  // - create variable n with given range and put it in workspace
  RooRealVar* n = (RooRealVar*) wspace->factory("n[0., 1000.]");
  RooRealVar* s = (RooRealVar*) wspace->factory("s[0., 0.,1000.]");
  //RooRealVar* theta = (RooRealVar*) wspace->factory("theta[-7, 7]");
  
  for (int k=0; k<int(sysname_vec.size()); k++) {
    RooRealVar* temp_theta= (RooRealVar*) wspace->factory(Form("theta%i[-7, 7]",k));
  }
  
  //RooRealVar* b1_mean = (RooRealVar*) wspace->factory(Form("b1_mean[%f]", b_vec[0]));
  //RooRealVar* b2_mean = (RooRealVar*) wspace->factory(Form("b2_mean[%f]", b_vec[1]));
  //RooRealVar* b3_mean = (RooRealVar*) wspace->factory(Form("b3_mean[%f]", b_vec[2]));

  //RooRealVar* b1_sigma = (RooRealVar*) wspace->factory(Form("b1_sigma[%f]", berr_vec[0][0]));
  //RooRealVar* b2_sigma = (RooRealVar*) wspace->factory(Form("b2_sigma[%f]", berr_vec[0][1]));
  //RooRealVar* b3_sigma = (RooRealVar*) wspace->factory(Form("b3_sigma[%f]", berr_vec[0][2]));
  
  for (int j=0; j<int(b_vec.size()); j++) {
    RooRealVar* temp_mean = (RooRealVar*) wspace->factory(Form("b%i_mean[%f]", j, b_vec[j]));
    
    for (int k=0; k<int(sysname_vec.size()); k++) {
      RooRealVar* temp_sigma =  (RooRealVar*) wspace->factory(Form("b%i%i_sigma[%f]", k, j, berr_vec[k][j]));
    }
  }

  // define set for convinience
  wspace->defineSet("obs","n");
  wspace->defineSet("POI", "s");

  // create data set, set data value, and put it in workspace
  RooDataSet* data = new RooDataSet("data", "data", RooArgSet(*n)); // empty
  n->setVal(n_obs);
  //data->add(*n);
  data->add(*wspace->set("obs"));
  wspace->import(*data);
  
  // we will consider a log normal distribution as prior pdf
  // n = s + e^(theta*log(1+sigma_b))*b0
  // theta: normal prior with mean 0 and standard deviation 1.
  //RooAbsPdf* prior_pdf = (RooAbsPdf*) wspace->factory("Gaussian::prior_pdf(theta, 0., 1.)");
  //RooFormulaVar* b1 = (RooFormulaVar*) wspace->factory("expr::b1('exp(@0*log(1+@1))*@2',{theta, b1_sigma, b1_mean})");
  //RooFormulaVar* b2 = (RooFormulaVar*) wspace->factory("expr::b2('exp(@0*log(1+@1))*@2',{theta, b2_sigma, b2_mean})");
  //RooFormulaVar* b3 = (RooFormulaVar*) wspace->factory("expr::b3('exp(@0*log(1+@1))*@2',{theta, b3_sigma, b3_mean})");
  

  for (int k=0; k<int(sysname_vec.size()); k++) {
    RooAbsPdf* temp_prior_pdf = (RooAbsPdf*) wspace->factory(Form("Gaussian::prior_pdf_%i(theta%i, 0., 1.)", k, k));
  }

  string bfunction = "";
  for (int j=0; j<int(b_vec.size()); j++) {
    for (int k=0; k<int(sysname_vec.size()); k++) {

      if (berr_vec[k][j] == 0) continue;

      RooFormulaVar* temp_b = (RooFormulaVar*) wspace->factory(Form("expr::b%i%i('exp(@0*log(1+@1))*@2',{theta%i, b%i%i_sigma, b%i_mean})",k,j,k,k,j,j));

      if ( (j == b_vec.size()-1) && (k == sysname_vec.size() - 1) ) {
        bfunction += Form("b%i%i",k,j);
      }
      else {
        bfunction += Form("b%i%i, ",k,j);
      }
    }
  }
  //std::cout << bfunction << std::endl;
  
  string priorfunction = "";
  string thetas = "";
  for (int k=0; k<int(sysname_vec.size()); k++) {
    if (k == sysname_vec.size() - 1) {
      priorfunction += Form("prior_pdf_%i",k);
      thetas += Form("theta%i",k);
    }
    else {
      priorfunction += Form("prior_pdf_%i, ",k);
      thetas += Form("theta%i,",k);
    }
  }
  //std::cout << priorfunction << std::endl;
  std::cout << thetas<< std::endl;

  //wspace->factory("sum::splusb(s, b1, b2, b3)");
  //RooAbsPdf* model_poisson = (RooAbsPdf*) wspace->factory("Poisson::model_poisson(n, splusb)");
  //RooAbsPdf* model = (RooAbsPdf*) wspace->factory("PROD::model(model_poisson, prior_pdf)");
  
  wspace->factory(Form("sum::splusb(s, %s)", bfunction.c_str()));
  RooAbsPdf* model_poisson = (RooAbsPdf*) wspace->factory("Poisson::model_poisson(n, splusb)");
  RooAbsPdf* model = (RooAbsPdf*) wspace->factory(Form("PROD::model(model_poisson, %s)", priorfunction.c_str()));

  wspace->Print();

  //s->setVal(0);
  //s->setConstant();
  
  //wspace->defineSet("nuisPar", "theta");
  wspace->defineSet("nuisPar", Form("%s", thetas.c_str()));
  

  // - make ModelConfig object and put it in workspace
  ModelConfig* modelConfig = new ModelConfig("modelConfig");
  modelConfig->SetWorkspace(*wspace);
  modelConfig->SetPdf(*wspace->pdf("model"));
  modelConfig->SetParametersOfInterest(*wspace->set("POI"));
  modelConfig->SetNuisanceParameters(*wspace->set("nuisPar"));
  wspace->import(*modelConfig);
  //wspace->writeToFile("modelConfig.root");
  modelConfig->Print();
  
  // background-only hypothesis (s = 0)
  // - here try profile likelihood
  ProfileLikelihoodCalculator plc(*data, *modelConfig);
  RooArgSet* nullParams = (RooArgSet*)(*wspace->set("POI")).snapshot();
  nullParams->setRealValue("s",0);
  plc.SetNullParameters(*nullParams);
  HypoTestResult* htr = plc.GetHypoTest();
  double pvalue = htr->NullPValue();
  return pvalue;
}
