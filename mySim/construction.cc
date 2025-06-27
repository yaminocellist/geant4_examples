#include "construction.hh"

MyDetectorConstruction::MyDetectorConstruction()
{
    fMessenger = new G4GenericMessenger(this, "/detector/", "Detector Construction");

    fMessenger ->DeclareProperty("nRows", nRows, "Number of rows");
    fMessenger ->DeclareProperty("nCols", nCols, "Number of columns");

    nRows = 64;
    nCols = 64;

    DefineMaterials();
}

MyDetectorConstruction::~MyDetectorConstruction()
{}

void MyDetectorConstruction::DefineMaterials () {
    H   = new G4Element("H", "H", 1., 1.01 * g / mole);
    C   = new G4Element("C", "C", 6., 12.01 * g / mole);
    elN = new G4Element("Nitrogen", "N", 7., 14.01*g/mole);
    elO = new G4Element("Oxygen", "O", 8., 16.00*g/mole);

    G4NistManager *nist = G4NistManager::Instance();

    SiO2 = new G4Material("SiO2", 2.201*g/cm3, 2);
    SiO2 -> AddElement(nist -> FindOrBuildElement("Si"), 1);
    SiO2 -> AddElement(nist -> FindOrBuildElement("O"), 2);

    H2O = new G4Material("H2O", 1.000*g/cm3, 2);
    H2O -> AddElement(nist -> FindOrBuildElement("H"), 2);
    H2O -> AddElement(nist -> FindOrBuildElement("O"), 1);

    // C = nist -> FindOrBuildElement("C");

    Aerogel = new G4Material("Aerogel", 0.200*g/cm3, 3);
    Aerogel -> AddMaterial(SiO2, 62.5*perCent);
    Aerogel -> AddMaterial(H2O, 37.4*perCent);
    Aerogel -> AddElement(C, 0.1*perCent);

    worldMat = nist->FindOrBuildMaterial("G4_AIR");

    G4double energy[2] = {1.239841939*eV/0.9, 1.239841939*eV/0.2};
    G4double rindexAerogel[2] = {1.1, 1.1};
    G4double rindexWorld[2]   = {1.0, 1.0};
    G4MaterialPropertiesTable *mptAerogel = new G4MaterialPropertiesTable();
    mptAerogel -> AddProperty("RINDEX", energy, rindexAerogel, 2);
    Aerogel -> SetMaterialPropertiesTable(mptAerogel);

    G4MaterialPropertiesTable *mptWorld = new G4MaterialPropertiesTable();
    mptWorld -> AddProperty("RINDEX", energy, rindexWorld, 2);
    worldMat -> SetMaterialPropertiesTable(mptWorld);

    // Polystyrene (C8H8)n
    polystyrene = new G4Material("Polystyrene", 1.05*g/cm3, 2);
    polystyrene->AddElement(C, 8);
    polystyrene->AddElement(H, 8);

    // PPO: C15H11NO (approximate)
    PPO = new G4Material("PPO", 1.06*g/cm3, 3);
    PPO->AddElement(C, 15);
    PPO->AddElement(H, 11);
    PPO->AddElement(elN, 1);

    // POPOP: C24H15N2O2 (approximate)
    POPOP = new G4Material("POPOP", 1.20*g/cm3, 4);
    POPOP->AddElement(C, 24);
    POPOP->AddElement(H, 15);
    POPOP->AddElement(elN, 2);
    POPOP->AddElement(elO, 2);

    // Create the final scintillator material (by weight fractions)
    MINERVA_Scintillator = new G4Material("MINERVA_Scintillator", 1.05*g/cm3, 3);
    MINERVA_Scintillator->AddMaterial(polystyrene, 98.97*perCent);
    MINERVA_Scintillator->AddMaterial(PPO, 1.00*perCent);
    MINERVA_Scintillator->AddMaterial(POPOP, 0.03*perCent);
}

G4VPhysicalVolume *MyDetectorConstruction::Construct() {
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

    G4double xWorld = 0.2*m, yWorld = 0.2*m, zWorld = 0.2*m;
    solidWorld = new G4Box("solidWorld", xWorld, yWorld, zWorld);

    logicWorld = new G4LogicalVolume(solidWorld, 
                                     worldMat,
                                     "logicWorld");

    physWorld = new G4PVPlacement(0,
                                  G4ThreeVector(0., 0., 0.),
                                  logicWorld,
                                  "physWorld",
                                  0,
                                  false,
                                  0,
                                  true);

    G4double xScint = 5*cm/2, yScint = 5*cm/2, zScint = 1*cm/2;
    solidRadiator = new G4Box("solidRadiator", xScint, yScint, zScint);
    logicRadiator = new G4LogicalVolume(solidRadiator,
                                        MINERVA_Scintillator,
                                        "logicalRadiator");
    physRadiator = new G4PVPlacement(0,
                                     G4ThreeVector(0., 0., 5*cm),
                                     logicRadiator,
                                     "physRadiator",
                                     logicWorld,
                                     false,
                                     0,
                                     true);

    solidDetector = new G4Box("solidDetector", xScint/nRows, yScint/nCols, 0.41*cm/2);
    logicDetector = new G4LogicalVolume(solidDetector, worldMat, "logicDetector");
    for (G4int i = 0; i < nRows; ++i) {
        for (G4int j = 0; j < nCols; ++j) {
            physDetector = new G4PVPlacement(0,
                                             G4ThreeVector(xScint*(-1+(2*static_cast<double>(i)+1)/nRows),yScint*(-1+(2*static_cast<double>(j)+1)/nCols),(5.5+0.41/2)*cm),
                                             logicDetector,
                                             "physDetector",
                                             logicWorld,
                                             false,
                                             j+i*nCols, // CopyNo, row-major index;
                                             true);
        }
    }

    return physWorld;
}

void MyDetectorConstruction::ConstructSDandField() {
    MySensitiveDetector *sensDet = new MySensitiveDetector("SensitiveDetector");
    logicDetector -> SetSensitiveDetector(sensDet);
}