// @HEADER
//
// ***********************************************************************
//
//   Zoltan2: A package of combinatorial algorithms for scientific computing
//                  Copyright 2012 Sandia Corporation
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Karen Devine      (kddevin@sandia.gov)
//                    Erik Boman        (egboman@sandia.gov)
//                    Siva Rajamanickam (srajama@sandia.gov)
//
// ***********************************************************************
//
// @HEADER

/* \file test_driver.cpp
 * \brief Test driver for Zoltan2. Facillitates generation of test problem via
 * a simple .xml input interface
 */

// taking headers from existing driver template
// will keep or remove as needed
#include <UserInputForTests.hpp>
#include <Zoltan2_Typedefs.hpp>
#include <AdapterForTests.hpp>
#include <Zoltan2_ComparisonHelper.hpp>
#include <Zoltan2_MetricAnalyzer.hpp>

#include <Zoltan2_ProblemFactory.hpp>
#include <Zoltan2_BasicIdentifierAdapter.hpp>
#include <Zoltan2_XpetraCrsGraphAdapter.hpp>
#include <Zoltan2_XpetraCrsMatrixAdapter.hpp>
#include <Zoltan2_XpetraMultiVectorAdapter.hpp>

#include <Zoltan2_Parameters.hpp>

#include <Teuchos_DefaultComm.hpp>
#include <Teuchos_XMLObject.hpp>
#include <Teuchos_FileInputSource.hpp>

#include <sstream>
#include <string>
#include <map>
#include <iostream>
#include <queue>

//#include <BDD_PamgenUtils.hpp>

using Teuchos::ParameterList;
using Teuchos::Comm;
using Teuchos::RCP;
using Teuchos::ArrayRCP;
using Teuchos::XMLObject;
using namespace Zoltan2_TestingFramework;

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::map;
using std::pair;
using std::exception;
using std::ostringstream;
using std::queue;

#define ERRMSG(msg) if (rank == 0){ cerr << "FAIL: " << msg << endl; }
#define EXC_ERRMSG(msg, e) \
if (rank==0){ cerr << "FAIL: " << msg << endl << e.what() << endl;}

// temporary methods for debugging and leanring
//typedef Zoltan2::MetricValues<zscalar_t> metric_t; // typedef metric_type

void xmlToModelPList(const Teuchos::XMLObject &xml, Teuchos::ParameterList & plist)
{
  // This method composes a plist for the problem
  Teuchos::XMLParameterListReader reader;
  plist = reader.toParameterList(xml);
  
  //  Get list of valid Zoltan2 Parameters
  // Zoltan 2 parameters appear in the input file
  // Right now we have default values stored in
  // the parameter list, we would like to apply
  // the options specified by the user in their
  // input file
  Teuchos::ParameterList zoltan2Parameters;
  Zoltan2::createAllParameters(zoltan2Parameters);
  
  if (plist.isSublist("Zoltan2Parameters")) {
    // Apply user specified zoltan2Parameters
    ParameterList &sub = plist.sublist("Zoltan2Parameters");
    zoltan2Parameters.setParameters(sub);
  }
  
  zoltan2Parameters.set("compute_metrics", "true");

}

void getParameterLists(const string &inputFileName,
                       queue<ParameterList> &problems,
                       queue<ParameterList> &comparisons,
                       const RCP<const Teuchos::Comm<int> > & comm)
{
  int rank = comm->getRank();
  // return a parameter list of problem definitions
  // and a parameter list for solution comparisons
  Teuchos::FileInputSource inputSource(inputFileName);
  if(rank == 0) cout << "input file source: " << inputFileName << endl;
  XMLObject xmlInput;
  
  // Try to get xmlObject from inputfile
  try{
    xmlInput = inputSource.getObject();
  }
  catch(exception &e)
  {
    EXC_ERRMSG("Test Driver error: reading", e); // error reading input
  }
  
  // get the parameter lists for each model
  for(int i = 0; i < xmlInput.numChildren(); i++)
  {
    ParameterList plist;
    xmlToModelPList(xmlInput.getChild(i), plist);
    if(plist.name() == "Comparison") comparisons.emplace(plist);
    else problems.emplace(plist);
  }
}

