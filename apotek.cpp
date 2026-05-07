#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
using namespace std;

struct Obat {
    int     id;
    string  nama;
    string  kategori; // jenis obat (batuk, demam, dll)
    string  bentuk; // (tablet, sirup, dll)
    float   harga;
};

struct Node { // utk linked list ganda
    Obat    data; // data obat yg disimpan
    Node*   prev;
    Node*   next;
};

Node* head = nullptr;
Node* tail = nullptr;
int   idCounter = 1; // kasih id baru otomatis
bool  isSorted = false;  // menandai data udh diurutkan

void clearScreen() {
    system("cls");
}

string toLowerCase(string str) {
    for (int i = 0; i < str.length(); i++) {
        if (str[i] >= 'A' && str[i] <= 'Z') {
            str[i] = str[i] + 32; // pindah dari huruf besar ke kecil
        }
    }
    return str;
}

bool isSameString(string a, string b) {
    return toLowerCase(a) == toLowerCase(b);
}

bool isNamaObatExist(string nama) {
    Node* bantu = head;
    while (bantu != nullptr) {
        if (isSameString(bantu->data.nama, nama)) {
            return true;
        }
        bantu = bantu->next;
    }
    return false;
}

Node* buatNode(Obat o) {
    Node* n = new Node;
    n->data = o;
    n->prev = nullptr;
    n->next = nullptr;
    return n;
}

void saveDataToFile() {
    ofstream file("data_obat.txt"); // buka file buat nulis
    if (!file.is_open()) {
        cout << "\n  [!] Gagal menyimpan data!\n";
        return;
    }
    
    // hitung ada brp data
    Node* bantu = head;
    int count = 0;
    while (bantu != nullptr) {
        count++;
        bantu = bantu->next;
    }
    
    file << count << endl;
    bantu = head;
    while (bantu != nullptr) {
        file << bantu->data.id << endl;
        file << bantu->data.nama << endl;
        file << bantu->data.kategori << endl;
        file << bantu->data.bentuk << endl;
        file << bantu->data.harga << endl;
        bantu = bantu->next;
    }
    file.close();
}

void loadDataFromFile() { // dijalankan pas program mulai 
    ifstream file("data_obat.txt");
    if (!file.is_open()) {
        cout << "\n  [!] Belum ada file data. Mulai dengan data kosong.\n";
        return;
    }
    
    // baca jumlah data dari baris pertama
    int jumlahData;
    file >> jumlahData;
    file.ignore();
    
    // bersihin data yg ada di memori dulu
    Node* bantu = head;
    while (bantu != nullptr) {
        Node* temp = bantu->next;
        delete bantu;
        bantu = temp;
    }
    head = nullptr;
    tail = nullptr;
    idCounter = 1;
    isSorted = false;
    
    for (int i = 0; i < jumlahData; i++) {
        Obat o;
        file >> o.id;
        file.ignore();
        getline(file, o.nama);
        getline(file, o.kategori);
        getline(file, o.bentuk);
        file >> o.harga;
        file.ignore();
        
        // buat node baru & sambung ke linked list
        Node* n = buatNode(o);
        
        if (head == nullptr) {
            head = tail = n;
        } else {
            tail->next = n;
            n->prev = tail;
            tail = n;
        }
        
        // update ID counter utk data berikutnya
        if (o.id >= idCounter) {
            idCounter = o.id + 1;
        }
    }
    file.close();
    
    if (jumlahData > 0) {
        cout << "\n  [OK] Berhasil memuat " << jumlahData << " data obat!\n";
    }
}

// FUNGSI CRUD (Create, Read, Update, Delete)

