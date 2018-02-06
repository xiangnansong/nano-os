#include "main.h"
#include "alarm.h"


//
#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

extern FILE *fdevopen(int (*__put)(char, FILE *), int (*__get)(FILE *));

int serial_putc(char c, FILE *) {
    sputc((uint8_t) c);
    return c;
}

void printf_begin(void) {
    fdevopen((int (*)(char, FILE *)) serial_putc, 0);
}
////注意Drawable.h不要放在main.h中，因为main.h 是用 c 编译，用 c++ 工具链编译的头文件不要放在
//#include "Drawable.h"
//
//extern uint8_t uBuffer[BUFLEN + 2];
//
//int main() {
//    static TP_STATE* TP_State;
//    init();
//    Drawable drawable;
//    ltdc_clear(WHITE);
//    drawable.draw();
//    ltdc_draw_picture(100,100,64,64,gImage_alarm);
////    ltdc_draw_picture(0,0,240,320,doula);
////    ltdc_fill(20, 20, 100, 100, RED);
////    ltdc_draw_line(20, 20, 120, 120, BLACK);
////    ltdc_draw_font(50, 50, 's', 16);
////    ltdc_draw_string(60, 60, (uint8_t *) "hello, song!faslkjfkasjfklajflkasjflkajfklaj");
//    LED_GREEN = 1;
//    LED_RED = 0;
//    while (1) {
//        TP_State = IOE_TP_GetState();
//        uint16_t x = TP_State->X;
//        uint16_t y = TP_State->Y;
//        if(x>5&&x<235&&y>5&&y<315) {
//            ltdc_draw_point(TP_State->X, TP_State->Y, BLACK);
//        }
//        if (uBuffer[BUFLEN + 1]) {
//            printf("you say: \n");
//            send(uBuffer);
//            ltdc_fill(0, 0, 240, 320, BLACK);
//            ltdc_draw_string(20, 20, uBuffer);
//        }
////        printf("song\n");
////        LED_GREEN = 1;
////        LED_RED = 0;
////        delay_ms(500);
////        LED_GREEN = 0;
////        LED_RED = 1;
////        delay_ms(500);
//    }
//    return 0;
//}
//
//void init(void) {
//    //初始化内核
//    clock_init(8, 360, 2, 4);
//    delay_init(180);
////    timer_init(500);
//    //初始化板载LED
//    exti_init(GPIO_A, 0, RTIR);
//    nvic_init(2, 2, EXTI0_IRQn, 2);
////    nvic_init(3, 3, SysTick_IRQn, 2);
//    usart_init(9600);
//    ltdc_init();
//    sdram_init();
//    IOE_Config();
//    RCC->AHB1ENR |= 1 << 6;
//    RCC->AHB1ENR |= 1 << 0;
//    gpio_set(GPIOG, PIN13 | PIN14, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU);
//    gpio_set(GPIOA, PIN0, GPIO_MODE_IN, 0, 0, GPIO_PUPD_PD);
//
//}
//
//
//void timer_init(uint16_t arr) {
//    RCC->APB1ENR |= 1 << 1;
//    TIM3->ARR = --arr;
//    TIM3->PSC = 90000 - 1;
//    TIM3->DIER |= 0x01;
//    TIM3->CR1 |= 0x01;
//    nvic_init(1, 3, TIM3_IRQn, 2);
//
//
//}
//



//#define HW32_REG(ADDRESS) (*((volatile unsigned int *)(ADDRESS)))
//volatile uint32_t systick_count = 0;
//long long task0_stack[1024], task1_stack[1024];
//uint32_t curr_task = 0;
//uint32_t next_task = 0;
//uint32_t PSP_array[4];

void task0();

void task1();

void print_task();

void receive_task();

extern uint32_t systick_count;

uint8_t test_a, test_b;

extern uint8_t uBuffer[BUFLEN + 2];

static TP_STATE *TP_State;

void LED_Init() {

    RCC->AHB1ENR |= 1 << 6;
    RCC->AHB1ENR |= 1 << 0;
    gpio_set(GPIOG, PIN13 | PIN14, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU);
    gpio_set(GPIOA, PIN0, GPIO_MODE_IN, 0, 0, GPIO_PUPD_PD);

};

int main() {

    enter_critical();
    clock_init(8, 360, 2, 4);
    systick_init(CLOCK);
    SCB->CCR |= SCB_CCR_STKALIGN_Msk;
    LED_Init();
    os_init();
    exti_init(GPIO_A, 0, RTIR);
//    nvic_init(2, 2, EXTI0_IRQn, 2);
    nvic_init(1, 0, EXTI0_IRQn, 4);
    usart_init(9600);
    printf("os start\n");
    char *s = "os start";
    sprint(s);

    ltdc_init();
    sdram_init();
    IOE_Config();
    ltdc_clear(WHITE);
    ltdc_draw_picture(100, 100, 64, 64, gImage_alarm);

    task_create((void *(*)(void *)) task0, 0, 0);
    task_create((void *(*)(void *)) task1, 0, 1);
    task_create((void *(*)(void *)) print_task, 0, 2);
    task_create((void *(*)(void *)) receive_task, 0, 3);
    exit_critical();
    os_start();
}

void task0() {
    uint8_t flag = 0;
    while (1) {
        test_a = 100;
        test_b = 200;
        PGout(13) = ~PGout(13);
        if (flag) {
            ltdc_fill(0, 0, 240, 150, RED);
            flag = 0;
        } else {
            ltdc_fill(0, 0, 240, 150, GREEN);
            flag = 1;
        }
        task_sleep(0, 500);

//        os_delay(10000);
        printf("task0: a+b = %d\n", test_a + test_b);
    }
}

void task1() {
    uint8_t flag = 0;

    while (1) {
        semaphore_post(3, 7);

        PGout(14) = ~PGout(14);
        test_a = 10;
        test_b = 20;
        if (flag) {
            ltdc_fill(0, 170, 240, 150, RED);
            flag = 0;
        } else {
            ltdc_fill(0, 170, 240, 150, GREEN);
            flag = 1;
        }
        task_sleep(1, 3000);
        printf("task1: a+b = %d\n", test_a + test_b);
    }
}

void print_task() {
    while (1) {
        printf("os running\n");
        task_sleep(2, 500);
    }
}

void receive_task() {
    while (1) {
//        task_suspend(3);
//        printf("still running\n");
        task_sleep(3, 200);
//        semaphore_request(3);
        uint8_t a = semaphore_request(3);
//        printf("you say: \n");
//        send(uBuffer);

        printf("received %d\n", a);
    }
}


PUTCHAR_PROTOTYPE {
    /* Place your implementation of fputc here */
    /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
    // HAL_UART_Transmit(&handleTypeDef, (uint8_t *)&ch, 1, 0xFFFF);
    sputc((uint8_t) ch);
    return ch;
}




