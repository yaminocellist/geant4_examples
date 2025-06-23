#include "run.hh"

MyRunAction::MyRunAction() {}

MyRunAction::~MyRunAction() {}

void MyRunAction::BeginOfRunAction(const G4Run*) {
    G4AnalysisManager *man = G4AnalysisManager::Instance();

    man -> OpenFile("output.root");
    man -> CreateNtuple("Hits", "Hits");
    man -> CreateNtupleDColumn("fX");
    man -> CreateNtupleDColumn("fY");
    man -> CreateNtupleDColumn("fZ");
    man -> CreateNtupleIColumn("fEvent");
    man -> FinishNtuple(0);
}
void MyRunAction::EndOfRunAction(const G4Run*) {
    G4AnalysisManager *man = G4AnalysisManager::Instance();

    G4cout << "Writing ROOT file..." << G4endl;
man->Write();
G4cout << "Closing ROOT file..." << G4endl;
man->CloseFile();
G4cout << "ROOT file closed." << G4endl;

}
