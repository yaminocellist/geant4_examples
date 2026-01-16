#include "myRunAction.hh"

MyRunAction::MyRunAction() {
    G4AnalysisManager *man = G4AnalysisManager::Instance();

    man -> CreateNtuple("Photons", "Photons");
    man -> CreateNtupleDColumn("fX");
    man -> CreateNtupleDColumn("fY");
    man -> CreateNtupleDColumn("fZ");
    man -> CreateNtupleDColumn("fWlen");
    man -> CreateNtupleIColumn("fEvent");
    man -> FinishNtuple(0);

    man -> CreateNtuple("Hits", "Hits");
    man -> CreateNtupleDColumn("fX");
    man -> CreateNtupleDColumn("fY");
    man -> CreateNtupleDColumn("fZ");
    man -> CreateNtupleIColumn("fEvent");
    // man -> CreateNtupleDColumn("fEnergy");
    man -> FinishNtuple(1);
}

MyRunAction::~MyRunAction() {}

void MyRunAction::BeginOfRunAction(const G4Run* run) {
    G4AnalysisManager *man = G4AnalysisManager::Instance();

    G4int runID = run -> GetRunID();
    std::stringstream strRunID;
    strRunID << runID;
    
    man -> OpenFile("output"+strRunID.str()+".root");
}

void MyRunAction::EndOfRunAction(const G4Run*) {
    G4AnalysisManager *man = G4AnalysisManager::Instance();

    man -> Write();
    man -> CloseFile();
    G4cout << "Root File generated." << G4endl;
}