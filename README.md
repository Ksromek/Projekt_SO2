# Projekt 1:
## Instrukcja uruchomienia projektu:
## Opis problemu:
Problem ucztujących filozofów (Dining Philosophers Problem) to klasyczny problem synchronizacji współbieżnych procesów, opisany przez Edsgera Dijkstrę w 1965 roku. Służy jako przykład problemu alokacji zasobów oraz synchronizacji wielu procesów lub wątków w systemach operacyjnych i programowaniu współbieżnym.
### Opis sytuacji problemowej:
Pięciu filozofów siedzi przy okrągłym stole, a pomiędzy nimi znajduje się pięć widelców. Każdy filozof ma dostęp do dwóch sąsiadujących widelców, które są mu niezbędne do jedzenia. Filozofowie wykonują cyklicznie dwie czynności:
1. Myślą – nie korzystają z widelców.
2. Jedzą – wymagają dwóch widelców jednocześnie.

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









