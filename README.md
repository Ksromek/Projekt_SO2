# Projekt 1 (Kamil Sromek):
## Instrukcja uruchomienia projektu:
Należy uruchomić plik wykonywalny Projekt1.exe, program zapyta o liczbę filozofów, którą należy podać w konsoli oraz zatwierdzić ENTEREM. Program uruchomi się, a następnie zacznie pracować w nieskończonej pętli.
## Opis problemu:
Problem jedzących filozofów (Dining Philosophers Problem) to klasyczny problem synchronizacji współbieżnych procesów, opisany przez Edsgera Dijkstrę w 1965 roku. Służy jako przykład problemu alokacji zasobów oraz synchronizacji wielu procesów lub wątków w systemach operacyjnych i programowaniu współbieżnym.
### Opis sytuacji problemowej:
Pięciu filozofów siedzi przy okrągłym stole, a pomiędzy nimi znajduje się pięć widelców. Każdy filozof ma dostęp do dwóch widelców, które są mu niezbędne do jedzenia. Filozofowie wykonują cyklicznie dwie czynności:
1. **Myślą** – nie korzystają z widelców.
2. **Jedzą** – korzystają z dwóch widelców jednocześnie.

Każdy filozof może podnieść tylko jeden widelec naraz. Jeśli drugi widelec jest zajęty przez sąsiada, filozof musi czekać, aż stanie się dostępny.
### Główne problemy do rozwiązania:
Należy sterować synchronizacją dostępu do zasobów (widelców) w taki sposób, aby uniknąć następujących problemów:
1. **Deadlock** (zakleszczenie) - może wystąpić, jeśli wszyscy filozofowie jednocześnie podniosą jeden widelec i będą czekać na drugi. W takim przypadku żaden z nich nigdy nie będzie w stanie dokończyć jedzenia, co prowadzi do stanu wzajemnej blokady.
2. **Starvattion** (zagłodzenie) - może wystąpić, gdy niektórzy filozofowie nigdy nie dostaną dostępu do dwóch widelców z powodu złej strategii przydzielania zasobów. Jeśli pewien filozof jest stale pomijany przez innych (np. szybsi filozofowie częściej podnoszą widelce), może nigdy nie zjeść.
## Wątki:
- Wątki filozofów: `philosopher(int id)`
- Każdy filozof reprezentowany jest przez oddzielny wątek.
  - Filozof myśli przez określony czas.
  - Następnie próbuje podnieść dwa widelce (operacje wejścia do sekcji krytycznej).
  - Jeśli uda mu się podnieść oba widelce, zaczyna jeść.
  - Po jedzeniu odkłada widelce (operacje wyjścia z sekcji krytycznej) i wraca do myślenia.
## Sekcje krytyczne i ich rozwiązania:
### Sekcje krytyczne:
- Podnoszenie widelców:
  - Filozof musi uzyskać dostęp do dwóch współdzielonych zasobów (widelców).
- Odkładanie widelców:
  - Filozof zwalnia używane przez siebie zasoby, aby inni filozofowie mogli je wykorzystać.
### Rozwiązanie problemu sekcji krytycznej:
- Mechanizm semaforów binarnych (implementowany jako zmienne ` forks[i] ` kontrolowane przez ` wait() ` i ` signal() `).
- Zasada asymetrii – ostatni filozof podnosi najpierw prawy widelec, a następnie lewy, co zapobiega zakleszczeniu.

