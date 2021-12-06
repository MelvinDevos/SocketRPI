// #include <stdio.h>
// #include <time.h>
// #include <wiringPi.h>
// #include "ud_ucase.h"

// // A function that terminates when enter key is pressed
// void fun()
// {
//     printf("fun() starts \n");
//     printf("Press enter to stop fun \n");
//     while (1)
//     {
//         if (getchar())
//             break;
//     }
//     printf("fun() ends \n");
// }

// // The main program calls fun() and measures time taken by fun()
// int main()
// {
//     // wiringPiSetupGpio();
//     // pinMode(27, OUTPUT) ;
//     // // pinMode(24, INPUT) ;
//     // // pinMode(25, INPUT) ;
//     // // Calculate the time taken by fun()
//     // int t;
//     // t = millis();
//     // fun();
//     // t = millis() - t;

//     // digitalWrite(27, 0);
//     // printf("DR: %d\n", digitalRead(27));
//     // printf("DR: %d\n", digitalRead(24));
//     // printf("DR: %d\n", digitalRead(25));

//     // printf("fun() took %d millis to execute \n", t);

//     return 0;
// }

#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include "ud_ucase.h"
#include <wiringPi.h>

pthread_mutex_t lock;
int j;
Rpi_pin pin_list[MAX_ITEMS];
int pin_list_lenght = 0;

void process1()
{
    // pthread_mutex_lock(&lock);
    struct sockaddr_un svaddr, claddr;
    int sfd, j;
    ssize_t numBytes;
    socklen_t len;
    char buf[BUF_SIZE];

    sfd = socket(AF_UNIX, SOCK_DGRAM, 0); /* Create server socket */
    if (sfd == -1)
        errExit("socket");

    /* Construct well-known address and bind server socket to it */

    /* For an explanation of the following check, see the erratum note for
       page 1168 at http://www.man7.org/tlpi/errata/. */

    if (strlen(SV_SOCK_PATH) > sizeof(svaddr.sun_path) - 1)
        fatal("Server socket path too long: %s", SV_SOCK_PATH);

    if (remove(SV_SOCK_PATH) == -1 && errno != ENOENT)
        errExit("remove-%s", SV_SOCK_PATH);

    memset(&svaddr, 0, sizeof(struct sockaddr_un));
    svaddr.sun_family = AF_UNIX;
    strncpy(svaddr.sun_path, SV_SOCK_PATH, sizeof(svaddr.sun_path) - 1);

    if (bind(sfd, (struct sockaddr *)&svaddr, sizeof(struct sockaddr_un)) == -1)
        errExit("bind");

    /* Receive messages, convert to uppercase, and return to client */

    for (;;)
    {
        Rpi_pin received_data;
        len = sizeof(struct sockaddr_un);
        numBytes = recvfrom(sfd, &received_data, sizeof(received_data), 0,
                            (struct sockaddr *)&claddr, &len);
        if (numBytes == -1)
            errExit("recvfrom");

        printf("test\n\n");
        printf("Received IO: %zd: periode %zdms\n", received_data.io_number, received_data.period);
        /*FIXME: above: should use %zd here, and remove (long) cast */

        // add received data to pin_list
        int index = is_duplicate(pin_list, received_data, &pin_list_lenght);
        if (index >= 0)
        {
            printf("Index duplicate: %d\n", index);
            pin_list[index].period = received_data.period;
        }
        else
        {
            add_item(pin_list, received_data, &pin_list_lenght);
        }

        print_struct(pin_list, &pin_list_lenght);

        if (sendto(sfd, &received_data, sizeof(received_data), 0, (struct sockaddr *)&claddr, len) !=
            numBytes)
            fatal("sendto");
    }
    // pthread_mutex_unlock(&lock);
}
void process2()
{
    // pthread_mutex_lock(&lock);
    wiringPiSetupGpio();

    while (1)
    {

        if (pin_list_lenght > 0)
        {
            int last_index = pin_list_lenght - 1;
            for (int i = 0; i <= last_index; i++)
            {
                //printf("Last toggle: %d, periode: %d, now: %d\n", pin_list[i].last_toggle, pin_list[i].period, millis());
                if (pin_list[i].last_toggle + pin_list[i].period < millis())
                {
                    pinMode(pin_list[i].io_number, OUTPUT);
                    printf("Item: %d aansturen\n", i, pin_list[i].io_number);
                    pin_list[i].last_toggle = millis();
                    digitalWrite(pin_list[i].io_number, !digitalRead(pin_list[i].io_number));
                }
            }
        }
    }

    // pthread_mutex_unlock(&lock);
}

int main(void)
{
    int err;
    pthread_t t1, t2;

    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("Mutex initialization failed.\n");
        return 1;
    }

    j = 0;

    pthread_create(&t1, NULL, process1, NULL);
    pthread_create(&t2, NULL, process2, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;
}