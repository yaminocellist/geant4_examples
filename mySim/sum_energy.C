void sum_energy() {
    TFile *file = TFile::Open("output0.root");
    TTree *tree = (TTree*)file->Get("Hits");

    tree->Draw("fEnergy", "", "goff"); // fill internal buffer
    double totalEnergy = 0;
    for (int i = 0; i < tree->GetSelectedRows(); ++i) {
        totalEnergy += tree->GetV1()[i];
    }

    std::cout << "Total energy deposited: " << totalEnergy << " eV" << std::endl;
}
