// File: runPSigCalc.cxx

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <string>
#include "TMath.h"
#include "TString.h"
#include "TRandom3.h"

#include "SimpleCount.h"

using namespace std;

int main(int argc, char **argv) {
  
  bool useDebug = true;

  // -------- Read input file --------
  TString inputFileName;
  if (argc == 1) {
    cout << "Enter the input file name: ";
    cin >> inputFileName;
    cin.ignore(); // clear buffer
  }
  else {
    inputFileName = argv[1];
  }
  if (useDebug)cout << "Input file name: " << inputFileName << endl;
  
  ifstream f;
  f.open(inputFileName);
  if (f.fail()) {
    cout << "Cound not open input file" << endl;
    exit(1);
  }
  
  int imax, jmax, kmax; // # of signal channels, # of background channels, # of nuisance parameters
  double n_obs, s;
  vector<string> events_options;
  vector<double> b_vec;
  vector<string> sysname_vec;
  vector<double> serr_vec;
  vector<vector<double>> berr_vec;
  
  istringstream instream; // declear an input string stream
  string line;
  int lineNum = 0;
  while (getline(f, line)) {
    bool useLine = !(line.substr(0,1) == "#" || line.substr(0,1) == "-");
    if (useLine) {
      instream.clear();
      instream.str(line);
      lineNum++;
      if (lineNum == 1) {
        instream >> imax >> jmax >> kmax;
      }
      if (lineNum == 2) {
        instream >> n_obs;
      }
      if (lineNum == 3) {
        for (int j=0; j<(imax + jmax); j++) {
          string temp_catogory;
          instream >> temp_catogory;
          events_options.push_back(temp_catogory);
        }
      }
      if (lineNum == 4) {
        instream >> s;
        for (int j=0; j<jmax; j++) {
          double temp_b;
          instream >> temp_b;
          b_vec.push_back(temp_b);
        }
      }
      if (lineNum >= 5) {
        string temp_sysname ;
        instream >> temp_sysname;
        sysname_vec.push_back(temp_sysname);
        double temp_serr;
        instream >> temp_serr;
        serr_vec.push_back(temp_serr);
        vector<double> temp_berr_vec;
        for (int j=0; j<jmax; j++) {
          double temp_berr;
          instream >> temp_berr;
          temp_berr_vec.push_back(temp_berr);
        }
        berr_vec.push_back(temp_berr_vec);
      }
    }
  } // while(getline(f, line))
  if (useDebug) {
    cout << "imax = " << imax << endl;
    cout << "jmax = " << jmax << endl;
    cout << "kmax = " << kmax << endl;
    cout << "n_obs = " << n_obs << endl;
    cout << "events_options: " << endl; 
    for (int j=0; j<(imax+jmax); j++) {
      cout << "  " << events_options[j];
    }
    cout << endl << "  " << s;
    for (int j=0; j<jmax; j++) {
      cout << "  " << b_vec[j];
    }
    cout << endl;
    
    for (int k=0; k<kmax; k++) {
      cout << sysname_vec[k] << " " << serr_vec[k];
      for (int j=0; j<jmax; j++) {
        cout << "  " << berr_vec[k][j];
      }
      cout << endl;
    }
  } // useDebug

  
  // -------- example 1 --------
  //double test_pval =  SimpleCount::pValue(81, 21.767);
  //double test_significance =  SimpleCount::significanceFromPValue(test_pval);
  //cout << "test_pval: " << test_pval << endl;
  //cout << "test_significance: " << test_significance << endl;

  // -------- example 2 --------
  double pval =  SimpleCount::pValue(n_obs, b_vec, sysname_vec, berr_vec);
  double significance =  SimpleCount::significanceFromPValue(pval);
  cout << "pval: " << pval << endl;
  cout << "significance: " << significance << endl;


  // -------- Done --------
  cout << endl << "Done." << endl;
  return 0;
}
