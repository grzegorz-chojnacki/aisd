# Pomiary czasu algorytmu quicksort
 - Ilość powtórzeń pomiaru czasu wynosi 2 * rozmiar tablicy
 - Stała 'c' == 20

## Dane losowe:
Rozmiar tablicy [N] | Algorytm podstawowy [s] | Algorytm zmodyfikowany [s]
 :--- | ---: | ---:
 500 | 0.000078 | 0.000109
1000 | 0.000234 | 0.000305
2000 | 0.000516 | 0.001207
2500 | 0.000684 | 0.002503

## Dane niekorzystne:
Rozmiar tablicy [N] | Algorytm podstawowy [s] | Algorytm zmodyfikowany [s]
 :--- | ---: | ---:
 500 | 0.000844 | 0.000922
1000 | 0.003109 | 0.003977
2000 | 0.011984 | 0.016336
2500 | 0.019569 | 0.026047