# Projekt 2 (Kamil Sromek i Michał Głuszkiewicz):
## Instrukcja uruchomienia projektu:
Najpierw należy uruchomić serwer (server.exe), który będzie nasłuchiwał połączeń na porcie 12345. Następnie można uruchomić dowolną liczbę klientów (client.exe), podając przy starcie nazwę użytkownika. Programy komunikują się za pomocą gniazd TCP (IPv4, lokalnie – 127.0.0.1 lub przez sieć LAN).
## Opis problemu:
Aplikacje czatowe muszą obsługiwać jednoczesnych użytkowników i zapewniać im bezpieczną oraz płynną komunikację w czasie rzeczywistym. Projekt rozwiązuje problem podstawowej komunikacji tekstowej pomiędzy wieloma klientami za pośrednictwem jednego serwera.
Klienci mogą wymieniać się wiadomościami globalnie (wszyscy widzą) lub wysyłać prywatne wiadomości do wybranych użytkowników. Dodatkowo, po dołączeniu nowy użytkownik otrzymuje historię czatu, aby znać kontekst rozmowy.
### Opis sytuacji problemowej:
Aplikacja czatu musi:
- Obsługiwać dynamicznie dołączających i rozłączających się użytkowników.
- Przechowywać dane o klientach (np. nazwy użytkowników) i zapewniać do nich bezpieczny dostęp.
- Reagować na różne polecenia tekstowe użytkownika (np. `/list` , `/private`, ` exit `).
- Zachować historię czatu i udostępniać ją nowym klientom.
- Obsłużyć sytuację, gdy klient niespodziewanie się rozłączy. 

Wszystkie te funkcje muszą być realizowane w sposób bezpieczny wielowątkowo, co wymusza użycie mechanizmów synchronizacji.
### Główne problemy do rozwiązania:
1. **Obsługa wielu klientów jednocześnie:**
- Każdy klient musi być obsługiwany w osobnym wątku serwera.
- Wątki te muszą działać niezależnie, bez zakłócania pracy innych klientów.
2. **Synchronizacja dostępu do zasobów współdzielonych:**
- Lista klientów, mapa nazw użytkowników oraz plik z logami chatu muszą być chronione przed równoczesnym zapisem lub odczytem przez wiele wątków.
3. **Realizacja funkcji czatu:**
- Wiadomości ogólne (widoczne dla wszystkich).
- Wiadomości prywatne (z użyciem komendy `/private`).
- Lista aktywnych użytkowników (`/list`).
- Historia rozmów przesyłana do nowo połączonych klientów.
4. **Bezpieczne zakończenie sesji:**
- Obsługa komendy `exit`.
- Usunięcie klienta z listy połączeń i zwolnienie zasobów.
## Wątki:
1. **Po stronie klienta:**
- *Wątek główny:* obsługuje wprowadzanie wiadomości przez użytkownika.
- *Wątek pomocniczy:* `receiveMessages()` – odbiera wiadomości od serwera w tle i wyświetla je.
2. **Po stronie serwera:**
- Dla każdego klienta tworzony jest nowy wątek `handleClient()`, który:
  - Rejestruje nazwę użytkownika.
  - Odbiera wiadomości.
  - Obsługuje komendy `/list` i `/private`.
  - Wysyła historię czatu po dołączeniu.
  - Wysyła wiadomości do innych klientów (broadcast).
- Dzięki temu każdy klient działa niezależnie i nie blokuje innych.
## Sekcje krytyczne i ich rozwiązania:
### Sekcje krytyczne:
1. Lista połączonych klientów (`clients`) – musi być chroniona podczas dodawania/usuwania gniazd.
2. Mapa nazw użytkowników (`usernames`) – każda zmiana lub odczyt nazwy wymaga synchronizacji.
3. Plik `chat_log.txt` – odczytywany przez nowych klientów i zapisywany przy każdej wiadomości.
### Rozwiązanie problemu sekcji krytycznej:
- `std::mutex clientsMutex`: chroni listę gniazd i mapę `usernames`.
- `std::mutex fileMutex`: chroni dostęp do pliku logów.
Użyto `std::lock_guard<std::mutex>` jako RAII-locka, który automatycznie odblokowuje mutex po zakończeniu zakresu.

## Kompilacja:
### Serwer
C:\mingw64\bin\g++ server.cpp -o server.exe -lws2_32
### Client
C:\mingw64\bin\g++ client.cpp -o client.exe -lws2_32



