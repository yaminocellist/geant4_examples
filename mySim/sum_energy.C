void sum_energy(int fileIndex = 1) {
    TString filename = Form("output%d.root", fileIndex);
    TFile *file = TFile::Open(filename);
    TTree *tree = (TTree*)file->Get("Hits");

    tree->Draw("fEnergy"); // fill internal buffer
    double totalEnergy = 0;
    for (int i = 0; i < tree->GetSelectedRows(); ++i) {
        totalEnergy += tree->GetV1()[i];
    }

    std::cout << "Total energy deposited: " << totalEnergy << " eV" << std::endl;
}
