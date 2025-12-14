#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "game.h"
#include "cJSON.h"

#define PORT 12345
#define MAX_CLIENTS 2

// Load generic skills from JSON
int load_skills(const char *filename, Skill skills_out[], int max) {
    FILE *f = fopen(filename, "r");
    if(!f) { printf("Error: Could not open %s\n", filename); return 0; }

    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *data = (char*)malloc(len + 1);
    fread(data, 1, len, f);
    data[len] = '\0';
    fclose(f);

    cJSON *json = cJSON_Parse(data);
    free(data);
    
    if(!json) return 0;

    int count = 0;
    cJSON *item = NULL;
    cJSON_ArrayForEach(item, json) {
        if(count >= max) break;
        cJSON *name = cJSON_GetObjectItem(item, "name");
        cJSON *dmg = cJSON_GetObjectItem(item, "base_damage");
        
        if (name && dmg) {
            strcpy(skills_out[count].base_name, name->valuestring);
            skills_out[count].damage = dmg->valueint;
            count++;
        }
    }
    cJSON_Delete(json);
    return count;
}

// NEW: Helper function to shuffle an array (Fisher-Yates algorithm)
void shuffle_array(int *array, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

// NEW: Assign 3 UNIQUE random skills to a player
void assign_random_skills(Player *p, Skill *pool, int pool_size) {
    // Create an array of indices [0, 1, 2, ... pool_size-1]
    int *indices = malloc(pool_size * sizeof(int));
    for(int i=0; i<pool_size; i++) indices[i] = i;

    // Shuffle them to get random order
    shuffle_array(indices, pool_size);

    // Pick the first 3
    for(int s=0; s<3; s++) {
        // Copy the skill from the pool using the shuffled index
        p->skills[s] = pool[indices[s]];
        
        // Rename it based on player's element
        generate_skill_name(p->skills[s].display_name, 
                            p->element, 
                            p->skills[s].base_name);
    }
    free(indices);
}

// Send current battle state to client
void send_game_state(int sockfd, Player *me, Player *opponent, int is_turn) {
    cJSON *root = cJSON_CreateObject();
    
    cJSON_AddNumberToObject(root, "myHp", me->hp);
    cJSON_AddNumberToObject(root, "myMaxHp", MAX_HP);
    cJSON_AddStringToObject(root, "myElement", me->element);
    
    cJSON_AddNumberToObject(root, "opHp", opponent->hp);
    cJSON_AddNumberToObject(root, "opMaxHp", MAX_HP);
    cJSON_AddStringToObject(root, "opElement", opponent->element);
    
    cJSON_AddBoolToObject(root, "isMyTurn", is_turn);

    cJSON *skillsArray = cJSON_CreateArray();
    for(int i=0; i<MAX_SKILLS; i++) {
        cJSON *s = cJSON_CreateObject();
        cJSON_AddNumberToObject(s, "index", i);
        cJSON_AddStringToObject(s, "name", me->skills[i].display_name);
        cJSON_AddNumberToObject(s, "damage", me->skills[i].damage);
        cJSON_AddItemToArray(skillsArray, s);
    }
    cJSON_AddItemToObject(root, "skills", skillsArray);

    char *json_str = cJSON_PrintUnformatted(root);
    char message[4096];
    snprintf(message, sizeof(message), "%s\n", json_str); 
    
    send(sockfd, message, strlen(message), 0);
    
    free(json_str);
    cJSON_Delete(root);
}

int main() {
    srand(time(NULL));
    
    // --- Socket Setup ---
    int server_fd, clients[MAX_CLIENTS];
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd == 0) { perror("socket failed"); exit(1); }
    
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if(bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed"); exit(1);
    }

    if(listen(server_fd, MAX_CLIENTS) < 0) {
        perror("listen failed"); exit(1);
    }

    printf("Wizard Duel Server Started on Port %d\nWaiting for 2 players...\n", PORT);

    for(int i=0; i<MAX_CLIENTS; i++) {
        clients[i] = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        printf("Player %d Connected!\n", i+1);
    }

    // --- Game Initialization ---
    Player players[2];
    Skill all_skills[20];
    int total_loaded = load_skills("data/skills.json", all_skills, 20);
    if(total_loaded < 3) {
        printf("Error: Not enough skills in JSON file!\n");
        return 1;
    }

    // Initial setup (HP and Elements only)
    for(int i=0; i<2; i++) {
        players[i].id = i;
        players[i].hp = MAX_HP;
        strcpy(players[i].element, get_random_element());
        printf("Player %d assigned %s element.\n", i+1, players[i].element);
    }

    // --- Game Loop ---
    int turn = 0;
    while(players[0].hp > 0 && players[1].hp > 0) {
        int p_idx = turn % 2;
        int op_idx = 1 - p_idx;
        
        // --- NEW: RANDOMIZE SKILLS EVERY TURN ---
        // Both players get fresh skills for the new round
        assign_random_skills(&players[0], all_skills, total_loaded);
        assign_random_skills(&players[1], all_skills, total_loaded);
        // ----------------------------------------
        
        // 1. Send State to both
        for(int i=0; i<2; i++) {
            send_game_state(clients[i], &players[i], &players[1-i], (i == p_idx));
        }

        usleep(100000); // Prevent sticky packets

        // 2. Ask Current Player for Move
        send(clients[p_idx], "Select Skill\n", 13, 0);

        // 3. Receive Input
        char buffer[64];
        int n = recv(clients[p_idx], buffer, 63, 0);
        if(n <= 0) break;
        buffer[n] = 0;

        int skill_idx = atoi(buffer);
        if(skill_idx < 0 || skill_idx >= 3) skill_idx = 0; 

        // 4. Calculate Damage
        Skill *used_skill = &players[p_idx].skills[skill_idx];
        int damage = calculate_damage(used_skill->damage, players[p_idx].element, players[op_idx].element);
        
        players[op_idx].hp -= damage;
        if(players[op_idx].hp < 0) players[op_idx].hp = 0;

        // 5. Broadcast Results
        char log_msg[256];
        sprintf(log_msg, "\n> %s used %s!\n> Dealt %d damage! (Opponent HP: %d)\n\n", 
            (p_idx == 0 ? "Player 1" : "Player 2"), 
            used_skill->display_name, 
            damage, 
            players[op_idx].hp);
            
        send(clients[0], log_msg, strlen(log_msg), 0);
        send(clients[1], log_msg, strlen(log_msg), 0);

        sleep(1); 
        turn++;
    }

    char result[128];
    if(players[0].hp > 0) sprintf(result, "GAME OVER! Player 1 Wins!\n");
    else sprintf(result, "GAME OVER! Player 2 Wins!\n");

    for(int i=0; i<2; i++) send(clients[i], result, strlen(result), 0);
    
    close(clients[0]);
    close(clients[1]);
    close(server_fd);
    return 0;
}