#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"

const char* ELEMENTS[] = {"Fire", "Water", "Nature"};

const char* get_random_element() {
    return ELEMENTS[rand() % 3];
}

void generate_skill_name(char *buffer, char *element, char *base_name) {
    // Formats string like "Fire Blast" or "Water Slash"
    sprintf(buffer, "%s %s", element, base_name);
}

int calculate_damage(int base_damage, char *attacker_element, char *defender_element) {
    float multiplier = 1.0;

    if (strcmp(attacker_element, "Fire") == 0) {
        if (strcmp(defender_element, "Nature") == 0) multiplier = 2.0;
        if (strcmp(defender_element, "Water") == 0) multiplier = 0.5;
    }
    else if (strcmp(attacker_element, "Water") == 0) {
        if (strcmp(defender_element, "Fire") == 0) multiplier = 2.0;
        if (strcmp(defender_element, "Nature") == 0) multiplier = 0.5;
    }
    else if (strcmp(attacker_element, "Nature") == 0) {
        if (strcmp(defender_element, "Water") == 0) multiplier = 2.0;
        if (strcmp(defender_element, "Fire") == 0) multiplier = 0.5;
    }

    return (int)(base_damage * multiplier);
}