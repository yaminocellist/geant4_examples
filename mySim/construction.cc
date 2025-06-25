#include "construction.hh"

MyDetectorConstruction::MyDetectorConstruction()
{}

MyDetectorConstruction::~MyDetectorConstruction()
{}

G4VPhysicalVolume *MyDetectorConstruction::Construct() {
    G4NistManager *nist = G4NistManager::Instance();

    // G4Material *SiO2 = new G4Material("SiO2", 2.201*g/cm3, 2);
    // SiO2 -> AddElement(nist -> FindOrBuildElement("Si"), 1);
    // SiO2 -> AddElement(nist -> FindOrBuildElement("O"), 2);

    // G4Material *H2O = new G4Material("H2O", 1.000*g/cm3, 2);
    // H2O -> AddElement(nist -> FindOrBuildElement("H"), 2);
    // H2O -> AddElement(nist -> FindOrBuildElement("O"), 1);

    // G4Element *C = nist -> FindOrBuildElement("C");

    // G4Material *Aerogel = new G4Material("Aerogel", 0.200*g/cm3, 3);
    // Aerogel -> AddMaterial(SiO2, 62.5*perCent);
    // Aerogel -> AddMaterial(H2O, 37.4*perCent);
    // Aerogel -> AddElement(C, 0.1*perCent);

    // G4double energy[2] = {1.239841939*eV/0.9, 1.239841939*eV/0.2};
    // // G4double energy[2] = {1.239841939*eV/0.2, 1.239841939*eV/0.9};
    // G4double rindexAerogel[2] = {1.1, 1.1};
    G4double rindexWorld[2]   = {1.0, 1.0};
    // G4MaterialPropertiesTable *mptAerogel = new G4MaterialPropertiesTable();
    // mptAerogel -> AddProperty("RINDEX", energy, rindexAerogel, 2);
    // Aerogel -> SetMaterialPropertiesTable(mptAerogel);

    // 1. Define the material using chemical composition

    G4Element* H = nist->FindOrBuildElement("H");
    G4Element* C = nist->FindOrBuildElement("C");

    // BC-408 is polystyrene-based: (C8H8)n
    G4Material* BC408 = new G4Material("BC408", 1.032*g/cm3, 2);
    BC408->AddElement(C, 8);
    BC408->AddElement(H, 8);

    // 2. Define optical properties
    const G4int nEntries = 2;
    G4double energy[nEntries] = {2.0*eV, 3.5*eV}; // Typical range of visible light

    // Refractive Index
    G4double rIndex[nEntries] = {1.58, 1.58}; // constant across range

    // Absorption length
    G4double absLength[nEntries] = {380.*cm, 380.*cm}; // just an example

    // Emission spectrum (simplified - usually a Gaussian or experimental data)
    G4double scintSpectrum[nEntries] = {1.0, 1.0}; // simplified (normally varies with energy)

    // 3. Create and attach material properties table
    G4MaterialPropertiesTable* mptBC408 = new G4MaterialPropertiesTable();
    mptBC408->AddProperty("RINDEX", energy, rIndex, nEntries);
    mptBC408->AddProperty("ABSLENGTH", energy, absLength, nEntries);
    mptBC408->AddProperty("SCINTILLATIONCOMPONENT1", energy, scintSpectrum, nEntries);

    mptBC408->AddConstProperty("SCINTILLATIONYIELD", 6400./MeV); // number of photons/MeV
    mptBC408->AddConstProperty("RESOLUTIONSCALE", 1.0); // resolution (1.0 = statistical limit)
    mptBC408->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 2.1*ns); // fast decay component
    mptBC408->AddConstProperty("SCINTILLATIONYIELD1", 1.0); // only one component

    BC408->SetMaterialPropertiesTable(mptBC408);
    // Optional: Birks constant (for quenching of high dE/dx)
    BC408->GetIonisation()->SetBirksConstant(0.126 * mm / MeV); // typical value for polystyrene

    G4Material *worldMat = nist->FindOrBuildMaterial("G4_AIR");

    G4MaterialPropertiesTable *mptWorld = new G4MaterialPropertiesTable();
    mptWorld -> AddProperty("RINDEX", energy, rindexWorld, 2);
    worldMat -> SetMaterialPropertiesTable(mptWorld);

    G4Box *solidWorld = new G4Box("solidWorld", .5*m, .5*m, .5*m);

    G4LogicalVolume *logicWorld = new G4LogicalVolume(solidWorld, 
                                                      worldMat,
                                                      "logicWorld");

    G4VPhysicalVolume *physWorld = new G4PVPlacement(0,
                                                    G4ThreeVector(0., 0., 0.),
                                                    logicWorld,
                                                    "physWorld",
                                                    0,
                                                    false,
                                                    0,
                                                    true);
    G4Box *solidRadiator = new G4Box("solidRadiator", 0.4*m, 0.4*m, 0.01*m);
    G4LogicalVolume *logicRadiator = new G4LogicalVolume(solidRadiator,
                                                         BC408,
                                                         "logicalRadiator");
    G4VPhysicalVolume *physRadiator = new G4PVPlacement(0,
                                                        G4ThreeVector(0., 0., 0.25*m),
                                                        logicRadiator,
                                                        "physRadiator",
                                                        logicWorld,
                                                        false,
                                                        0,
                                                        true);

    G4Box *solidDetector = new G4Box("solidDetector", 0.005*m, 0.005*m, 0.01*m);
    logicDetector = new G4LogicalVolume(solidDetector, worldMat, "logicDetector");
    for (G4int i = 0; i < 100; ++i) {
        for (G4int j = 0; j < 100; ++j) {
            G4VPhysicalVolume *physDetector = new G4PVPlacement(0,
                                                                G4ThreeVector(-0.5*m+(i+0.5)*m/100, -0.5*m+(j+0.5)*m/100, 0.49*m),
                                                                logicDetector,
                                                                "physDetector",
                                                                logicWorld,
                                                                false,
                                                                j+i*100,
                                                                true);
        }
    }

    return physWorld;
}

void MyDetectorConstruction::ConstructSDandField() {
    MySensitiveDetector *sensDet = new MySensitiveDetector("SensitiveDetector");
    logicDetector -> SetSensitiveDetector(sensDet);
}