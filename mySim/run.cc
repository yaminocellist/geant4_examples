#include "run.hh"

MyRunAction::MyRunAction() {
    G4AnalysisManager *man = G4AnalysisManager::Instance();

    // man -> OpenFile("output.root");
    man -> CreateNtuple("Hits", "Hits");
    man -> CreateNtupleDColumn("fX");
    man -> CreateNtupleDColumn("fY");
    man -> CreateNtupleDColumn("fZ");
    man -> CreateNtupleIColumn("fEvent");
    man -> CreateNtupleDColumn("fEnergy");
    man -> FinishNtuple(0);
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

    G4cout << "Writing ROOT file..." << G4endl;
man->Write();
G4cout << "Closing ROOT file..." << G4endl;
man->CloseFile();
G4cout << "ROOT file closed." << G4endl;

}
