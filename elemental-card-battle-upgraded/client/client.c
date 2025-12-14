#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "../server/cJSON.h" // Ensure this path matches your folder structure

#define SERVER_PORT 12345
#define BUFFER_SIZE 4096

// Helper to draw a text-based health bar
void print_health_bar(int current, int max) {
    if (max <= 0) max = 100; // Prevent division by zero
    int bars = (current * 20) / max; // 20 segments total
    
    printf("[");
    for(int i=0; i<20; i++) {
        if(i < bars) printf("=");
        else printf(" ");
    }
    printf("] %d/%d\n", current, max);
}

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // 1. IP Address Logic
    // Default to localhost (for testing on one PC)
    const char *server_ip = "127.0.0.1"; 
    
    // If the user provided an IP address argument, use that instead
    if (argc > 1) {
        server_ip = argv[1];
    }

    // 2. Create Socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        printf("\nInvalid address / Address not supported \n");
        return -1;
    }

    // 3. Connect to Server
    printf("Connecting to Server at %s...\n", server_ip);

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        printf("Make sure the server is running on %s and the firewall is open.\n", server_ip);
        exit(EXIT_FAILURE);
    }

    printf("Connected! Waiting for opponent...\n");

    // 4. Main Game Loop
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        
        // Read data from server
        int n = read(sockfd, buffer, sizeof(buffer)-1);
        if (n <= 0) {
            printf("Disconnected from server.\n");
            break;
        }
        buffer[n] = '\0';

        // CASE A: JSON Game State (Update UI)
        if (buffer[0] == '{') {
            cJSON *json = cJSON_Parse(buffer);
            if(json) {
                // Clear the terminal screen
                printf("\033[H\033[J"); 
                
                // Parse stats
                char *myElem = cJSON_GetObjectItem(json, "myElement")->valuestring;
                char *opElem = cJSON_GetObjectItem(json, "opElement")->valuestring;
                int myHp = cJSON_GetObjectItem(json, "myHp")->valueint;
                int opHp = cJSON_GetObjectItem(json, "opHp")->valueint;
                int maxHp = cJSON_GetObjectItem(json, "myMaxHp")->valueint;

                // Render UI
                printf("\n--- OPPONENT (%s) ---\n", opElem);
                print_health_bar(opHp, maxHp);
                
                printf("\n\n       VS       \n\n");

                printf("--- YOU (%s) ---\n", myElem);
                print_health_bar(myHp, maxHp);

                printf("\n--- YOUR SKILLS ---\n");
                cJSON *skills = cJSON_GetObjectItem(json, "skills");
                cJSON *s;
                cJSON_ArrayForEach(s, skills) {
                    printf("%d: %-20s (Dmg: %d)\n", 
                        cJSON_GetObjectItem(s, "index")->valueint,
                        cJSON_GetObjectItem(s, "name")->valuestring,
                        cJSON_GetObjectItem(s, "damage")->valueint
                    );
                }
                
                if(cJSON_IsTrue(cJSON_GetObjectItem(json, "isMyTurn"))) {
                    printf("\n>>> IT IS YOUR TURN! <<<\n");
                } else {
                    printf("\n(Waiting for opponent...)\n");
                }
                cJSON_Delete(json);
            }
        }
        // CASE B: Server requesting input
        else if (strstr(buffer, "Select Skill") != NULL) {
            int move;
            // Loop until valid input is entered locally (simple check)
            while(1) {
                printf("Enter skill number (0-2): ");
                if (scanf("%d", &move) == 1 && move >= 0 && move <= 2) {
                    break;
                }
                // Clear input buffer if invalid
                while(getchar() != '\n'); 
                printf("Invalid input. Please enter 0, 1, or 2.\n");
            }
            
            // Send just the number as a string
            char msg[16];
            sprintf(msg, "%d", move);
            send(sockfd, msg, strlen(msg), 0);
        }
        // CASE C: Regular text message (Game Logs/Results)
        else {
            printf("%s", buffer);
        }
    }

    close(sockfd);
    return 0;
}