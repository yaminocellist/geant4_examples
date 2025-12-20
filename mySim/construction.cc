#include "construction.hh"

MyDetectorConstruction::MyDetectorConstruction()
{}

MyDetectorConstruction::~MyDetectorConstruction()
{}

G4VPhysicalVolume *MyDetectorConstruction::Construct() {
    G4NistManager *nist = G4NistManager::Instance();

    G4Material *worldMat = nist -> FindOrBuildMaterial("G4 Air");

    G4Box *solidWorld = new G4Box("solidWorld", 0.5*m, 0.5*m, 0.5*m);

    G4LogicalVolume *logicalWorld = new G4LogicalVolume(solidWorld, 
                                                          worldMat,
                                                          "logicalWorld");

    G4VPhysicalVolume *physWorld = new G4PVPlacement(0,
                                                     G4ThreeVector(0., 0., 0.),
                                                     logicalWorld,
                                                     "physWorld",
                                                     0,
                                                     false,
                                                     0,
                                                     true);
    
    return physWorld;   // <- Mother Volume;
}