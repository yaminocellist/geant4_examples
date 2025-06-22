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
#include "FTFP_BERT.hh"

namespace B1
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // Get nist material manager
  G4NistManager* nist = G4NistManager::Instance();

  // Envelope parameters
  //
  G4double env_sizeXY = 60*cm, env_sizeZ = 90*cm;
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
    0.5 * env_sizeXY, 0.5 * env_sizeXY, 0.5 * env_sizeZ);  // its size

  auto logicEnv = new G4LogicalVolume(solidEnv,   // its solid
                                      env_mat,    // its material
                                      "Envelope");// its name

  new G4PVPlacement(nullptr,  // no rotation
                    G4ThreeVector(),          // at (0,0,0)
                    logicEnv,                 // its logical volume
                    "Envelope",               // its name
                    logicWorld,               // its mother  volume
                    false,                    // no boolean operation
                    0,                        // copy number
                    checkOverlaps);           // overlaps checking

  G4Element* elH = nist->FindOrBuildElement("H");
  G4Element* elC = nist->FindOrBuildElement("C");
  G4Material* scintMat = new G4Material("MyScintillator", 1.032*g/cm3, 2);
  // BC412, made of PVT (Polyvinyl toluene):
  scintMat->AddElement(elH, 10);
  scintMat->AddElement(elC, 9);

  // Property settings for Scintillation:
  auto MPT = new G4MaterialPropertiesTable();

  const G4int nEntries = 2;
  G4double photonEnergy[nEntries]    = {1.5895*eV, 3.5*eV};
  G4double scintSpectrum[nEntries]   = {1.0,     1.0};

  MPT->AddProperty("FASTCOMPONENT", photonEnergy, scintSpectrum, nEntries, true);
  MPT->AddConstProperty("SCINTILLATIONYIELD", 10000.0 / MeV);
  MPT->AddConstProperty("RESOLUTIONSCALE", 1.0);
  MPT->AddConstProperty("FASTTIMECONSTANT", 2.1 * ns, true);
  MPT->AddConstProperty("YIELDRATIO", 1.0, true); // All light is fast component

  scintMat->SetMaterialPropertiesTable(MPT);
  
  //
  // Shape 1
  //
  G4Material* shape1_mat = nist->FindOrBuildMaterial("G4_Pb");
  G4ThreeVector pos1 = G4ThreeVector(0, 0., +15.0*cm);

  // Dimensions:
  G4double shape1_x = 10.0 * cm;
  G4double shape1_y = 1.0 * cm;
  G4double shape1_z = 30.0 * cm;
  // Half-dimensions required by G4Box:
  auto solidShape1 = new G4Box("Shape1", shape1_x/2, shape1_y/2, shape1_z/2);
  auto logicShape1 = new G4LogicalVolume(solidShape1,  // its solid
                                         shape1_mat,   // its material
                                         "Shape1");    // its name

  new G4PVPlacement(nullptr,  // no rotation
                    pos1,                     // at position
                    logicShape1,              // its logical volume
                    "Shape1",                 // its name
                    logicEnv,                 // its mother  volume
                    false,                    // no boolean operation
                    0,                        // copy number
                    checkOverlaps);           // overlaps checking

//
// Shape 3 - Cuboid
//
// G4Material* shape3_mat = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
G4Material* shape3_mat = scintMat;

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
// G4Material* shape2_mat = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
G4Material* shape2_mat = scintMat;

// Size
G4double shape2_x = 5.0 * cm;
G4double shape2_y = 5.0 * cm;
G4double shape2_z = 1.0 * cm;
// Half-dimensions required by G4Box
auto solidShape2 = new G4Box("Shape2", shape2_x/2, shape2_y/2, shape2_z/2);
auto logicShape2 = new G4LogicalVolume(solidShape2,
                                       shape2_mat,
                                       "Shape2");
G4ThreeVector pos2 = G4ThreeVector(0, -9*cm, 0);
rotX->rotateX(0.*deg);

new G4PVPlacement(rotX,             // rotation
                  pos2,             // position
                  logicShape2,      // logical volume
                  "Shape2",         // name
                  logicEnv,         // mother volume
                  false,            // no boolean operation
                  0,                // copy number
                  checkOverlaps);   // check overlaps

  // Set Scoring Volume
  fScoringVolume = logicShape2;

  //
  //always return the physical World
  //
  return physWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
