#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <random>
#include <iterator>
#include <algorithm>

using namespace std;

int satirSayisiF(const string& dosyaAdi) {
    ifstream dosya(dosyaAdi);
    if (!dosya.is_open()) {
        cout << "Dosya acilamadi." << endl;
        return 0;
    }

    string satir;
    int sayac = 0;

    while (getline(dosya, satir)) {
        sayac++;
    }

    dosya.close();

    return sayac;
}

void readFirstLine(const string& fileName, int& firstInt, int& secondInt) {
    ifstream file(fileName);
    if (!file.is_open()) {
        cout << "Dosya acilamadi." << endl;
        return;
    }

    string line;
    if (getline(file, line)) {
        istringstream iss(line);
        if (!(iss >> firstInt >> secondInt)) {
            cout << "Ilk satirda gecerli iki tamsayi bulunamadi." << endl;
            return;
        }
    } else {
        cout << "Dosya bos." << endl;
        return;
    }

    file.close();
}

void depoVerileri(const string& dosyaAdi, int depoKapasitesi[], float depoMaliyet[], int limit, int baslangicSatiri) {
    ifstream dosya(dosyaAdi);
    if (!dosya.is_open()) {
        cout << "Dosya acilamadi." << endl;
        return;
    }

    string satir;
    int index = 0;
    int satirSayaci = 1;
    while (getline(dosya, satir) && index < limit) {
        if (satirSayaci >= baslangicSatiri) {
            istringstream iss(satir);
            int kapasite;
            float maliyet;
            if (iss >> kapasite >> maliyet) {
                depoKapasitesi[index] = kapasite;
                depoMaliyet[index] = maliyet;
                index++;
            }
        }
        satirSayaci++;
    }

    dosya.close();
}

void musteriTalepleriF(const string& dosyaAdi, int musteriTalep[], int limit, int baslangicSatiri) {
    ifstream dosya(dosyaAdi);
    if (!dosya.is_open()) {
        cout << "Dosya acilamadi." << endl;
        return;
    }

    string satir;
    int index = 0;
    int satirSayaci = 1;
    while (getline(dosya, satir) && index < limit) {
        if (satirSayaci >= baslangicSatiri) {
            if(satirSayaci % 2 == 0) {
                istringstream iss(satir);
                int talep;
                if (iss >> talep) {
                    musteriTalep[index] = talep;
                    index++;
                }
            }
        }
        satirSayaci++;
    }

    dosya.close();
}

void depoMaliyetF(const string& dosyaAdi, float depoMaliyet[], int limit, int baslangicSatiri, int depoSayisi) {
    ifstream dosya(dosyaAdi);
    if (!dosya.is_open()) {
        cout << "Dosya açılamadı." << endl;
        return;
    }

    string satir;
    int index = 0;
    int satirSayaci = 1;
    while (getline(dosya, satir) && index < limit) {
        if (satirSayaci >= baslangicSatiri) {
            if(satirSayaci % 2 == 1) {
                istringstream iss(satir);
                for (int i = 0; i < depoSayisi; i++) {
                    if (!(iss >> depoMaliyet[index * depoSayisi + i])) {
                        cout << "Hata: Dosyadan okuma hatasi!" << endl;
                        dosya.close();
                        return;
                    }
                }
                index++;
            }
        }
        satirSayaci++;
    }

    dosya.close();
}


float calculateTotalCost(const vector<int>& assignedDepots, const float* depoMaliyetleri, const int* musteriTalepleri, int depoSayisi, int musteriSayisi) {
    float totalCost = 0.0;

    for (int i = 0; i < musteriSayisi; i++) {
        int assignedDepotIndex = assignedDepots[i];
        totalCost += depoMaliyetleri[assignedDepotIndex * musteriSayisi + i];
    }

    for (int i = 0; i < depoSayisi; i++) {
        float depotCapacity = 0.0;
        for (int j = 0; j < musteriSayisi; j++) {
            if (assignedDepots[j] == i)
                depotCapacity += musteriTalepleri[j];
        }

        if (depotCapacity > 0)
            totalCost += depotCapacity * depoMaliyetleri[i * musteriSayisi + musteriSayisi];
    }

    return totalCost;
}

