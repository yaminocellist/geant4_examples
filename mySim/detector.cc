#include "detector.hh"

MySensitiveDetector::MySensitiveDetector(G4String name):
G4VSensitiveDetector(name) {
    qEff = new G4PhysicsOrderedFreeVector();
    
    std::ifstream dataFile;
    dataFile.open("qEff.dat");

    while(1) {
        G4double wlen, qEff_value;
        dataFile >> wlen >> qEff_value;
        if (dataFile.eof()) break;
        qEff -> InsertValues(wlen, qEff_value/100.);
    }
    dataFile.close();
    // qEff -> SetSpline(false);
}

MySensitiveDetector::~MySensitiveDetector() {}

G4bool MySensitiveDetector::ProcessHits(G4Step *aStep, G4TouchableHistory *R0hist) {
    G4Track *track = aStep -> GetTrack();
    track -> SetTrackStatus(fStopAndKill);

    G4StepPoint *preStepPoint  = aStep -> GetPreStepPoint();
    G4StepPoint *postStepPoint = aStep -> GetPostStepPoint();

    G4ThreeVector posPhoton = preStepPoint -> GetPosition();
    G4ThreeVector momPhoton = preStepPoint -> GetMomentum();
    G4double wlen = (1.239841939*eV/momPhoton.mag())*1E+03;

    // G4cout << "Photon position: " << posPhoton << G4endl;
    const G4VTouchable *touchable = aStep -> GetPreStepPoint() -> GetTouchable();
    G4int copyNum = touchable -> GetCopyNumber();
    // G4cout << "Copy Number is: " << copyNum << G4endl;

    G4VPhysicalVolume *physVol = touchable -> GetVolume();
    G4ThreeVector posDetector = physVol -> GetTranslation();
    // G4cout << "Position of Detector is; " << posDetector << G4endl;
    G4double energy = track->GetTotalEnergy();
    G4int evt = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
    G4AnalysisManager *man = G4AnalysisManager::Instance();
    // man->FillNtupleDColumn(0, 0, posPhoton.x() / mm);  // x in mm
    // man->FillNtupleDColumn(0, 1, posPhoton.y() / mm);  // y in mm
    // man->FillNtupleDColumn(0, 2, posPhoton.z() / mm);  // z in mm
    man -> FillNtupleDColumn(0, 0, posDetector[0]);
    man -> FillNtupleDColumn(0, 1, posDetector[1]);
    man -> FillNtupleDColumn(0, 2, posDetector[2]);
    man -> FillNtupleDColumn(0, 3, wlen);
    man -> FillNtupleIColumn(0, 4, evt);
    // man->FillNtupleDColumn(0, 4, energy / eV);  // energy in eV
    man -> AddNtupleRow(0);   // Commit row: IMPORTANT!

    if (G4UniformRand() < qEff->Value(wlen)) {
        man -> FillNtupleDColumn(1, 0, posDetector[0]);
        man -> FillNtupleDColumn(1, 1, posDetector[1]);
        man -> FillNtupleDColumn(1, 2, posDetector[2]);
        man -> FillNtupleIColumn(1, 3, evt);
    // man->FillNtupleDColumn(0, 4, energy / eV);  // energy in eV
        man -> AddNtupleRow(1);   // Commit row: IMPORTANT!
    }

    return true;
}