#include "generator.hh"

MyPrimaryGenerator::MyPrimaryGenerator() {
    fParticleGun = new G4ParticleGun(1);

    G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();
    G4String particleName = "mu-";
    G4ParticleDefinition *particle = particleTable -> FindParticle("mu-");

    // G4ThreeVector pos(0., 0., 0.);
    // G4ThreeVector mom(0., 0., 1.);
    G4ThreeVector pos(10*cm, 0., 5.*cm);
    G4ThreeVector mom(-1., 0., 0.);

    fParticleGun -> SetParticlePosition(pos);
    fParticleGun -> SetParticleMomentumDirection(mom);
    fParticleGun -> SetParticleMomentum(4.*GeV);
    fParticleGun -> SetParticleDefinition(particle);
}

MyPrimaryGenerator::~MyPrimaryGenerator() {
    delete fParticleGun;
}

void MyPrimaryGenerator::GeneratePrimaries(G4Event *anEvent) {
    fParticleGun -> GeneratePrimaryVertex(anEvent);
}