void printResult(const vector<int>& assignedDepots, const float* depoMaliyetleri, const int* musteriTalepleri, int depoSayisi, int musteriSayisi) {
    cout << fixed << setprecision(2);
    cout << "\nToplam Maliyet: " << calculateTotalCost(assignedDepots, depoMaliyetleri, musteriTalepleri, depoSayisi, musteriSayisi) << endl;
    cout << "Musterilere Atanan Depolar: \n";
    for (int i = 0; i < assignedDepots.size(); i++) {
        cout << assignedDepots[i] + 1 << " ";
    }
    cout << endl;
}



vector<int> optimizeWLP(const float* depoMaliyetleri, const int* musteriTalepleri, int depoSayisi, int musteriSayisi, const int* originalDepoKapasiteleri) {
    vector<int> assignedDepots(musteriSayisi, -1);

    int depoKapasiteleri[depoSayisi];
    memcpy(depoKapasiteleri, originalDepoKapasiteleri, depoSayisi * sizeof(int));

    random_device rd;
    mt19937 gen(rd());

    for (int customer = 0; customer < musteriSayisi; ++customer) {
        vector<int> feasibleDepots;

        for (int depot = 0; depot < depoSayisi; ++depot) {
            if (depoKapasiteleri[depot] >= musteriTalepleri[customer]) {
                feasibleDepots.push_back(depot);
            }
        }

        if (!feasibleDepots.empty()) {
            uniform_int_distribution<> dis(0, feasibleDepots.size() - 1);
            int bestDepot = feasibleDepots[dis(gen)];
            assignedDepots[customer] = bestDepot;
            depoKapasiteleri[bestDepot] -= musteriTalepleri[customer];
        }
    }

    return assignedDepots;
}

int main() {
    string dosyaAdi = "16.txt";
    int depoSayisi, musteriSayisi;
    int satirSayisi = satirSayisiF(dosyaAdi);

    readFirstLine(dosyaAdi, depoSayisi, musteriSayisi);
    cout << "Depo Sayisi: " << depoSayisi;
    cout << "\nMusteri Sayisi: " << musteriSayisi;

    int depoKapasiteleri[depoSayisi];
    float depoMaliyetleri[depoSayisi];
    depoVerileri(dosyaAdi, depoKapasiteleri, depoMaliyetleri, depoSayisi, 2);

    //cout << "\n\nDepo Kapasiteleri ve Maliyetleri:" << endl;
    /*for (int i = 0; i < depoSayisi; i++) {
        cout << "Depo " << i + 1 << " - Kapasite: " << depoKapasiteleri[i] << ", Maliyet: " << fixed << setprecision(1) << depoMaliyetleri[i] << endl;
    }*/

    //cout << "\nMusteri Talepleri\n";
    int musteriTalepleri[musteriSayisi];
    musteriTalepleriF(dosyaAdi, musteriTalepleri, satirSayisi, depoSayisi+2);
    /*for(int j=0; j<musteriSayisi; j++) {
        cout << "Musteri " << j+1 << " - Talep: " << musteriTalepleri[j] << endl;
    }*/

    //cout << "\nMusterinin Depo Maliyetleri\n";
    float depoMaliyet[musteriSayisi * depoSayisi];
    depoMaliyetF(dosyaAdi, depoMaliyet, satirSayisi, depoSayisi+3, depoSayisi);
    /*int index = 0;
    for(int j=0; j<musteriSayisi; j++) {
        for(int k=0; k<depoSayisi; k++) {
            cout << "Musteri " << j+1 << " - Depo " << musteriTalepleri[j] << " - Maliyet: " << fixed << setprecision(5) << depoMaliyet[index] << endl;
            index++;
        }
    }*/

    vector<int> assignedDepots = optimizeWLP(depoMaliyet, musteriTalepleri, depoSayisi, musteriSayisi, depoKapasiteleri);
    printResult(assignedDepots, depoMaliyet, musteriTalepleri, depoSayisi, musteriSayisi);
    return 0;
}
