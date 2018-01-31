#include "mbed.h"
 
DigitalOut trigger(p6);
DigitalOut myled(LED1); //monitor trigger
DigitalOut myled2(LED2); //monitor echo
InterruptIn echo1(p7);
InterruptIn echo2(p8);
Serial pc(USBTX, USBRX);
int distance = 0;
int correction = 0;
Timer sonar1_timer;
Timer sonar2_timer;
int echo1_delay = 0;
int echo2_delay = 0;

bool flag_echo1_ok = true;
bool flag_echo2_ok = true;

template <Timer *t, bool *echo_ok>
void echo_high()
{
	// Echo is high. Start timing. (Assume t has been reset)
	t->start();
	*echo_ok = false;
}

template <Timer *t, int *store, bool *echo_ok>
void echo_low()
{
	// Echo is low. Finish timing.
	t->stop();
	// Save value
	*store = t->read_us();
	// Clear the timer now.
	t->reset();
	*echo_ok = true;
}

class interrupt_guard
{
public:
	interrupt_guard(InterruptIn &ii) : ii_(ii)
	{
		ii.disable_irq();
	}
	~interrupt_guard()
	{
		ii_.enable_irq();
	}
private:
	InterruptIn &ii_;
};

 
int main()
{
	echo1.rise(&echo_high<&sonar1_timer, &flag_echo1_ok>);
	echo2.rise(&echo_high<&sonar2_timer, &flag_echo2_ok>);
	echo1.fall(&echo_low<&sonar1_timer, &echo1_delay, &flag_echo1_ok>);
	echo2.fall(&echo_low<&sonar2_timer, &echo2_delay, &flag_echo2_ok>);

 
//Loop to read Sonar distance values, scale, and print
    while(1) {
// trigger sonar to send a ping
    	{
    		interrupt_guard g1(echo1);
    		interrupt_guard g2(echo2);

	        trigger = 1;
	        wait_us(10.0);
	        trigger = 0;
	        wait_us(50.0);
	    }

        wait(0.2);

        pc.printf(" %d cm (1) \n\r", echo1_delay );
        pc.printf(" %d cm (2) \n\r", echo2_delay );

        // while (! (flag_echo1_ok && flag_echo2_ok)) ;
    }
}