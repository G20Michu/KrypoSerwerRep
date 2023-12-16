#include <iostream>
#include <cstring>
#include <fstream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <curl/curl.h>
#include <map>
#include <vector>
#include <ctime>
#include <pthread.h>
#include <cstdlib>
#include <thread>
#include <vector>
#include <cstdio>
#include <iomanip>
using namespace std;
//const int PORT = 12345;
//const int BUFFER_SIZE = 1024;
//const char* TARGET_IP = "192.168.0.182";
bool StatusServer = true;
vector <int>  AktywnoscWalut; // 0 lub 1 index odnosci sie do Nazwywalut. stan aktywnosci watku 
vector <string> NazwyWalut;
vector <string> LinkWalut;
vector <string> _20Currencyback;
bool Upr = true; //uruchomiony pierwszy raz do konfiguracji plikow etc
//do sprawdzania watkow
//struktura danych
//[indentyfikator][kurs][data]
//[identyfikator][kurs][data]
string DATA; //chwilowa data ale to musi byc tablica
string KURS; //chwilowy kurs ale to musi byc tablica
int Send(int Port, int BUFFER_SIZE, char* TARGET_IP)
{
    int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_socket == -1) {
        std::cerr << "Błąd przy tworzeniu gniazda." << std::endl;
        return -1;
    }

    sockaddr_in server_address;
    std::memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(TARGET_IP);  // Adres IP docelowego klienta
    server_address.sin_port = htons(Port);

    const int buffer_size = BUFFER_SIZE;
    char buffer[buffer_size];

    cout << "Wprowadź wiadomość do wysłania: ";
    cin.getline(buffer, buffer_size);

    // Wysyłanie wiadomości do klienta o określonym IP
    sendto(udp_socket, buffer, strlen(buffer), 0, (struct sockaddr*)&server_address, sizeof(server_address));

    std::cout << "Wiadomość wysłana." << std::endl;

    close(udp_socket);

    return 0;
}
int ReadPort(int Port)
{
    int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_socket == -1)
    {
        std::cerr << "bład przy tworzeniu gniazda" << std::endl;
    }
    char Rmessange = ReadPort(Port);
    return(0);
}
void GetKrypto(string KryptoName, string Kryptolink)
{
    vector<string> AktualnyKurs; // nie moze byc wspolne
    vector<string> AktualnaData; //niemzoe byc wspolne

    while (StatusServer == true)
    {

        auto start = std::chrono::high_resolution_clock::now();
        // Polecenie, które chcesz wykonać
        string commands = "curl -s " + Kryptolink;
        char* command = new char[commands.size() + 1];
        strcpy(command, commands.c_str());
        // Otwórz proces za pomocą potoku
        FILE* pipe = popen(command, "r");
        if (!pipe) {
            std::cerr << "Błąd podczas otwierania potoku do polecenia systemowego.\n";
        }

        // Bufor do przechowywania danych wyjściowych
        char buffer[128];
        string  result;

        // Odczytaj dane wyjściowe z potoku w pętli
        while (!feof(pipe)) {
            if (fgets(buffer, 128, pipe) != NULL)
            {
                result += buffer;
            }
        }
        const char* convertresult = result.c_str();
        string ofresult; // ostateczny wynik
        for (int i = 0; i <= (result.size() + 1); i++)
        {
            if (isdigit(convertresult[i]) || convertresult[i] == '.')
            {
                ofresult += convertresult[i];
            }
        }
        //konwersja odcinanie 0
        const char* convertresultzero;
        convertresultzero = ofresult.c_str();
        cout << convertresultzero << endl;
        string converofresult;
        int cutindex;
        for (int i = (ofresult.size()) - 1; i >= 0; i--)
        {
            if (convertresultzero[i] != '0')
            {
                cout << "i " << i << "converresult " << convertresultzero[i] << endl;
                if (convertresultzero[i] == '.' || convertresultzero[i] == ',')
                    cutindex = (i - 1);
                else
                    cutindex = i;
                i = 0;
            }
        }
        for (int i = 0; i <= cutindex; i++)
        {
            converofresult += convertresultzero[i];
        }
        ofresult = converofresult;
        //---------- dodajemy do puli aktualnych walut

        //----------
        //
        cout << " end buffer" << endl;
        // Zamknij potok
        pclose(pipe);
        clock_t end = clock();
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
        // Wyświetl dane wyjściowe lub zrób z nimi coś innego
        std::cout << "Dane wyjściowe z polecenia:\n" << ofresult << " " << duration.count() << " ms" << std::endl;
        auto currenttime = chrono::system_clock::now();
        time_t currenttimenum = chrono::system_clock::to_time_t(currenttime);
        if (AktualnyKurs.size() < 20)
        {
            AktualnyKurs.push_back(ofresult);
            AktualnaData.push_back(to_string(currenttimenum));
        }
        else
        {
            AktualnyKurs.erase(AktualnyKurs.begin());
            AktualnaData.erase(AktualnaData.begin());
            AktualnyKurs.insert(AktualnyKurs.end(), ofresult);
            AktualnaData.insert(AktualnaData.end(), to_string(currenttimenum));
        }
        //zapisujemy waluty do pliku
        string nazwapliku = "/home/pi/projects/KryptoLinuxSerwer/Currency/Curr-20TB/" + KryptoName + ".txt";
        cout << AktualnyKurs.size() << " aktualny kurs" << endl;
        ofstream waluta(nazwapliku, ios::trunc);
        AktualnyKurs.size();
        for (int i = 0; i <= AktualnyKurs.size() - 1; i++)
        {
            waluta << AktualnyKurs[i] << "," << AktualnaData[i] << endl;
        }
        waluta.close();
        KURS = ofresult;
        DATA = to_string(currenttimenum);
        delete[] command;
        sleep(2);
    }
}
void confread()
{

    ifstream Currlink("/home/pi/projects/KryptoLinuxSerwer/Currency/conf/link.txt");
    ifstream Currname("/home/pi/projects/KryptoLinuxSerwer/Currency/conf/lCurrname.txt");
    string linia = "", linia2 = "";

    //konfiguracja wstepna!!!!
    if (Upr == true)
    {
        while (getline(Currname, linia))
        {
            AktywnoscWalut.push_back(0);
        }
        Upr = false;
    }
    ///konfiguracja wstepna!!!!

    while (getline(Currname, linia))
    {
        NazwyWalut.push_back(linia);
        getline(Currlink, linia2);
        if (AktywnoscWalut.size() != 0)
        {
            for (int i = 0; i <= AktywnoscWalut.size(); i++)
            {
                if (AktywnoscWalut[i] == 0)
                {
                    thread a(GetKrypto, linia, linia2);
                    a.join();
                    a.detach();
                    AktywnoscWalut[i] = 1;
                }
            }
        }
    }
    Currname.close();
    Currlink.close();

}
class UI
{
public:
    //script to remove line to conf file
    void remline(int line)
    {
        ifstream Currlink("/home/pi/projects/KryptoLinuxSerwer/Currency/conf/link.txt");
        ifstream Currname("/home/pi/projects/KryptoLinuxSerwer/Currency/conf/lCurrname.txt");
        string linia1, linia2, linia3, linia4;
        vector<string> linie1, linie2, linie3, linie4;
        int numerlini = 0, numerlini2 = 0;
        //popraw to bo mam nadpisywanie do jednego pliku mianowice numerlini jest taki sam dla jednego i drugiego while i linia tez
        while (getline(Currlink, linia1))
        {
            if (numerlini != line)
                linie1.push_back(linia1);

            numerlini++;
        }
        numerlini = 0;
        while (getline(Currname, linia2))
        {
            if (numerlini != line)
                linie2.push_back(linia2);

            numerlini++;
        }
        Currname.close();
        Currlink.close();
        ofstream Currlinkuppend("/home/pi/projects/KryptoLinuxSerwer/Currency/conf/link.txt");
        ofstream Currnameuppend("/home/pi/projects/KryptoLinuxSerwer/Currency/conf/lCurrname.txt");
        for (string linia3 : linie1)
        {
            Currlinkuppend << linia3 << endl;
        }
        for (string linia4 : linie2)
        {
            Currnameuppend << linia4 << endl;
        }
        Currnameuppend.close();
        Currlinkuppend.close();
    }
    void renline(int linein, string linkout, string nameout) //script to rename line //linein input index of line , linout output string line
    {
        ifstream Currlink("/home/pi/projects/KryptoLinuxSerwer/Currency/conf/link.txt");
        ifstream Currname("/home/pi/projects/KryptoLinuxSerwer/Currency/conf/lCurrname.txt");
        string linia1, linia2, linia3, linia4;
        vector<string> linie1, linie2, linie3, linie4;
        int numerlini = 0, numerlini2 = 0;
        while (getline(Currlink, linia1))
        {
            if (numerlini != linein)
                linie1.push_back(linia1);
            else
                linie1.push_back(linkout);
            numerlini++;
        }
        while (getline(Currname, linia2))
        {
            if (numerlini2 != linein)
                linie2.push_back(linia2);
            else
                linie2.push_back(nameout);
            numerlini2++;
        }
        Currname.close();
        Currlink.close();
        ofstream Currlinkuppend("/home/pi/projects/KryptoLinuxSerwer/Currency/conf/link.txt");
        ofstream Currnameuppend("/home/pi/projects/KryptoLinuxSerwer/Currency/conf/lCurrname.txt");
        for (string linia3 : linie1)
        {
            Currlinkuppend << linia3 << endl;
        }
        for (string linia4 : linie2)
        {
            Currnameuppend << linia4 << endl;
        }
        Currnameuppend.close();
        Currlinkuppend.close();
    }
    bool TurnServ = true; //turn on/off
    void Menu()
    {
        confread();
        system("clear");
        int wybor = 0;
        cout << "_____WELCOME TO KRYPTO SERWER_____" << endl;
        cout << "Wybierz 1 aby zresetować serwer" << endl;
        switch (TurnServ)
        {
        case true:
            cout << "Wybierz 2 aby wyłączyć serwer" << endl;
            break;
        case false:
            cout << "Wybierz 2 aby włączyć serwer" << endl;
            break;
        }
        cout << "Wybierz 3 aby przejść do ustawień" << endl;
        cout << endl;
        cout << "              status:  ";
        if (StatusServer == true)
            cout << "ON" << endl;
        else
            cout << "off" << endl;
        cin >> wybor;
        switch (wybor)
        {
        case 1:
            cin.clear(); // Wyczyść bity błędów
            system("clear");
            Reset();
            break;
        case 2:
            OnStopServer();
            break;
        case 3:
            cin.clear(); // Wyczyść bity błędów
            system("clear");
            Settings();
            break;
        }
    }
    void Reset()
    {
        system("clear");
        cout << "Restart..." << endl;
        sleep(5);
        system("clear");
        Menu();
    }
    void OnStopServer()
    {
        switch (TurnServ)
        {
        case true:
            cout << "Wyłączanie..." << endl;
            StatusServer = false;
            TurnServ = false;
            break;
        case false:
            cout << "Włączanie..." << endl;
            StatusServer = true;
            TurnServ = true;
            break;
        }
        sleep(5);
        system("clear");
        Menu();
    }
    void Settings()
    {
        int wybor2;
        cout << "wybierz 1 jesli chcesz dodac kryptowalute" << endl;
        cout << "wybierz 2 jesli chcesz edytowac walute" << endl;
        cout << "wybierz 3 jesli chcesz usunac walute" << endl;
        cout << "wybierz 4 jesli chcesz wyjsc do menu" << endl;
        string nazwawaluty = "", link;
        cin >> wybor2;
        switch (wybor2)
        {
        case 1:
        {
            bool dod = true;
            system("clear");

            while (dod == true)
            {
                system("clear");
                cout << "Akutalnie dodane Waluty:  ";
                for (int i = 0; i < NazwyWalut.size(); i++)
                {
                    cout << NazwyWalut[i] << " ";
                }
                cout << endl;
                cout << "podaj nazwe waluty" << endl;
                cin >> nazwawaluty;
                AktywnoscWalut.push_back(0);
                cout << "podaj link do waluty" << endl;
                cin >> link;
                NazwyWalut.push_back(nazwawaluty);
                LinkWalut.push_back(link);
                cout << endl;
                cout << "Chcesz dodać kolejną walutę ? (1-tak,0-nie)" << endl;
                cin >> dod;
                cin.clear(); // Wyczyść bity błędów
                cout << endl;
                cout << "zapisuje ustawienia..." << endl;
                ofstream Currlink("/home/pi/projects/KryptoLinuxSerwer/Currency/conf/link.txt", ios::app);
                ofstream Currname("/home/pi/projects/KryptoLinuxSerwer/Currency/conf/lCurrname.txt", ios::app);
                Currlink << link << endl;
                Currname << nazwawaluty << endl;
                Currlink.close();
                Currname.close();
                cout << "zapisano" << endl;
                system("clear");
            }

            system("clear");
            Menu();

            break;
        }
        case 2:
        {
            bool ad = true; //czy chcesz nadal edytowac
            system("clear");
            while (ad == true)
            {
                system("clear");
                cout << "Dostępne Waluty do edytowania " << endl;
                for (int i = 0; i < NazwyWalut.size(); i++)
                {
                    cout << i << " " << NazwyWalut[i] << " ";
                }
                cout << endl;
                int wyborv; //wybor waluty do edytowania
                cout << "wybierz walute do edytowania " << endl;
                cin >> wyborv;
                cout << "wybrana waluta to " << NazwyWalut[wyborv] << endl;
                AktywnoscWalut[wyborv] = 0;
                cout << "zmień nazwe " << endl;
                string nazwwaluty;
                cin >> nazwwaluty;
                NazwyWalut[wyborv] = nazwwaluty;
                cout << "zmień link do waluty" << endl;
                cin >> LinkWalut[wyborv];
                renline(wyborv, LinkWalut[wyborv], NazwyWalut[wyborv]);

                cout << "chcesz edytować jeszcze jakąś walute? (1-tak,0-nie)" << endl;
                cin >> ad;
                cin.clear(); // Wyczyść bity błędów
                cout << endl;
                system("clear");
            }
            system("clear");
            Menu();
            break;
        }
        case 3:
        {
            system("clear");
            bool ed = true;
            while (ed == true)
            {
                cout << "Dostępne Waluty do usunięcia " << endl;
                for (int i = 0; i < NazwyWalut.size(); i++)
                {
                    cout << i << " " << NazwyWalut[i] << " ";
                }
                cout << endl;
                int wyborv; //wybor waluty do edytowania
                cout << "wybierz walute do usunięcia " << endl;
                cin >> wyborv;
                NazwyWalut.erase(NazwyWalut.begin() + wyborv);
                AktywnoscWalut.erase(AktywnoscWalut.begin() + wyborv);
                cout << "Usunięto walute z serwera:  " << NazwyWalut[wyborv] << endl;
                cout << "chcesz usunąć jeszcze jakąś walute? (1-tak,0-nie)" << endl;
                remline(wyborv);
                cin >> ed;
                cin.clear(); // Wyczyść bity błędów
                cout << endl;
                system("clear");

            }
            system("clear");
            Menu();
            break;
        }
        case 4:
        {
            Menu();
            break;
        }
        }
    }
};
void KryptoStat()
{

}
int main()
{

    int SendPort = 12345;
    int SendBufferSize;
    int targetip;
    UI ui;
    ui.Menu();
    //pthread_t thread1, thread2;

    // Create two threads and assign functions to them
    //pthread_create(&thread1, nullptr, Send(SendPort,SendBufferSize, ), nullptr);

    // Wait for both threads to finish
   // pthread_join(thread1, nullptr);
    //pthread_join(thread2, nullptr);

}