void run(const UserInputForTests &uinput,
         const ParameterList &problem_parameters,
         RCP<ComparisonHelper> & comparison_helper,
         const RCP<const Teuchos::Comm<int> > & comm)
{
  // Major steps in running a problem in zoltan 2
  // 1. get an input adapter
  // 2. construct the problem
  // 3. solve the problem
  // 4. analyze metrics
  // 5. clean up

  int rank = comm->getRank();
  
  if(!problem_parameters.isParameter("kind"))
  {
    if(rank == 0) std::cerr << "Problem kind not provided" << std::endl;
    return;
  }
  if(!problem_parameters.isParameter("InputAdapterParameters"))
  {
    if(rank == 0) std::cerr << "Input adapter parameters not provided" << std::endl;
    return;
  }
  if(!problem_parameters.isParameter("Zoltan2Parameters"))
  {
    if(rank == 0) std::cerr << "Zoltan2 problem parameters not provided" << std::endl;
    return;
  }
  if(rank == 0)
    cout << "\n\nRunning test: " << problem_parameters.name() << endl;
  
  ////////////////////////////////////////////////////////////
  // 0. add comparison source
  ////////////////////////////////////////////////////////////
  ComparisonSource * comparison_source = new ComparisonSource;
  comparison_helper->AddSource(problem_parameters.name(), comparison_source);
  comparison_source->addTimer("adapter construction time");
  comparison_source->addTimer("problem construction time");
  comparison_source->addTimer("solve time");
  ////////////////////////////////////////////////////////////
  // 1. get basic input adapter
  ////////////////////////////////////////////////////////////
  
  const ParameterList &adapterPlist = problem_parameters.sublist("InputAdapterParameters");
  comparison_source->timers["adapter construction time"]->start();
  base_adapter_t * ia = AdapterForTests::getAdapterForInput(const_cast<UserInputForTests *>(&uinput), adapterPlist,comm); // a pointer to a basic type
  comparison_source->timers["adapter construction time"]->stop();
  
//  if(rank == 0) cout << "Got input adapter... " << endl;
  if(ia == nullptr)
  {
    if(rank == 0)
      cout << "Get adapter for input failed" << endl;
    return;
  }
  
  ////////////////////////////////////////////////////////////
  // 2. construct a Zoltan2 problem
  ////////////////////////////////////////////////////////////
  string adapter_name = adapterPlist.get<string>("input adapter"); // If we are here we have an input adapter, no need to check for one.
  // get Zoltan2 partion parameters
  ParameterList zoltan2_parameters = const_cast<ParameterList &>(problem_parameters.sublist("Zoltan2Parameters"));
  
  //if(rank == 0){
  //  cout << "\nZoltan 2 parameters:" << endl;
  //  zoltan2_parameters.print(std::cout);
  //  cout << endl;
  //}

  comparison_source->timers["problem construction time"]->start();
  std::string problem_kind = problem_parameters.get<std::string>("kind"); 
  if (rank == 0) std::cout << "Creating a new " << problem_kind << " problem." << std::endl;
#ifdef HAVE_ZOLTAN2_MPI
  base_problem_t * problem = 
    Zoltan2_TestingFramework::ProblemFactory::newProblem(problem_kind, adapter_name, ia, &zoltan2_parameters, MPI_COMM_WORLD);
#else
  base_problem_t * problem = 
    Zoltan2_TestingFramework::ProblemFactory::newProblem(problem_kind, adapter_name, ia, &zoltan2_parameters);
#endif

  if (problem == nullptr) {
    if (rank == 0)
      std::cerr << "Input adapter type: " + adapter_name + ", is unvailable, or misspelled." << std::endl;
    return;
  }

  ////////////////////////////////////////////////////////////
  // 3. Solve the problem
  ////////////////////////////////////////////////////////////
  
  comparison_source->timers["solve time"]->start();
  if (problem_kind == "partitioning") {
    reinterpret_cast<partitioning_problem_t *>(problem)->solve();
  } else if (problem_kind == "ordering") {
    reinterpret_cast<ordering_problem_t *>(problem)->solve();
  } else if (problem_kind == "coloring") {
    reinterpret_cast<coloring_problem_t *>(problem)->solve();
  }
  comparison_source->timers["solve time"]->stop();
  if (rank == 0)
    cout << problem_kind + "Problem solved." << endl;
  
//#define KDDKDD
#ifdef KDDKDD
  {
  const base_adapter_t::gno_t *kddIDs = NULL;
  ia->getIDsView(kddIDs);
    for (size_t i = 0; i < ia->getLocalNumIDs(); i++) {
      std::cout << rank << " LID " << i
                << " GID " << kddIDs[i]
                << " PART " 
                << reinterpret_cast<partitioning_problem_t *>(problem)->getSolution().getPartListView()[i]
                << std::endl;
    }
  }
#endif

  ////////////////////////////////////////////////////////////
  // 4. Print problem metrics
  ////////////////////////////////////////////////////////////
  // An environment.  This is usually created by the problem.
  // BDD unused, only applicable to partitioning problems
  // RCP<const Zoltan2::Environment> env =
  //   reinterpret_cast<partitioning_problem_t *>(problem)->getEnvironment();

  if( problem_parameters.isParameter("Metrics")) {
    // calculate pass fail based on imbalance
    if(rank == 0) cout << "Analyzing metrics..." << endl;
    if(rank == 0 && problem_kind == "partitioning") {
      std::cout << "Print the " << problem_kind << "problem's metrics:" << std::endl; 
      reinterpret_cast<partitioning_problem_t *>(problem)->printMetrics(cout);
    }

    std::ostringstream msg;
    auto metricsPlist = problem_parameters.sublist("Metrics");
    bool all_tests_pass = false;
    
    if (problem_kind == "partitioning") {
      all_tests_pass
      = MetricAnalyzer<partitioning_problem_t>::analyzeMetrics( metricsPlist,
                                                                reinterpret_cast<const partitioning_problem_t *>(const_cast<base_problem_t *>(problem)),
                                                                comm,
                                                                msg); 
    } else if (problem_kind == "ordering") {
      all_tests_pass
      = MetricAnalyzer<ordering_problem_t>::analyzeMetrics( metricsPlist,
                                                            reinterpret_cast<const ordering_problem_t *>(const_cast<base_problem_t *>(problem)),
                                                            comm,
                                                            msg); 
    } else if (problem_kind == "coloring") {
      all_tests_pass
      = MetricAnalyzer<coloring_problem_t>::analyzeMetrics( metricsPlist,
                                                            reinterpret_cast<const coloring_problem_t *>(const_cast<base_problem_t *>(problem)),
                                                            comm,
                                                            msg); 
    }
    std::cout << msg.str() << std::endl;
    if(rank == 0 && all_tests_pass) cout << "All tests PASSED." << endl;
    else if(rank == 0) cout << "Testing FAILED." << endl;
    
  }else if(rank == 0 && problem_kind == "partitioning") {
    // BDD for debugging
    cout << "No test metrics requested. Problem Metrics are: " << endl;
    reinterpret_cast<partitioning_problem_t *>(problem)->printMetrics(cout);
    cout << "PASSED." << endl;
  }

#define BDD
#ifdef BDD 
  if (problem_kind == "ordering") {
    std::cout << "\nLet's examine the solution..." << std::endl;
    auto solution = reinterpret_cast<ordering_problem_t *>(problem)->getSolution();
    if (solution->haveSeparators() ) {
      
      std::ostringstream sol;
      sol << "Number of column blocks: " << solution->getSepColBlockCount() << std::endl;
      if (solution->getPermutationSize() < 100) {
        if (solution->havePerm()) {
          sol << "permutation: {";
          for (auto &x : solution->getPermutationRCPConst(false)) sol << " " << x;
          sol << "}" << std::endl;
        }
       
       if (solution->haveInverse()) { 
          sol << "inverse permutation: {";
          for (auto &x : solution->getPermutationRCPConst(true)) sol << " " << x;
          sol << "}" << std::endl;
       }
        
       if (solution->haveSepRange()) {
          sol << "separator range: {";
          for (auto &x : solution->getSepRangeRCPConst()) sol << " " << x;
          sol << "}" << std::endl;
       }
       
        if (solution->haveSepTree()) { 
          sol << "separator tree: {";
          for (auto &x : solution->getSepTreeRCPConst()) sol << " " << x;
          sol << "}" << std::endl;
        }
      }

      std::cout << sol.str() << std::endl;
    }
  }
#endif
  // 4b. timers
//  if(zoltan2_parameters.isParameter("timer_output_stream"))
//    reinterpret_cast<partitioning_problem_t *>(problem)->printTimers();
  
  ////////////////////////////////////////////////////////////
  // 5. Add solution to map for possible comparison testing
  ////////////////////////////////////////////////////////////
  comparison_source->adapter = RCP<basic_id_t>(reinterpret_cast<basic_id_t *>(ia));
  comparison_source->problem = RCP<base_problem_t>(reinterpret_cast<base_problem_t *>(problem));
  comparison_source->problem_kind = problem_parameters.isParameter("kind") ? problem_parameters.get<string>("kind") : "?";
  comparison_source->adapter_kind = adapter_name;
  
  // write mesh solution
//  auto sol = reinterpret_cast<partitioning_problem_t *>(problem)->getSolution();
//  MyUtils::writePartionSolution(sol.getPartListView(), ia->getLocalNumIDs(), comm);

  ////////////////////////////////////////////////////////////
  // 6. Clean up
  ////////////////////////////////////////////////////////////

}