void tambahObat(Obat o) {
    // cek apa nama obat udh ada
    if (isNamaObatExist(o.nama)) {
        cout << "\n  [!] Obat \"" << o.nama << "\" sudah ada!\n";
        return;
    }
    
    o.id = idCounter++; // kasih ID otomatis
    Node* n = buatNode(o); 
    
    // klo masih kosong, jd data pertama
    if (head == nullptr) {
        head = tail = n;
    } else {
        // klo udh ada data, tambah di blkg
        tail->next = n;
        n->prev = tail;
        tail = n;
    }
    
    cout << "\n  [OK] Obat \"" << o.nama << "\" berhasil ditambahkan!\n";
    isSorted = false;  // slth tambah data, data jd blm terurut
    saveDataToFile();
}

// hapus berdasarkan nama
void hapusObat(string nama) {
    if (head == nullptr) {
        cout << "\n  [!] Tidak ada data!\n";
        return;
    }
    
    // cari node yg mau dihapus
    Node* bantu = head;
    while (bantu != nullptr && !isSameString(bantu->data.nama, nama)) {
        bantu = bantu->next;
    }
    
    if (bantu == nullptr) {
        cout << "\n  [!] Obat \"" << nama << "\" tidak ditemukan!\n";
        return;
    }

    // putusin sambungan node dari linked list
    // case 1: bukan node pertama (ada node sblmnya)  
    if (bantu->prev != nullptr) {
        bantu->prev->next = bantu->next;
    } else {
        // case 2: node pertama yg dihapus
        head = bantu->next;
    }
    
    // case 3: bkn node terakhir (ada node sesudahnya)
    if (bantu->next != nullptr) {
        bantu->next->prev = bantu->prev;
    } else {
        // case 4: node terakhir yg dihapus
        tail = bantu->prev;
    }
    
    cout << "\n  [OK] Obat \"" << bantu->data.nama << "\" berhasil dihapus!\n";
    delete bantu;
    isSorted = false;  // stlh hapus data, data jd blm terurut
    saveDataToFile();
}

void hapusSemuaData() {
    if (head == nullptr) {
        cout << "\n  [!] Data sudah kosong!\n";
        return;
    }
    
    char yakin;
    cout << "\n  +------------------------------------------+\n";
    cout << "  |         HAPUS SEMUA DATA                 |\n";
    cout << "  +------------------------------------------+\n";
    cout << "  [!] PERINGATAN: Semua data akan hilang!\n";
    cout << "  Apakah Anda yakin? (y/t): ";
    cin >> yakin;
    
    if (yakin == 'y' || yakin == 'Y') {
        Node* bantu = head;
        while (bantu != nullptr) {
            Node* temp = bantu->next;
            delete bantu;
            bantu = temp;
        }
        head = nullptr;
        tail = nullptr;
        idCounter = 1;
        isSorted = false;
        
        // kosongin file 
        ofstream file("data_obat.txt", ios::trunc);
        file.close();
        
        cout << "\n  [OK] Semua data berhasil dihapus!\n";
    } else {
        cout << "\n  [OK] Penghapusan dibatalkan.\n";
    }
}

// pindah data dari linked list ke array biasa (biar gampang diurutin)
int listKeArray(Obat arr[], int maxSize) {
    int n = 0;
    Node* bantu = head;
    while (bantu != nullptr && n < maxSize) {
        arr[n] = bantu->data;
        n++;
        bantu = bantu->next;
    }
    return n; // kembaliin jumlah data yg dipindah
}

// update linked list dari array yg udh diurutin
void updateLinkedListFromArray(Obat arr[], int n) {
    Node* bantu = head;
    for (int i = 0; i < n && bantu != nullptr; i++) {
        bantu->data = arr[i];
        bantu = bantu->next;
    }
    saveDataToFile();
}

