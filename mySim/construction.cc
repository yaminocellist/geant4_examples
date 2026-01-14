#include "construction.hh"

#include "G4SDManager.hh"
#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4ThreeVector.hh"
#include "G4ios.hh"

MyDetectorConstruction::MyDetectorConstruction()
: nCols(100), nRows(100)
{
    fMessenger = new G4GenericMessenger(this, "/detector/", "Detector control");

    fMessenger->DeclareProperty("nCols", nCols, "Number of columns")
        .SetStates(G4State_PreInit, G4State_Idle);

    fMessenger->DeclareProperty("nRows", nRows, "Number of rows")
        .SetStates(G4State_PreInit, G4State_Idle);
}

MyDetectorConstruction::~MyDetectorConstruction()
{
    delete fMessenger;
}

void MyDetectorConstruction::DefineMaterials()
{
    if (worldMat) return;   // already defined

    G4NistManager* nist = G4NistManager::Instance();

    SiO2 = new G4Material("SiO2", 2.201*g/cm3, 2);
    SiO2->AddElement(nist->FindOrBuildElement("Si"), 1);
    SiO2->AddElement(nist->FindOrBuildElement("O"), 2);

    H2O = new G4Material("H2O", 1.000*g/cm3, 2);
    H2O->AddElement(nist->FindOrBuildElement("H"), 2);
    H2O->AddElement(nist->FindOrBuildElement("O"), 1);

    C = nist->FindOrBuildElement("C");

    Aerogel = new G4Material("Aerogel", 0.200*g/cm3, 3);
    Aerogel->AddMaterial(SiO2, 62.5*perCent);
    Aerogel->AddMaterial(H2O, 37.4*perCent);
    Aerogel->AddElement(C, 0.1*perCent);

    worldMat = nist->FindOrBuildMaterial("G4_AIR");

    // Optical properties
    G4double energy[2]        = {1.239841939*eV/0.9, 1.239841939*eV/0.2};
    G4double rindexWorld[2]   = {1.0, 1.0};
    G4double rindexAerogel[2] = {1.1, 1.1};

    auto mptWorld = new G4MaterialPropertiesTable();
    mptWorld->AddProperty("RINDEX", energy, rindexWorld, 2);
    worldMat->SetMaterialPropertiesTable(mptWorld);

    auto mptAerogel = new G4MaterialPropertiesTable();
    mptAerogel->AddProperty("RINDEX", energy, rindexAerogel, 2);
    Aerogel->SetMaterialPropertiesTable(mptAerogel);
}

G4VPhysicalVolume* MyDetectorConstruction::Construct()
{
    // FULL geometry reset (required for reinitializeGeometry)
    G4GeometryManager::GetInstance()->OpenGeometry();
    G4PhysicalVolumeStore::GetInstance()->Clean();
    G4LogicalVolumeStore::GetInstance()->Clean();
    G4SolidStore::GetInstance()->Clean();

    DefineMaterials();

    G4cout << "[Detector] Building geometry: "
           << nCols << " x " << nRows << G4endl;

    // World
    G4double worldSize = 0.5*m;
    G4double xWorld, yWorld, zWorld;
    xWorld = worldSize; yWorld = worldSize; zWorld = worldSize;
    solidWorld = new G4Box("solidWorld", xWorld, yWorld, zWorld);
    logicalWorld = new G4LogicalVolume(solidWorld, worldMat, "logicalWorld");
    physWorld = new G4PVPlacement(
        nullptr, {}, logicalWorld, "physWorld", nullptr, false, 0, true);

    // Radiator
    solidRadiator = new G4Box("solidRadiator", 0.4*m, 0.4*m, 0.01*m);
    logicalRadiator = new G4LogicalVolume(
        solidRadiator, Aerogel, "logicalRadiator");

    fScoringVolume = logicalRadiator;

    physRadiator = new G4PVPlacement(
        nullptr,
        G4ThreeVector(0., 0., 0.25*m),
        logicalRadiator,
        "physRadiator",
        logicalWorld,
        false,
        0,
        true);

    // Detector pixels
    solidDetector = new G4Box(
        "solidDetector",
        xWorld / nRows,
        yWorld / nCols,
        0.01*m);

    logicalDetector = new G4LogicalVolume(
        solidDetector, worldMat, "logicalDetector");

    for (G4int row = 0; row < nRows; ++row) {
        for (G4int col = 0; col < nCols; ++col) {
            new G4PVPlacement(
                nullptr,
                G4ThreeVector(
                    -xWorld + (row + 0.5) * 2 * xWorld / nRows,
                    -yWorld + (col + 0.5) * 2 * yWorld / nCols,
                    0.49*m),
                logicalDetector,
                "physDetector",
                logicalWorld,
                false,
                col + row * nCols,
                true);
        }
    }

    return physWorld;
}

// void MyDetectorConstruction::ConstructSDandField()
// {
//     auto sd = new MySensitiveDetector("SensitiveDetector");

//     auto sdManager = G4SDManager::GetSDMpointer();
//     sdManager->AddNewDetector(sd);

//     logicalDetector->SetSensitiveDetector(sd);
// }
void MyDetectorConstruction::ConstructSDandField()
{
    auto sdManager = G4SDManager::GetSDMpointer();

    auto sd = sdManager->FindSensitiveDetector("SensitiveDetector", false);
    if (!sd) {
        sd = new MySensitiveDetector("SensitiveDetector");
        sdManager->AddNewDetector(sd);
    }

    logicalDetector->SetSensitiveDetector(sd);
}
