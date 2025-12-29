#include "myRunAction.hh"

MyRunAction::MyRunAction() {
    G4AnalysisManager *man = G4AnalysisManager::Instance();

    man -> CreateNtuple("Hits", "Hits");
    man -> CreateNtupleDColumn("fX");
    man -> CreateNtupleDColumn("fY");
    man -> CreateNtupleDColumn("fZ");
    man -> CreateNtupleIColumn("fEvent");
    // man -> CreateNtupleDColumn("fEnergy");
    man -> FinishNtuple(0);
}

MyRunAction::~MyRunAction() {}

void MyRunAction::BeginOfRunAction(const G4Run*) {
    G4AnalysisManager *man = G4AnalysisManager::Instance();
    
    man -> OpenFile("output.root");
}

void MyRunAction::EndOfRunAction(const G4Run*) {
    G4AnalysisManager *man = G4AnalysisManager::Instance();

    man -> Write();
    man -> CloseFile();
    G4cout << "Root File generated." << G4endl;
}