void tampilkanSemua() {
    if (head == nullptr) { 
        cout << "\n  +------------------------------------------+\n";
        cout << "  |         DAFTAR OBAT KOSONG               |\n";
        cout << "  +------------------------------------------+\n";
        return; 
    }
    
    cout << "\n";
    cout << "  +---------------------------------------------------------------------------------+\n";
    cout << "  | ID | Nama Obat                 | Kategori          | Bentuk      | Harga (Rp)  |\n";
    cout << "  +---------------------------------------------------------------------------------+\n";
    
    Node* bantu = head;
    while (bantu != nullptr) {
        cout << "  | " 
             << left << setw(3) << bantu->data.id << "| "
             << setw(25) << bantu->data.nama << "| "
             << setw(18) << bantu->data.kategori << "| "
             << setw(11) << bantu->data.bentuk << "| "
             << setw(11) << bantu->data.harga << " |\n";
        bantu = bantu->next;
    }
    cout << "  +---------------------------------------------------------------------------------+\n";
    
    // tampilkan status sorting (urut/blm)
    if (isSorted) {
        cout << "\n  [INFO] Data saat ini sudah TERURUT (A-Z)\n";
    } else {
        cout << "\n  [INFO] Data saat ini BELUM TERURUT\n";
    }
}

// buat tampilin 1 baris data obat di tabel 
void cetakObat(Obat o) {
    cout << "  | " 
         << left << setw(3) << o.id << "| "
         << setw(25) << o.nama << "| "
         << setw(18) << o.kategori << "| "
         << setw(11) << o.bentuk << "| "
         << setw(11) << o.harga << " |\n";
}

void headerTabel() {
    cout << "\n  +---------------------------------------------------------------------------------+\n";
    cout << "  | ID | Nama Obat                 | Kategori          | Bentuk      | Harga (Rp)  |\n";
    cout << "  +---------------------------------------------------------------------------------+\n";
}

void footerTabel() {
    cout << "  +---------------------------------------------------------------------------------+\n";
}

// SORTING 
// urutin dari A-Z
void bubbleSortAscending(Obat arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - 1 - i; j++) {
            // bandingin nama obat
            if (toLowerCase(arr[j].nama) > toLowerCase(arr[j+1].nama)) {
                // tuker posisi
                Obat temp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = temp;
            }
        }
    }
}

// urutin dari Z-A
void quickSortDescending(Obat arr[], int kiri, int kanan) {
    if (kiri >= kanan) return;
    
    int i = kiri, j = kanan;
    string pivot = toLowerCase(arr[(kiri + kanan) / 2].nama);
    
    while (i <= j) {
        // cari yg lebih besar dari pivot di kiri
        while (toLowerCase(arr[i].nama) > pivot) i++;
        // cari yg lebih kecil dari pivot di kanan
        while (toLowerCase(arr[j].nama) < pivot) j--;
        // tuker posisi kalo perlu
        if (i <= j) {
            Obat temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
            i++;
            j--;
        }
    }
    
    // rekursif utk bagian kiri & kanan
    if (kiri < j) quickSortDescending(arr, kiri, j);
    if (i < kanan) quickSortDescending(arr, i, kanan);
}

void menuSorting() {
    const int MAX = 200;
    Obat arr[MAX];
    int n = listKeArray(arr, MAX);
    
    if (n == 0) { 
        cout << "\n  +------------------------------------------+\n";
        cout << "  |         TIDAK ADA OBAT                   |\n";
        cout << "  +------------------------------------------+\n";
        return; 
    }

    int pilih;
    cout << "\n";
    cout << "  +=====================================+\n";
    cout << "  |        SORTING NAMA OBAT            |\n";
    cout << "  +=====================================+\n";
    cout << "  |  1. Bubble Sort (Ascending A->Z)   |\n";
    cout << "  |  2. Quick Sort  (Descending Z->A)  |\n";
    cout << "  +=====================================+\n";
    cout << "  Pilihan: ";
    cin >> pilih;

    if (pilih == 1) {
        bubbleSortAscending(arr, n);
        updateLinkedListFromArray(arr, n);
        isSorted = true;  // data urut A-Z
        cout << "\n  >> Hasil Bubble Sort (A -> Z):\n";
        headerTabel();
        for (int i = 0; i < n; i++) {
            cetakObat(arr[i]);
        }
        footerTabel();
        cout << "\n  [OK] Data obat telah diurutkan secara ascending (A-Z)!\n";
        
    } else if (pilih == 2) {
        quickSortDescending(arr, 0, n - 1);
        updateLinkedListFromArray(arr, n);
        isSorted = false;  // data urut Z-A
        cout << "\n  >> Hasil Quick Sort (Z -> A):\n";
        headerTabel();
        for (int i = 0; i < n; i++) {
            cetakObat(arr[i]);
        }
        footerTabel();
        cout << "\n  [OK] Data obat telah diurutkan secara descending (Z-A)!\n";
        cout << "  [!] PERINGATAN: Binary Search hanya bisa digunakan jika data terurut A-Z!\n";
        
    } else {
        cout << "\n  [!] Pilihan tidak valid!\n";
    }
}

