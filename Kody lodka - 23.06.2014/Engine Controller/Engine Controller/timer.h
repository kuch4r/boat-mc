//Dekalaracje funkcji

uint8_t Timer_new_tick(void);

uint16_t Get_timer(void);

void Init_Timer(void);

uint16_t Time_elapsed(uint16_t time_stamp);
uint16_t Get_timer_us();
uint16_t Time_elapsed_us(uint16_t time_stamp);
void Set_commutation_time(uint16_t delay);

uint16_t Time_difference_us(uint16_t time_1, uint16_t time_2);
void Set_error_time(uint16_t delay);
void Set_commutation_time_prev(uint16_t delay,uint16_t current_timer);
#define TIMER1_CLEAR_INTERRUPT_A TIFR1 |= (1<<OCF0A);
#define TIMER1_CLEAR_INTERRUPT_B TIFR1 |= (1<<OCF0B);

#define TURN_ON_COMMUTATION_INTERRUPT TIMSK1 |= (1<<OCIE1A);
#define TURN_OFF_COMMUTATION_INTERRUPT TIMSK1 &= ~(1<<OCIE1A);

#define TURN_ON_ERROR_INTERRUPT TIMSK1 |= (1<<OCIE1B);
#define TURN_OFF_ERROR_INTERRUPT TIMSK1 &= ~(1<<OCIE1B);
