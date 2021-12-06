#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <wiringPi.h>
#include <netdb.h> // for getnameinfo()

// Usual socket headers
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <arpa/inet.h>

#define SIZE 1024
#define BACKLOG 10 // Passed to listen()

#define OUTPUT 1
#define INPUT 0

#define LOW 0
#define HIGH 1

typedef struct
{
    int gpio_number;
    int header_pin;
    int type;
    int value;
} Rpi_pin;

Rpi_pin pin_list[] = {
    [0].gpio_number = 17,
    [0].header_pin = 11,
    [0].type = OUTPUT,
    [0].value = LOW,

    [1].gpio_number = 27,
    [1].header_pin = 13,
    [1].type = OUTPUT,
    [1].value = LOW,

    [2].gpio_number = 22,
    [2].header_pin = 15,
    [2].type = OUTPUT,
    [2].value = LOW,

    [3].gpio_number = 23,
    [3].header_pin = 16,
    [3].type = OUTPUT,
    [3].value = LOW,

    [4].gpio_number = 23,
    [4].header_pin = 16,
    [4].type = OUTPUT,
    [4].value = LOW,

    [5].gpio_number = 23,
    [5].header_pin = 16,
    [5].type = OUTPUT,
    [5].value = LOW,

    [6].gpio_number = 23,
    [6].header_pin = 16,
    [6].type = OUTPUT,
    [6].value = LOW,

    [7].gpio_number = 24,
    [7].header_pin = 18,
    [7].type = INPUT,
    [7].value = LOW,

    [8].gpio_number = 25,
    [8].header_pin = 22,
    [8].type = INPUT,
    [8].value = LOW,
};

void report(struct sockaddr_in *serverAddress);

void setHttpHeader(char httpHeader[])
{
    printf("setting header?\n");
    // File object to return
    FILE *htmlData = fopen("../index.html", "r");

    printf("opened file?\n");

    char line[100];
    char responseData[8000];
    while (fgets(line, 100, htmlData) != 0)
    {
        if (strstr(line, "<ul dir=\"rtl\" class=\"left\">"))
        {
            strcat(responseData, line);

            for (int i = 1; i <= 39; i = i + 2)
            {
                /* code */
                char buff[120];
                int index = is_duplicate(pin_list, i);
                if (index >= 0)
                {
                    switch (pin_list[index].type)
                    {
                    case INPUT:
                        sprintf(buff, "<li class=\"input\"><i>GPIO %d:</i><small>%d</small></li>",
                                pin_list[index].gpio_number, digitalRead(pin_list[index].gpio_number), i);
                        break;

                    case OUTPUT:
                        sprintf(buff, "<li class=\"output\"><i>GPIO %d: %d</i><small>%d</small></li>",
                                pin_list[index].gpio_number, digitalRead(pin_list[index].gpio_number), i);
                        break;
                    }
                }
                else
                {

                    sprintf(buff, "<li><small>%d</small></li>", i);
                }
                strcat(responseData, buff);
            }
        }

        else if (strstr(line, "<ul class=\"right\">"))
        {
            printf("found ul2\n");
            strcat(responseData, line);

            for (int i = 2; i <= 40; i = i + 2)
            {
                /* code */
                char buff[120];
                int index = is_duplicate(pin_list, i);
                if (index > 0)
                {
                    switch (pin_list[index].type)
                    {
                    case INPUT:
                        sprintf(buff, "<li class=\"input\"><small>%d</small><i>GPIO %d: %d</i></li>", i, pin_list[index].gpio_number, digitalRead(pin_list[index].gpio_number));
                        break;

                    case OUTPUT:
                        sprintf(buff, "<li class=\"output\"><small>%d</small><i>GPIO %d: %d</i></li>", i, pin_list[index].gpio_number, digitalRead(pin_list[index].gpio_number));
                        break;
                    }
                }
                else
                {

                    sprintf(buff, "<li><small>%d</small></li>", i);
                }
                strcat(responseData, buff);
            }
        }

        else
        {
            strcat(responseData, line);
        }
    }
    strcat(httpHeader, responseData);
}

int is_duplicate(Rpi_pin *p, int header_pin)
{
    int i;
    int num_items = sizeof(pin_list) / sizeof(pin_list[0]);
    for (i = 0; i < num_items; i++)
    {
        // printf("%d VS %d\n", pin_list[i].header_pin, header_pin);
        if (p[i].header_pin == header_pin)
        {
            return i;
        }
    }
    return -1;
}

int main(void)
{
    char httpHeader[9000] = "HTTP/1.1 200 OK\r\n\n";

    wiringPiSetupGpio();

    // Socket setup: creates an endpoint for communication, returns a descriptor
    // -----------------------------------------------------------------------------------------------------------------
    int serverSocket = socket(
        AF_INET,     // Domain: specifies protocol family
        SOCK_STREAM, // Type: specifies communication semantics
        0            // Protocol: 0 because there is a single protocol for the specified family
    );

    // Construct local address structure
    // -----------------------------------------------------------------------------------------------------------------
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8001);
    serverAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK); //inet_addr("127.0.0.1");

    // Bind socket to local address
    // -----------------------------------------------------------------------------------------------------------------
    // bind() assigns the address specified by serverAddress to the socket
    // referred to by the file descriptor serverSocket.
    bind(
        serverSocket,                      // file descriptor referring to a socket
        (struct sockaddr *)&serverAddress, // Address to be assigned to the socket
        sizeof(serverAddress)              // Size (bytes) of the address structure
    );

    // Mark socket to listen for incoming connections
    // -----------------------------------------------------------------------------------------------------------------
    int listening = listen(serverSocket, BACKLOG);
    if (listening < 0)
    {
        printf("Error: The server is not listening.\n");
        return 1;
    }
    report(&serverAddress);    // Custom report function
    setHttpHeader(httpHeader); // Custom function to set header
    int clientSocket;

    // Wait for a connection, create a connected socket if a connection is pending
    // -----------------------------------------------------------------------------------------------------------------
    while (1)
    {
        clientSocket = accept(serverSocket, NULL, NULL);
        send(clientSocket, httpHeader, sizeof(httpHeader), 0);
        close(clientSocket);
    }
    return 0;
}

void report(struct sockaddr_in *serverAddress)
{
    char hostBuffer[INET6_ADDRSTRLEN];
    char serviceBuffer[NI_MAXSERV]; // defined in `<netdb.h>`
    socklen_t addr_len = sizeof(*serverAddress);
    int err = getnameinfo(
        (struct sockaddr *)serverAddress,
        addr_len,
        hostBuffer,
        sizeof(hostBuffer),
        serviceBuffer,
        sizeof(serviceBuffer),
        NI_NUMERICHOST);
    if (err != 0)
    {
        printf("It's not working!!\n");
    }
    printf("Server listening on http://%s:%s\n\n", hostBuffer, serviceBuffer);
}