#include <iostream>

#include <Teuchos_GlobalMPISession.hpp>
#include <Teuchos_DefaultComm.hpp>

#include <Teuchos_ParameterList.hpp>
#include <Teuchos_TestForException.hpp>
#include <Teuchos_CommandLineProcessor.hpp>
#include <Teuchos_oblackholestream.hpp>
#include <Teuchos_VerboseObject.hpp>
#include <Teuchos_FancyOStream.hpp>

#define CTHULHU_ENABLED

#include "MueLu_MatrixFactory.hpp"
#include "MueLu_MatrixTypes.hpp"

#include "Cthulhu_Map.hpp"
#include "Cthulhu_CrsMatrix.hpp"

#include "Cthulhu_TpetraMap.hpp"
#include "Cthulhu_TpetraCrsMatrix.hpp"

#include "Cthulhu_EpetraMap.hpp"
#include "Cthulhu_EpetraCrsMatrix.hpp"

#include "Tpetra_Map.hpp"
#include "Tpetra_CrsMatrix.hpp"

// Define data types
typedef double SC;
typedef int    LO;
typedef int    GO;

using Teuchos::RCP;

// This is a test to see if the gallery is working with allowed matrices types.

int main(int argc, char* argv[]) 
{
  Teuchos::oblackholestream blackhole;
  Teuchos::GlobalMPISession mpiSession(&argc,&argv,&blackhole);
  Teuchos::RCP<const Teuchos::Comm<int> > comm = Teuchos::DefaultComm<int>::getComm();

  GO nx=4;
  GO ny=4;
  GO nz=4;
  std::string matrixType("Laplace1D");

  Teuchos::ParameterList pl;
  GO numGlobalElements = nx*ny;
  LO indexBase = 0;

  Teuchos::ParameterList matrixList;
  matrixList.set("nx",nx);
  matrixList.set("ny",ny);
  matrixList.set("nz",nz);

  RCP<Teuchos::FancyOStream> out = Teuchos::fancyOStream(Teuchos::rcpFromRef(std::cout));

  // Tpetra::CrsMatrix
  {
    RCP<const Tpetra::Map<LO,GO> > map = rcp( new Tpetra::Map<LO,GO> (numGlobalElements, indexBase, comm) ); 
    RCP<Tpetra::CrsMatrix<SC,LO,GO> > A = MueLu::Gallery::CreateCrsMatrix<SC,LO,GO, Tpetra::Map<LO,GO>, Tpetra::CrsMatrix<SC,LO,GO> > (matrixType,map,matrixList);
    A->describe(*out, Teuchos::VERB_EXTREME);
  }

  // Cthulhu::TpetraCrsMatrix
  {
    RCP<const Cthulhu::TpetraMap<LO,GO> > map = rcp( new Cthulhu::TpetraMap<LO,GO> (numGlobalElements, indexBase, comm) );
    RCP<Cthulhu::TpetraCrsMatrix<SC,LO,GO> > A = MueLu::Gallery::CreateCrsMatrix<SC,LO,GO, Cthulhu::TpetraMap<LO,GO>, Cthulhu::TpetraCrsMatrix<SC,LO,GO> > (matrixType,map,matrixList);
    A->describe(*out, Teuchos::VERB_EXTREME);
  }
  
  // Cthulhu::EpetraCrsMatrix
  { 
    RCP<const Cthulhu::EpetraMap > map = rcp( new Cthulhu::EpetraMap (numGlobalElements, indexBase, comm) );
    RCP<Cthulhu::EpetraCrsMatrix> A = MueLu::Gallery::CreateCrsMatrix<SC,LO,GO, Cthulhu::EpetraMap, Cthulhu::EpetraCrsMatrix> (matrixType,map,matrixList);
    A->describe(*out, Teuchos::VERB_EXTREME);
  } 
  
  // Cthulhu::CrsMatrix (Tpetra)
  {
    RCP<const Cthulhu::Map<LO,GO> > map = rcp( new Cthulhu::TpetraMap<LO,GO> (numGlobalElements, indexBase, comm) ); 
    RCP<Cthulhu::CrsMatrix<SC,LO,GO> > A = MueLu::Gallery::CreateCrsMatrix<SC,LO,GO, Cthulhu::Map<LO,GO>, Cthulhu::CrsMatrix<SC,LO,GO> >  (matrixType,map,matrixList);
    A->describe(*out, Teuchos::VERB_EXTREME);
  } 

  // Cthulhu::CrsMatrix (Epetra)
  {
    RCP<const Cthulhu::Map<LO,GO> > map = rcp( new Cthulhu::EpetraMap (numGlobalElements, indexBase, comm) );
    RCP<Cthulhu::CrsMatrix<SC,LO,GO> > A = MueLu::Gallery::CreateCrsMatrix<SC,LO,GO, Cthulhu::Map<LO,GO>, Cthulhu::CrsMatrix<SC,LO,GO> >  (matrixType,map,matrixList);
    A->describe(*out, Teuchos::VERB_EXTREME);
  } 

 return EXIT_SUCCESS;
}