// SEARCHING
// 1. SEQUENTIAL DENGAN SENTINEL (berdasarkan Kategori)
void sequentialSentinelKategori(string target) {
    const int MAX = 200;
    Obat arr[MAX];
    int n = listKeArray(arr, MAX);
    
    if (n == 0) { 
        cout << "\n  +------------------------------------------+\n";
        cout << "  |         TIDAK ADA OBAT                   |\n";
        cout << "  +------------------------------------------+\n";
        return; 
    }

    string targetLower = toLowerCase(target);
    
    // simpan data terakhir (bakal ditimpa sentinel)
    Obat backup = arr[n-1];
    
    // pasang sentinel di data terakhir (biar loop berhenti)
    arr[n-1].kategori = target;
    
    // cari dari awal sampe ketemu sentinel
    int i = 0;
    while (toLowerCase(arr[i].kategori) != targetLower) {
        i++;
    }
    
    // kembalikan data asli yg tadi ditimpa
    arr[n-1] = backup;
    
    // cek apakah ketemu atau cuma sentinel
    if (i == n-1 && toLowerCase(backup.kategori) != targetLower) {
        cout << "\n  [!] Data tidak ditemukan!\n";
        cout << "  [!] Tidak ada obat dengan kategori \"" << target << "\"!\n";
        return;
    }
    
    // klo ketemu, tampilin semua yg sama kategorinya
    cout << "\n  >> Hasil Pencarian (Sequential + Sentinel) - Kategori: " << target << "\n";
    headerTabel();
    int count = 0;
    for (int j = 0; j < n; j++) {
        if (toLowerCase(arr[j].kategori) == targetLower) {
            cetakObat(arr[j]);
            count++;
        }
    }
    footerTabel();
    cout << "  Ditemukan " << count << " data.\n";
}

// 2. SEQUENTIAL TANPA SENTINEL (berdasarkan Bentuk)
// tanpa sentinel
void sequentialTanpaSentinelBentuk(string target) {
    const int MAX = 200;
    Obat arr[MAX];
    int n = listKeArray(arr, MAX);
    
    if (n == 0) { 
        cout << "\n  +------------------------------------------+\n";
        cout << "  |         TIDAK ADA OBAT                   |\n";
        cout << "  +------------------------------------------+\n";
        return; 
    }

    string targetLower = toLowerCase(target);
    bool found = false;
    int count = 0;
    
    cout << "\n  >> Hasil Pencarian (Sequential tanpa Sentinel) - Bentuk: " << target << "\n";
    headerTabel();
    
    for (int i = 0; i < n; i++) {
        if (toLowerCase(arr[i].bentuk) == targetLower) {
            cetakObat(arr[i]);
            found = true;
            count++;
        }
    }
    
    if (!found) {
        cout << "\n  [!] Data tidak ditemukan!\n";
        cout << "  [!] Tidak ada obat dengan bentuk \"" << target << "\"!\n";
    } else {
        footerTabel();
        cout << "  Ditemukan " << count << " data.\n";
    }
}