int main(int argc, char *argv[])
{
  ////////////////////////////////////////////////////////////
  // (0) Set up MPI environment and timer
  ////////////////////////////////////////////////////////////
  Teuchos::GlobalMPISession session(&argc, &argv);
  RCP<const Comm<int> > comm = Teuchos::DefaultComm<int>::getComm();
  
  int rank = comm->getRank(); // get rank
  
  ////////////////////////////////////////////////////////////
  // (1) Get and read the input file
  // the input file defines tests to be run
  ////////////////////////////////////////////////////////////
  string inputFileName(""); 
  if(argc > 1)
    inputFileName = argv[1]; // user has provided an input file
  else{
    if(rank == 0){
      std::cout << "\nFAILED to specify xml input file!" << std::endl;
      ostringstream msg;
      msg << "\nStandard use of test_driver.cpp:\n";
      msg << "mpiexec -n <procs> ./Zoltan2_test_driver.exe <input_file.xml>\n";
      std::cout << msg.str() << std::endl;
    }
    
    return 1;
  }

  ////////////////////////////////////////////////////////////
  // (2) Get All Input Parameter Lists
  ////////////////////////////////////////////////////////////
  queue<ParameterList> problems, comparisons;
  getParameterLists(inputFileName,problems, comparisons, comm);
  
  ////////////////////////////////////////////////////////////
  // (3) Get Input Data Parameters
  ////////////////////////////////////////////////////////////
  
  // assumes that first block will always be
  // the input block
  const ParameterList inputParameters = problems.front();
  if(inputParameters.name() != "InputParameters")
  {
    if(rank == 0)
      cout << "InputParameters not defined. Testing FAILED." << endl;
    return 1;
  }
  
  // get the user input for all tests
  UserInputForTests uinput(inputParameters,comm);
  problems.pop();
  comm->barrier();
  
  if(uinput.hasInput())
  {
    ////////////////////////////////////////////////////////////
    // (4) Perform all tests
    ////////////////////////////////////////////////////////////
//     pamgen debugging
//    MyUtils::writeMesh(uinput,comm);
//    MyUtils::getConnectivityGraph(uinput, comm);
        
    RCP<ComparisonHelper> comparison_manager
      = rcp(new ComparisonHelper);
    while (!problems.empty()) {
      run(uinput, problems.front(), comparison_manager, comm);
      problems.pop();
    }
    
    ////////////////////////////////////////////////////////////
    // (5) Compare solutions
    ////////////////////////////////////////////////////////////
    while (!comparisons.empty()) {
      comparison_manager->Compare(comparisons.front(),comm);
      comparisons.pop();
    }
    
  }else{
    if(rank == 0){
      cout << "\nFAILED to load input data source.";
      cout << "\nSkipping all tests." << endl;
    }
  }
  
  return 0;
}

