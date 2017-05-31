#ifndef DANE
#define DANE

#include "ksiazka.h"

Dane* tabDanych[1000];

extern const char* names[];
extern const char* surnames[];
extern const char* mails[];
extern const char* adress[];
extern const char* tel[];
extern const char* birth[];

Dane* daneCreate(int n);
void wygenerujDane();

#endif // DANE