// 3. BINARY SEARCH (berdasarkan Nama Obat) - HARUS SUDAH DIURUTKAN DULU
void binarySearchNama(string target) {
    // cek apakah data sudah diurutkan dari menu sorting
    if (!isSorted) {
        cout << "\n  +====================================================+\n";
        cout << "  |  [!] DATA BELUM DIURUTKAN!                          |\n";
        cout << "  |  Binary Search membutuhkan data yang sudah terurut  |\n";
        cout << "  |  berdasarkan Nama Obat (A-Z).                       |\n";
        cout << "  +====================================================+\n";
        cout << "\n  Silakan urutkan data terlebih dahulu dari MENU SORTING (No.5)\n";
        cout << "  Pilih Bubble Sort (Ascending A-Z) untuk mengurutkan data.\n";
        return;
    }
    
    const int MAX = 200;
    Obat arr[MAX];
    int n = listKeArray(arr, MAX);
    
    if (n == 0) { 
        cout << "\n  +------------------------------------------+\n";
        cout << "  |         TIDAK ADA OBAT                   |\n";
        cout << "  +------------------------------------------+\n";
        return; 
    }
    
    string targetLower = toLowerCase(target);
    int kiri = 0, kanan = n - 1;
    int posisi = -1; // -1 artinya blm ketemu
    
    // proses binary search
    while (kiri <= kanan) {
        int tengah = (kiri + kanan) / 2;
        string namaTengah = toLowerCase(arr[tengah].nama);
        
        if (namaTengah == targetLower) {
            posisi = tengah;
            break; // ketemu langsung berhenti
        } else if (namaTengah < targetLower) {
            kiri = tengah + 1; // cari di sebelah kanan
        } else {
            kanan = tengah - 1; // cari di sebelah kiri
        }
    }
    
    // tampilin hasil
    if (posisi == -1) {
        cout << "\n  [!] Data tidak ditemukan!\n";
        cout << "  [!] Obat \"" << target << "\" tidak ditemukan!\n";
    } else {
        cout << "\n  >> Hasil Binary Search (Nama: " << target << ")\n";
        headerTabel();
        cetakObat(arr[posisi]);
        footerTabel();
    }
}

void menuSearching() {
    int pilih;
    cout << "\n";
    cout << "  +===================================================+\n";
    cout << "  |                SEARCHING OBAT                    |\n";
    cout << "  +===================================================+\n";
    cout << "  |  1. Sequential + Sentinel (berdasarkan Kategori) |\n";
    cout << "  |  2. Sequential tanpa Sentinel (berdasarkan Bentuk)|\n";
    cout << "  |  3. Binary Search (berdasarkan Nama Obat)        |\n";
    cout << "  +===================================================+\n";
    cout << "  Pilihan: ";
    cin >> pilih;
    cin.ignore();

    if (pilih == 1) {
        string kat;
        cout << "  Masukkan kategori (contoh: batuk, demam, antibiotik): ";
        getline(cin, kat);
        if (kat == "") {
            cout << "\n  [!] Input tidak boleh kosong!\n";
            return;
        }
        sequentialSentinelKategori(kat);

    } else if (pilih == 2) {
        string bentuk;
        cout << "  Masukkan bentuk obat (contoh: cair, padat, inhaler): ";
        getline(cin, bentuk);
        if (bentuk == "") {
            cout << "\n  [!] Input tidak boleh kosong!\n";
            return;
        }
        sequentialTanpaSentinelBentuk(bentuk);

    } else if (pilih == 3) {
        string nama;
        cout << "  Masukkan nama obat: ";
        getline(cin, nama);
        if (nama == "") {
            cout << "\n  [!] Input tidak boleh kosong!\n";
            return;
        }
        binarySearchNama(nama);

    } else {
        cout << "\n  [!] Pilihan tidak valid!\n";
    }
}

