#include <iostream>

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4VisManager.hh"
#include "G4UIExecutive.hh"

#include "construction.hh"
#include "myPhysicsList.hh"
#include "action.hh"

int main(int argc, char** argv) {
    G4RunManager *runManager = new G4RunManager();

    runManager -> SetUserInitialization(new MyDetectorConstruction());
    runManager -> SetUserInitialization(new MyPhysicsList());
    runManager -> SetUserInitialization(new MyActionInitialization());

    runManager -> Initialize();

    G4UIExecutive *ui = 0;

    if (argc == 1) {
        ui = new G4UIExecutive(argc, argv);
    }

    G4VisManager *visManager = new G4VisExecutive();
    visManager -> Initialize();

    G4UImanager *UImanager = G4UImanager::GetUIpointer();

    if (ui) {
    // UImanager -> ApplyCommand("/vis/viewer/set/numberOfCloudPoints 50000");
    // UImanager -> ApplyCommand("/vis/scene/activateModel G4PhysicalVolumeModel false");
    // UImanager -> ApplyCommand("/vis/viewer/set/depth 1");
        UImanager -> ApplyCommand("/control/execute vis.mac");

        ui -> SessionStart();
    } else {
        G4String command = "/control/execute ";
        G4String fileName = argv[1];
        UImanager -> ApplyCommand(command + fileName);
    }
                
    // --- CLEANUP START ---
    delete ui;          // Delete UI first
    delete visManager;  // Delete VisManager second
    delete runManager;  // Delete RunManager last - this triggers geometry cleanup
    // --- CLEANUP END ---

    return 0;
}