#include "detector.hh"
#include "G4Event.hh"
#include "G4SystemOfUnits.hh"

MySensitiveDetector::MySensitiveDetector(G4String name) : G4VSensitiveDetector(name) {}

MySensitiveDetector::~MySensitiveDetector() {}

G4bool MySensitiveDetector::ProcessHits (G4Step *aStep, G4TouchableHistory *ROhist) {
    G4Track *track = aStep -> GetTrack();
    if (track->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition()) {
        return false;
    }
    G4cout << "Particle name: " << track->GetDefinition()->GetParticleName() << G4endl;
    track -> SetTrackStatus(fStopAndKill);
    G4double energy = track->GetTotalEnergy();  // or preStepPoint->GetKineticEnergy();
    // G4cout << "Detector energy: " << energy / eV << G4endl;

    G4StepPoint *preStepPoint = aStep -> GetPreStepPoint();
    G4StepPoint *postStepPoint = aStep -> GetPostStepPoint();

    G4ThreeVector posPhoton = preStepPoint -> GetPosition();
    // G4cout << "Photon position: " << posPhoton << G4endl;

    const G4VTouchable *touchable = aStep -> GetPreStepPoint() -> GetTouchable();
    G4int copyNo = touchable -> GetCopyNumber();
    // G4cout << "Copy number: " << copyNo << G4endl;

    G4VPhysicalVolume *physVol = touchable -> GetVolume();
    G4ThreeVector posDetector = physVol -> GetTranslation();
    // G4cout << "Detector position: " << posDetector << G4endl;

    G4int evt = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
    auto man = G4AnalysisManager::Instance();
    man->FillNtupleDColumn(0, 0, posPhoton.x() / mm);  // x in mm
    man->FillNtupleDColumn(0, 1, posPhoton.y() / mm);  // y in mm
    man->FillNtupleDColumn(0, 2, posPhoton.z() / mm);  // z in mm
    man->FillNtupleIColumn(0, 3, evt);
    man->FillNtupleDColumn(0, 4, energy / eV);  // energy in eV
    man->AddNtupleRow(0);   // Commit row: IMPORTANT!

    return true;
}