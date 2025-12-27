#include "detector.hh"

MySensitiveDetector::MySensitiveDetector(G4String name):
                     G4VSensitiveDetector(name) {}

MySensitiveDetector::~MySensitiveDetector() {}

G4bool MySensitiveDetector::ProcessHits(G4Step *aStep, G4TouchableHistory *R0hist) {
    G4Track *track = aStep -> GetTrack();
    track -> SetTrackStatus(fStopAndKill);

    G4StepPoint *preStepPoint  = aStep -> GetPreStepPoint();
    G4StepPoint *postStepPoint = aStep -> GetPostStepPoint();

    G4ThreeVector posPhoton = preStepPoint -> GetPosition();

    G4cout << "Photon position: " << posPhoton << G4endl;

    return true;
}