// // Main program to build the application
// // Has main(); does initialization and cleanup and perhaps some basic logic.

// #include <stdio.h>
// #include <stdbool.h>
// #include "badmath.h"
// #include "hal/button.h"
// #include "draw_stuff.h"

// void foo() {
//     int data[3];    
//     for (int i = 0; i <= 3; i++) {
//         data[i] = 10;
//         printf("Value: %d\n", data[i]);
//     }
// }

// int main()
// {
//     printf("Hello world with LCD!\n");

//     // Initialize all modules; HAL modules first
//     button_init();
//     badmath_init();
//     DrawStuff_init();

//     // Main program logic:
//     #if 0
//     for (int i = 0; i < 10; i++) {
//         printf("  -> Reading button time %d = %d\n", i, button_is_button_pressed());
//     }

//     for (int i = 0; i <= 35; i++) {
//         int ans = badmath_factorial(i);
//         printf("%4d! = %6d\n", i, ans);
//     }
//     #endif

//     for (int i = 0; i < 100; i++) {
//         char buff[1024];
//         snprintf(buff, 1024, "Hello count: %3d", i);
//         DrawStuff_updateScreen(buff);
//     }
    

//     // Cleanup all modules (HAL modules last)
//     DrawStuff_cleanup();
//     badmath_cleanup();
//     button_cleanup();

//     printf("!!! DONE !!!\n"); 

//     // Some bad code to try out and see what shows up.
//     #if 0
//         // Test your linting setup
//         //   - You should see a warning underline in VS Code
//         //   - You should see compile-time errors when building (-Wall -Werror)
//         // (Linting using clang-tidy; see )
//         int x = 0;
//         if (x = 10) {
//         }
//     #endif
//     #if 0
//         // Demonstrate -fsanitize=address (enabled in the root CMakeFiles.txt)
//         // Compile and run this code. Should see warning at compile time; error at runtime.
//         foo();

//     #endif
// }

/*
	State machine for GPIO
*/

// Relies on the gpiod library.
// Insallation for cross compiling:
//      (host)$ sudo dpkg --add-architecture arm64
//      (host)$ sudo apt update
//      (host)$ sudo apt install libgpdiod-dev:arm64
// GPIO: https://www.ics.com/blog/gpio-programming-exploring-libgpiod-library
// Example: https://github.com/starnight/libgpiod-example/blob/master/libgpiod-input/main.c

#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

#include "gpio.h"
#include "btn_statemachine.h"
#include "hal/lightSensor.h"
#include <time.h>

int main(void) 
{
    // // Startup & Initialization
    // Gpio_initialize();
    // BtnStateMachine_init();

    // // TESTING State machine
    // while (true) {
    //     // TODO: This should be on it's own thread!
    //     BtnStateMachine_doState();

    //     printf("Counter at %+5d\n", BtnStateMachine_getValue());
    // }

    // BtnStateMachine_cleanup();
    // Gpio_cleanup();

    // printf("\nDone!\n");
    // return 0;
    Sampler_init();
    while (true) {
        printf("yes\n");
        sleep(1);
    }
}
