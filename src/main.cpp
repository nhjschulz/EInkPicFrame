#include <avr/io.h>
#include <avr/delay.h>
int main(int argc, char** argv)
{
    
    DDRB |= (1<<PB0);
    PORTB &= ~(1<<PB0);
    PORTB |= (1<<PB0);
    for(;;)
    {
        PORTB &= ~(1<<PB0);
        _delay_ms(500);
           
        PORTB |= (1<<PB0);
        _delay_ms(500);

    }

    return 0;
}