// input data dari user
void menuTambah() {
    Obat o;
    char tambahLagi;
    
    do {
        cin.ignore();
        cout << "\n";
        cout << "  +------------------------------------------+\n";
        cout << "  |            TAMBAH OBAT BARU             |\n";
        cout << "  +------------------------------------------+\n";
        cout << "  Nama Obat     : ";
        getline(cin, o.nama);
        
        // cek apa nama udah dipake
        if (isNamaObatExist(o.nama)) {
            cout << "\n  [!] Obat dengan nama ini sudah ada!\n";
            cout << "\n  Apakah ingin tambah obat lagi? (y/t): ";
            cin >> tambahLagi;
            continue;
        }
        
        cout << "  Kategori      : ";
        getline(cin, o.kategori);
        cout << "  Bentuk Obat   : ";
        getline(cin, o.bentuk);
        cout << "  Harga (Rp)    : ";
        cin >> o.harga;
        
        // harga gaboleh minus atau 0
        if (o.harga <= 0) {
            cout << "\n  [!] Harga harus lebih dari 0!\n";
            cout << "  Apakah ingin input ulang? (y/t): ";
            cin >> tambahLagi;
            if (tambahLagi == 'y' || tambahLagi == 'Y') {
                continue;
            } else {
                break;
            }
        }
        
        tambahObat(o);
        
        cout << "\n  Apakah ingin tambah obat lagi? (y/t): ";
        cin >> tambahLagi;
        
    } while (tambahLagi == 'y' || tambahLagi == 'Y');
}

void menuHapus() {
    if (head == nullptr) {
        cout << "\n  +------------------------------------------+\n";
        cout << "  |         TIDAK ADA OBAT                   |\n";
        cout << "  +------------------------------------------+\n";
        return;
    }
    
    string nama;
    tampilkanSemua();
    cout << "\n  +------------------------------------------+\n";
    cout << "  |            HAPUS OBAT                    |\n";
    cout << "  +------------------------------------------+\n";
    cout << "  Masukkan NAMA obat yang akan dihapus: ";
    cin.ignore();
    getline(cin, nama);
    
    hapusObat(nama);
}

int main() {
    clearScreen();
    
    cout << "\n  +=====================================+\n";
    cout << "  |    SISTEM MANAJEMEN APOTEK           |\n";
    cout << "  |    DOUBLE LINKED LIST + HEAD & TAIL  |\n";
    cout << "  +=====================================+\n";
    loadDataFromFile(); // baca data dari file
    
    int pilih;
    do {
        cout << "\n";
        cout << "  +=====================================+\n";
        cout << "  |  1. Tampilkan Semua Obat             |\n";
        cout << "  |  2. Tambah Obat                      |\n";
        cout << "  |  3. Hapus Obat (by NAMA)             |\n";
        cout << "  |  4. Hapus Semua Data                 |\n";
        cout << "  |  5. Sorting Nama Obat                |\n";
        cout << "  |  6. Searching Obat                   |\n";
        cout << "  |  0. Keluar                           |\n";
        cout << "  +=====================================+\n";
        cout << "  Pilihan: ";
        cin >> pilih;

        switch (pilih) {
            case 1: 
                clearScreen();
                tampilkanSemua();
                break;
            case 2: 
                clearScreen();
                menuTambah();
                break;
            case 3: 
                clearScreen();
                menuHapus();
                break;
            case 4:
                clearScreen();
                hapusSemuaData();
                break;
            case 5: 
                clearScreen();
                menuSorting();
                break;
            case 6: 
                clearScreen();
                menuSearching();
                break;
            case 0: 
                clearScreen();
                cout << "\n  +=====================================+\n";
                cout << "  |         TERIMA KASIH!                |\n";
                cout << "  +=====================================+\n";
                break;
            default: 
                cout << "\n  [!] Pilihan tidak valid!\n";
        }
    } while (pilih != 0);

    // bersihin memori sebelum keluar 
    Node* bantu = head;
    while (bantu != nullptr) {
        Node* temp = bantu->next;
        delete bantu;
        bantu = temp;
    }

    return 0;
}