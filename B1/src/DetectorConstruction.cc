//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file B1/src/DetectorConstruction.cc
/// \brief Implementation of the B1::DetectorConstruction class

#include "DetectorConstruction.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4CutTubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"

namespace B1
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // Get nist material manager
  G4NistManager* nist = G4NistManager::Instance();

  // Envelope parameters
  //
  G4double env_sizeXY = 20*cm, env_sizeZ = 30*cm;
  G4Material* env_mat = nist->FindOrBuildMaterial("G4_AIR");

  // Option to switch on/off checking of volumes overlaps
  //
  G4bool checkOverlaps = true;

  //
  // World
  //
  G4double world_sizeXY = 1.2*env_sizeXY;
  G4double world_sizeZ  = 1.2*env_sizeZ;
  G4Material* world_mat = nist->FindOrBuildMaterial("G4_AIR");

  auto solidWorld = new G4Box("World",                           // its name
    0.5 * world_sizeXY, 0.5 * world_sizeXY, 0.5 * world_sizeZ);  // its size

  auto logicWorld = new G4LogicalVolume(solidWorld,  // its solid
    world_mat,                                       // its material
    "World");                                        // its name

  auto physWorld = new G4PVPlacement(nullptr,  // no rotation
    G4ThreeVector(),                           // at (0,0,0)
    logicWorld,                                // its logical volume
    "World",                                   // its name
    nullptr,                                   // its mother  volume
    false,                                     // no boolean operation
    0,                                         // copy number
    checkOverlaps);                            // overlaps checking

  //
  // Envelope
  //
  auto solidEnv = new G4Box("Envelope",                    // its name
    1.0 * env_sizeXY, 1.0 * env_sizeXY, 1.0 * env_sizeZ);  // its size

  auto logicEnv = new G4LogicalVolume(solidEnv,  // its solid
    env_mat,                                     // its material
    "Envelope");                                 // its name

  new G4PVPlacement(nullptr,  // no rotation
    G4ThreeVector(),          // at (0,0,0)
    logicEnv,                 // its logical volume
    "Envelope",               // its name
    logicWorld,               // its mother  volume
    false,                    // no boolean operation
    0,                        // copy number
    checkOverlaps);           // overlaps checking

// //
// // Shape 3
// //
// G4Material* shape3_mat = nist->FindOrBuildMaterial("G4_A-150_TISSUE");

// // Positioning within the envelope
// G4ThreeVector pos3 = G4ThreeVector(0, 0, 0*cm);

// // Cylindrical section shape with cut planes
// G4double shape3_rmin = 8.*cm, shape3_rmax = 8.1*cm;
// G4double shape3_hz = 15.*cm;  // Half-length
// G4double shape3_phimin = -90.*deg, shape3_phimax = 180.*deg;

// G4ThreeVector cut1Normal(0, 0., -3);
// G4ThreeVector cut2Normal(0, 0, 3); 

// auto solidShape3 = new G4CutTubs("Shape3", shape3_rmin, shape3_rmax, shape3_hz, 
//     shape3_phimin, shape3_phimax, cut1Normal, cut2Normal);
    
// auto logicShape3 = new G4LogicalVolume(solidShape3,  // its solid
//     shape3_mat,                                      // its material
//     "Shape3");                                       // its name

// G4RotationMatrix* rotX = new G4RotationMatrix();
// rotX->rotateX(0.*deg);

// new G4PVPlacement(rotX,  // no rotation
//     pos3,                   // at position
//     logicShape3,            // its logical volume
//     "Shape3",               // its name
//     logicEnv,               // its mother volume
//     false,                  // no boolean operation
//     0,                      // copy number
//     checkOverlaps);         // overlaps checking

//
// Shape 3 - Cuboid
//
G4Material* shape3_mat = nist->FindOrBuildMaterial("G4_A-150_TISSUE");

// Size
G4double shape3_x = 5.0 * cm;
G4double shape3_y = 5.0 * cm;
G4double shape3_z = 1.0 * cm;
// Half-dimensions required by G4Box
auto solidShape3 = new G4Box("Shape3", shape3_x/2, shape3_y/2, shape3_z/2);
auto logicShape3 = new G4LogicalVolume(solidShape3,
                                       shape3_mat,
                                       "Shape3");
G4ThreeVector pos3 = G4ThreeVector(0, +10.0*cm, 0);
G4RotationMatrix* rotX = new G4RotationMatrix();
rotX->rotateX(0.*deg);

new G4PVPlacement(rotX,             // rotation
                  pos3,             // position
                  logicShape3,      // logical volume
                  "Shape3",         // name
                  logicEnv,         // mother volume
                  false,            // no boolean operation
                  0,                // copy number
                  checkOverlaps);   // check overlaps

//
// Shape 2 - Cuboid
//
G4Material* shape2_mat = nist->FindOrBuildMaterial("G4_A-150_TISSUE");

// Size
G4double shape2_x = 5.0 * cm;
G4double shape2_y = 5.0 * cm;
G4double shape2_z = 1.0 * cm;
// Half-dimensions required by G4Box
auto solidShape2 = new G4Box("Shape2", shape2_x/2, shape2_y/2, shape2_z/2);
auto logicShape2 = new G4LogicalVolume(solidShape2,
                                       shape2_mat,
                                       "Shape2");
G4ThreeVector pos2 = G4ThreeVector(0, -10*cm, 0);
// G4RotationMatrix* rotX = new G4RotationMatrix();
rotX->rotateX(0.*deg);

new G4PVPlacement(rotX,             // rotation
                  pos2,             // position
                  logicShape2,      // logical volume
                  "Shape2",         // name
                  logicEnv,         // mother volume
                  false,            // no boolean operation
                  0,                // copy number
                  checkOverlaps);   // check overlaps

//
// Shape 1 - Cuboid as Lead Brick:
//
G4Material* shape1_mat = nist->FindOrBuildMaterial("G4_Pb");

// Size
G4double shape1_x = 10.0 * cm;
G4double shape1_y = 1.0 * cm;
G4double shape1_z = 30.0 * cm;
// Half-dimensions required by G4Box
auto solidShape1 = new G4Box("Shape1", shape1_x/2, shape1_y/2, shape1_z/2);
auto logicShape1 = new G4LogicalVolume(solidShape1,
                                       shape1_mat,
                                       "Shape1");
G4ThreeVector pos1 = G4ThreeVector(0, 0, +15.0*cm);
rotX->rotateX(0.*deg);

new G4PVPlacement(rotX,             // rotation
                  pos1,             // position
                  logicShape1,      // logical volume
                  "Shape1",         // name
                  logicEnv,         // mother volume
                  false,            // no boolean operation
                  0,                // copy number
                  checkOverlaps);   // check overlaps

  // Set Scoring Volume
  fScoringVolumes.insert(logicShape3);
  fScoringVolumes.insert(logicShape2);

  //
  //always return the physical World
  //
  return physWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
