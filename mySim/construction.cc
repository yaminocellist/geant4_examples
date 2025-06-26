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
    // G4MaterialPropertiesTable *mptAerogel = new G4MaterialPropertiesTable();
    // mptAerogel -> AddProperty("RINDEX", energy, rindexAerogel, 2);
    // Aerogel -> SetMaterialPropertiesTable(mptAerogel);

    // 1. Define the material using chemical composition

    G4Element* H = nist->FindOrBuildElement("H");
    G4Element* C = nist->FindOrBuildElement("C");
    G4Element* elN = new G4Element("Nitrogen", "N", 7., 14.01*g/mole);
    G4Element* elO = new G4Element("Oxygen", "O", 8., 16.00*g/mole);

    // Polystyrene (C8H8)n
    G4Material* polystyrene = new G4Material("Polystyrene", 1.05*g/cm3, 2);
    polystyrene->AddElement(C, 8);
    polystyrene->AddElement(H, 8);

    // PPO: C15H11NO (approximate)
    G4Material* PPO = new G4Material("PPO", 1.06*g/cm3, 3);
    PPO->AddElement(C, 15);
    PPO->AddElement(H, 11);
    PPO->AddElement(elN, 1);

    // POPOP: C24H15N2O2 (approximate)
    G4Material* POPOP = new G4Material("POPOP", 1.20*g/cm3, 4);
    POPOP->AddElement(C, 24);
    POPOP->AddElement(H, 15);
    POPOP->AddElement(elN, 2);
    POPOP->AddElement(elO, 2);

    // Create the final scintillator material (by weight fractions)
    G4Material* MINERVA_Scintillator = new G4Material("MINERVA_Scintillator", 1.05*g/cm3, 3);
    MINERVA_Scintillator->AddMaterial(polystyrene, 98.97*perCent);
    MINERVA_Scintillator->AddMaterial(PPO, 1.00*perCent);
    MINERVA_Scintillator->AddMaterial(POPOP, 0.03*perCent);

    // 2. Define optical properties
    const G4int nEntries = 2;
    G4double energy[nEntries] = {2.0*eV, 3.5*eV}; // Typical range of visible light

    // Refractive Index
    G4double rIndex[nEntries] = {1.58, 1.58}; // constant across range

    // Absorption length
    G4double absLength[nEntries] = {380.*cm, 380.*cm}; // just an example

    // Emission spectrum (simplified - usually a Gaussian or experimental data)
    G4double scintSpectrum[nEntries] = {1.0, 1.0}; // simplified (normally varies with energy)

    // G4MaterialPropertiesTable* mptScint = new G4MaterialPropertiesTable();
    // mptScint->AddProperty("RINDEX", energy, rIndex, nEntries);
    // mptScint->AddProperty("ABSLENGTH", energy, absLength, nEntries);
    // mptScint->AddProperty("SCINTILLATIONCOMPONENT1", energy, scintSpectrum, nEntries);

    // mptScint->AddConstProperty("SCINTILLATIONYIELD", 7349.76/MeV); // number of photons/MeV
    // mptScint->AddConstProperty("RESOLUTIONSCALE", 1.0); // resolution (1.0 = statistical limit)
    // mptScint->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 2.1*ns); // fast decay component
    // mptScint->AddConstProperty("SCINTILLATIONYIELD1", 1.0); // only one component

    // MINERVA_Scintillator->SetMaterialPropertiesTable(mptScint);

    const G4int NUMENTRIES = 4;
    G4double photonEnergy[NUMENTRIES] = {
        2.48 * eV, // 500 nm
        2.75 * eV, // 450 nm
        2.95 * eV, // 420 nm - peak
        3.10 * eV  // 400 nm
    };

    // Emission spectrum (normalized) – centered at 420 nm
    G4double scintFast[NUMENTRIES] = {
        0.10,  // 500 nm
        0.60,  // 450 nm
        1.00,  // 420 nm
        0.60   // 400 nm
    };

    G4double scintSlow[NUMENTRIES] = {0.0, 0.0, 0.0, 0.0};  // no slow component
    G4double refractiveIndex[NUMENTRIES] = {1.58, 1.58, 1.58, 1.58};
    G4double absorptionLength[NUMENTRIES] = {
        30.52*cm, 35.0*cm, 42.15*cm, 42.15*cm
    };

    G4MaterialPropertiesTable* MPT = new G4MaterialPropertiesTable();
    MPT->AddProperty("RINDEX", photonEnergy, refractiveIndex, NUMENTRIES);
    MPT->AddProperty("ABSLENGTH", photonEnergy, absorptionLength, NUMENTRIES);
    MPT->AddProperty("SCINTILLATIONCOMPONENT1", photonEnergy, scintFast, NUMENTRIES);
    MPT->AddProperty("SCINTILLATIONCOMPONENT2", photonEnergy, scintSlow, NUMENTRIES);

    // Constants (typical values)
    MPT->AddConstProperty("SCINTILLATIONYIELD", 7349.76 / MeV);
    MPT->AddConstProperty("RESOLUTIONSCALE", 1.0);
    MPT->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 2.8 * ns);
    MPT->AddConstProperty("SCINTILLATIONTIMECONSTANT2", 0.0 * ns);
    MPT->AddConstProperty("YIELDRATIO", 1.0, true);

    MINERVA_Scintillator->SetMaterialPropertiesTable(MPT);


    // // BC-408 is polystyrene-based: (C8H8)n
    // G4Material* BC408 = new G4Material("BC408", 1.032*g/cm3, 2);
    // BC408->AddElement(C, 8);
    // BC408->AddElement(H, 8);

    // // 3. Create and attach material properties table
    // G4MaterialPropertiesTable* mptBC408 = new G4MaterialPropertiesTable();
    // mptBC408->AddProperty("RINDEX", energy, rIndex, nEntries);
    // mptBC408->AddProperty("ABSLENGTH", energy, absLength, nEntries);
    // mptBC408->AddProperty("SCINTILLATIONCOMPONENT1", energy, scintSpectrum, nEntries);

    // mptBC408->AddConstProperty("SCINTILLATIONYIELD", 7349.76/MeV); // number of photons/MeV
    // mptBC408->AddConstProperty("RESOLUTIONSCALE", 1.0); // resolution (1.0 = statistical limit)
    // mptBC408->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 2.1*ns); // fast decay component
    // mptBC408->AddConstProperty("SCINTILLATIONYIELD1", 1.0); // only one component

    // BC408->SetMaterialPropertiesTable(mptBC408);
    // // Optional: Birks constant (for quenching of high dE/dx)
    // BC408->GetIonisation()->SetBirksConstant(0.126 * mm / MeV); // typical value for polystyrene

    G4Material *worldMat = nist->FindOrBuildMaterial("G4_AIR");

    G4MaterialPropertiesTable *mptWorld = new G4MaterialPropertiesTable();
    G4double rindexWorld[2]   = {1.0, 1.0};
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
                                                         MINERVA_Scintillator,
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