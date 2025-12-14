#ifndef GAME_H
#define GAME_H

#define MAX_HP 100
#define MAX_SKILLS 3

typedef struct {
    char base_name[30];    // e.g., "Blast"
    char display_name[50]; // e.g., "Fire Blast"
    int damage;
} Skill;

typedef struct {
    int id;
    int hp;
    char element[20];
    Skill skills[MAX_SKILLS]; 
} Player;

// Function declarations
const char* get_random_element();
void generate_skill_name(char *buffer, char *element, char *base_name);
int calculate_damage(int base_damage, char *attacker_element, char *defender_element);

#endif